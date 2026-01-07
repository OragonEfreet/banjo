#include <HOOPSArgParser.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// dest == 0 ; action == 0: "A does-nothing argument"
// dest != 0 ; action == 0: flag, automatic storage of 1 or 0 into dest
// dest == 0 ; action != 0: non-storing arg with immediate exit.
// dest != 0 ; action != 0: argument with attached value and custom data type
// Only the later requires the user to type a value
static int ArgExpectsValue(const HOOPSArgParserArgument* arg) {
    assert(arg);
    return arg->dest != 0 && arg->action != 0;
}

static int ArgIsPositional(const HOOPSArgParserArgument* arg) {
    assert(arg);
    return arg->shortname == 0 && (arg->name == 0 || arg->name[0] == '\0');
}

static int ArgValidate(const HOOPSArgParserArgument* arg, char* buffer, size_t n, size_t index) {

    // Validate: flag arguments must not have metavar
    if(!ArgIsPositional(arg) && !ArgExpectsValue(arg) && arg->metavar != 0) {
        return snprintf(buffer, n, "argument expecting no-value must not have metavar, found '%s'", arg->metavar);
    }

    // Validate shortname: must be A-Z, a-z, or 0-9
    if (arg->shortname != '\0') {
        if (!(isalpha((unsigned char)arg->shortname) || isdigit((unsigned char)arg->shortname))) {
            return snprintf(buffer, n, "invalid shortname '%c' at argument %zu; must be A-Z, a-z, or 0-9", arg->shortname, index);
        }
    }

    // Validate long name
    if (arg->name != 0 && arg->name[0] != '\0') {
        // Must not start with '-'
        if (arg->name[0] == '-') {
            return snprintf(buffer, n, "argument name \"%s\" at index %zu cannot start with '-'", arg->name, index);
        }

        // Check each character
        for (const char* p = arg->name; *p != '\0'; ++p) {
            char c = *p;
            if (!(isalpha((unsigned char)c) || isdigit((unsigned char)c) || c == '-' || c == '_')) {
                return snprintf(buffer, n, "invalid character '%c' in argument name \"%s\" at index %zu; allowed: A-Z, a-z, 0-9, '-', '_'", c, arg->name, index);
            }
        }
    }

    return 0;
}

int HOOPSArgParserValidate(const HOOPSArgParser* parser, char* buffer, size_t n) {
    if (parser->arguments_len > 0 && parser->arguments == 0) {
        return snprintf(buffer, n, "arguments_len is %zu but arguments is 0", parser->arguments_len);
    }

    for (size_t i = 0; i < parser->arguments_len; ++i) {
        int err = ArgValidate(&parser->arguments[i], buffer, n, i);
        if (err != 0) {
            return err;
        }
    }

    // Check for duplicate shortnames
    for (size_t i = 0; i < parser->arguments_len; ++i) {
        char si = parser->arguments[i].shortname;
        if (si == '\0') continue;
        for (size_t j = i + 1; j < parser->arguments_len; ++j) {
            if (parser->arguments[j].shortname == si) {
                return snprintf(buffer, n, "duplicate shortname '%c' at arguments %zu and %zu", si, i, j);
            }
        }
    }

    // Check for duplicate names
    for (size_t i = 0; i < parser->arguments_len; ++i) {
        const char* name_i = parser->arguments[i].name;
        if (name_i == 0 || name_i[0] == '\0') {
            continue;
        }
        for (size_t j = i + 1; j < parser->arguments_len; ++j) {
            const char* name_j = parser->arguments[j].name;
            if (name_j == 0 || name_j[0] == '\0') {
                continue;
            }
            if (strcmp(name_i, name_j) == 0) {
                return snprintf(buffer, n, "duplicate name \"%s\" at arguments %zu and %zu", name_i, i, j);
            }
        }
    }

    // Check for duplicate dests (except 0)
    for (size_t i = 0; i < parser->arguments_len; ++i) {
        void* loc_i = parser->arguments[i].dest;
        if (loc_i == 0) continue;
        for (size_t j = i + 1; j < parser->arguments_len; ++j) {
            void* loc_j = parser->arguments[j].dest;
            if (loc_j == 0) continue;
            if (loc_i == loc_j) {
                return snprintf(buffer, n, "duplicate dest pointer at arguments %zu and %zu", i, j);
            }
        }
    }


    int found_optional_positional = 0;
    for (size_t i = 0; i < parser->arguments_len; ++i) {
        const HOOPSArgParserArgument* arg = &parser->arguments[i];
        if (ArgIsPositional(arg)) {
            if (!arg->required) {
                found_optional_positional = 1;
            } else if (found_optional_positional) {
                return snprintf(buffer, n, "required positional argument at index %zu follows optional one", i);
            }
        } else {
            if (arg->required) {
                if(arg->name && arg->name[0] != '\0') {
                    return snprintf(buffer, n, "named argument --%s cannot be required", arg->name);
                }
                return snprintf(buffer, n, "named argument -%c cannot be required", arg->shortname);
            }

        }
    }

    return 0;
}

static int VSPrintBuffer(char* buffer, size_t* n, const char* format, va_list args) {
    int written = vsnprintf(buffer, (n == 0 ? 0 : *n), format, args);
    if (written > 0 && n != 0) {
        *n -= (*n > (size_t)written) ? written : *n;
    }
    return written;
}


static int SPrintBuffer(char* buffer, size_t* n, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int res = VSPrintBuffer(buffer, n, format, args);
    va_end(args);
    return res;
}

static int SPrintArg(char* buffer, size_t* buffer_remaining, const HOOPSArgParserArgument* arg, const char* before, const char* after, int full) {
    assert(arg);
    size_t total_size       = 0;

    if(before) {
        total_size += SPrintBuffer(buffer+total_size, buffer_remaining, before);
    }

    const int short_name = arg->shortname != 0;
    const int long_name  = arg->name && (full || !short_name);

    if(!ArgIsPositional(arg)) {
        if (short_name) {
            total_size += SPrintBuffer(buffer + total_size, buffer_remaining, "-%c", arg->shortname);
            total_size += SPrintBuffer(buffer + total_size, buffer_remaining, long_name ? ", " : "");
        }

        if (long_name) {
            total_size += SPrintBuffer(buffer + total_size, buffer_remaining, "--%s", arg->name);
        }
        if(ArgExpectsValue(arg)) {
            total_size += SPrintBuffer(buffer+total_size, buffer_remaining, " ", arg->metavar);
        }
    }

    if (ArgExpectsValue(arg)) {
        const char* metavar = arg->metavar;
        
        if (!metavar) {
            if (arg->name) {
                metavar = arg->name;
            } else if (arg->shortname) {
                static char short_buf[2] = {0};  // local static buffer for 1-char shortname
                short_buf[0] = arg->shortname;
                metavar = short_buf;
            } else {
                metavar = "VALUE";
            }
        }

        assert(metavar && *metavar);

        for (const char* p = metavar; *p; ++p) {
            total_size += SPrintBuffer(buffer + total_size, buffer_remaining, "%c", toupper((unsigned char)*p));
        }
    }

    if(after) {
        total_size += SPrintBuffer(buffer+total_size, buffer_remaining, after);
    }

    return total_size;
}

static int SPrintUsage(char* buffer, size_t* buffer_remaining, const HOOPSArgParser* parser) {
    if(parser == 0) {return 0;}
    size_t total_size = 0;

    total_size += SPrintBuffer(buffer+total_size, buffer_remaining, "Usage: ");

    if(parser->prog != 0) {
        total_size += SPrintBuffer(buffer+total_size, buffer_remaining, "%s", parser->prog);
    }

    size_t n_positional = 0;
    for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
        const HOOPSArgParserArgument* arg = &parser->arguments[a];
        total_size += SPrintBuffer(buffer+total_size, buffer_remaining, " ");
        const char* before = arg->required ? 0 : "[";
        const char* after = arg->required ? 0 : "]";
        total_size += SPrintArg(buffer+total_size, buffer_remaining, arg, before, after, 0);
    }

    return total_size;
}

static int SPrintHelp(char* buffer, size_t n, const HOOPSArgParser* parser) {
    if(parser == 0) {return 0;}

    size_t total_size = HOOPSArgParserValidate(parser, buffer, n);

    if(total_size > 0) {
        size_t buffer_remaining = n < total_size ? 0 : n - total_size;
        total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");
        return total_size;
    }

    size_t buffer_remaining = n;

    total_size += SPrintUsage(buffer, &buffer_remaining, parser);
    total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");

    if(parser->description) {
        total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n%s\n", parser->description);
    }
    total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");

    if(parser->arguments_len > 0) {
        assert(parser->arguments);

        int gutter = 0;
        for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
            const HOOPSArgParserArgument* arg = &parser->arguments[a];
            if(ArgIsPositional(arg)) {
                const int strsize = SPrintArg(0, 0, arg, " ", "", 1);
                if(gutter < strsize) {
                    gutter = strsize;
                }
            }
        }
        gutter += 1;

        for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
            const HOOPSArgParserArgument* arg = &parser->arguments[a];
            if(ArgIsPositional(arg)) {
                const size_t strsize = SPrintArg(buffer+total_size, &buffer_remaining, arg, " ", "", 1);
                total_size += strsize;
                if(arg->help) {
                    total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "%*s%s", gutter - strsize, "", arg->help);
                }
                total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");
            }
        }

        gutter = 0;
        for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
            const HOOPSArgParserArgument* arg = &parser->arguments[a];
            if(!ArgIsPositional(arg)) {
                const int strsize = SPrintArg(0, 0, arg, " ", "", 1);
                if(gutter < strsize) {
                    gutter = strsize;
                }
            }
        }
        gutter += 1;

        total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\noptions:\n");
        for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
            const HOOPSArgParserArgument* arg = &parser->arguments[a];
            if(!ArgIsPositional(arg)) {
                const size_t strsize = SPrintArg(buffer+total_size, &buffer_remaining, arg, " ", "", 1);
                total_size += strsize;
                if(arg->help) {
                    total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "%*s%s", gutter - strsize, "", arg->help);
                }
                total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");
            }
        }

        total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");
    }

    if(parser->epilog) {
        total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "%s\n", parser->epilog);
    }

    return total_size;
}

int HOOPSArgParserFilePrintHelp(FILE* stream, const HOOPSArgParser* parser) {
    const int strsize = SPrintHelp(0, 0, parser);
    if(strsize) {
        char buffer[strsize+1];
        SPrintHelp(buffer, strsize+1, parser);
        fprintf(stream, "%s", buffer);
    }
    return strsize;
}

static const int VSPrintError(char* buffer, size_t n, const HOOPSArgParser* parser, const char* format, ...) {
    size_t buffer_remaining = n;
    size_t total_size = 0;
    va_list args;
    va_start(args, format);
    total_size += VSPrintBuffer(buffer, &buffer_remaining, format, args);
    va_end(args);
    total_size += SPrintBuffer(buffer+total_size, &buffer_remaining, "\n");
    total_size += SPrintUsage(buffer+total_size, &buffer_remaining, parser);
    return total_size;
}

static const HOOPSArgParserArgument* LookupShortArg(const HOOPSArgParser* parser, char shortname) {
    if(parser == 0) { return 0; }
    if(parser->arguments == 0) {return 0;}

    for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
        const HOOPSArgParserArgument* arg = &parser->arguments[a];
        if(shortname != 0 && arg->shortname == shortname) {
            return arg;
        }
    }
    return 0;
}


static const HOOPSArgParserArgument* LookupArg(const HOOPSArgParser* parser, const char* name, size_t len) {
    if (parser == 0 || parser->arguments == 0) return 0;

    for (size_t a = 0; a < parser->arguments_len; ++a) {
        const HOOPSArgParserArgument* arg = &parser->arguments[a];
        const char* b = arg->name;
        if (name == b) return arg;
        if (!name || !b) continue;

        size_t i;
        for (i = 0; i < len; ++i) {
            unsigned char ca = (unsigned char)name[i];
            unsigned char cb = (unsigned char)b[i];

            if (ca != cb) break;
            if (ca == '\0') break;
        }
        if (i == len || name[i] == '\0') {
            return arg;
        }
    }
    return 0;
}

int ProcessArg(const HOOPSArgParser* parser, const HOOPSArgParserArgument* arg, const char* value, char* buffer, size_t n) {

    if(arg->action) {
        char err[128] = {};
        int errsize = arg->action(parser, arg, value, arg->dest, err, 128);
        if(errsize > 0) {
            return VSPrintError(buffer, n, parser, "while parsing parameter: %s", err);
        }
    } else {
        if(arg->dest) {
            *((int*)arg->dest) = 1;
        }
    }

    if(arg->dest == 0) {
        exit(0);
    }

    return 0;
}


int HOOPSArgParserBuild_SN(HOOPSArgParser* parser, int argc, char* argv[], char* buffer, size_t n) {

#define TRY_PROCESS_ARG(arg, value) {const int errsize = ProcessArg(parser, arg, value, buffer, n); if(errsize > 0) {return errsize;}}

    const int errlen = HOOPSArgParserValidate(parser, buffer, n);
    if(errlen > 0) {
        return errlen;
    }

    size_t n_positional = 0;
    size_t n_positional_required = 0;

    for(size_t a = 0 ; a < parser->arguments_len ; ++a) {
        const HOOPSArgParserArgument* arg = &parser->arguments[a];
        if(ArgIsPositional(arg)) {
            ++n_positional;
            if(arg->required) {
                ++n_positional_required;
            }
        }
    }

    enum {
        FSM_IDLE,
        FSM_EXPECT_SHORTNAME,
        FSM_EXPECT_SHORTNAMES,
        FSM_EXPECT_LONGNAME,
        FSM_EXPECT_VALUE,
    } state = {};

    const HOOPSArgParserArgument* current_arg = 0;
    size_t n_positional_found = 0; // Number of positional argument we found
                                   // in argv

    for(size_t i = 1 ; i < argc ; ++i) {
        const char* token = argv[i];
        size_t str_start = 0;
        
        switch(state) {

            case FSM_IDLE:
                for(size_t c = 0 ; c < strlen(token) ; ++c) {
                    const char t = token[c];
                    switch(state) {
                        case FSM_IDLE:
                            if(t == '-') {
                                state = FSM_EXPECT_SHORTNAME;
                            } else {
                                assert(current_arg == 0);
                                const size_t positional_index = n_positional_found;
                                if(++n_positional_found > n_positional) {
                                    return VSPrintError(buffer, n, parser, "expected %ld positional arguments, got %d", n_positional, n_positional_found);
                                }

                                // Search for the positional argument


                                size_t skip_positional = n_positional_found;
                                for(size_t a = 0; a < parser->arguments_len ; ++a) {
                                    const HOOPSArgParserArgument* arg = &parser->arguments[a];
                                    if(ArgIsPositional(arg) && --skip_positional == 0) {
                                        current_arg = arg;
                                        break;
                                    }
                                }
                                assert(current_arg != 0);
                                state = FSM_EXPECT_VALUE;

                                /* size_t next_arg = n_positional_found; */
                                /* for(size_t a = 0 ; a < parser->arguments_len ; ++a) { */
                                /*     const HOOPSArgParserArgument* arg = &parser->arguments[a]; */
                                /*     if(ArgIsPositional(arg)) { */
                                /*         if(--next_arg == 0) { */
                                /*             TRY_PROCESS_ARG(arg, token); */
                                /*         } */
                                /*     } */
                                /* } */
                            }
                            break;

                        case FSM_EXPECT_SHORTNAMES:
                        case FSM_EXPECT_SHORTNAME:
                            if(t == '-' && state == FSM_EXPECT_SHORTNAME) {
                                state = FSM_EXPECT_LONGNAME;
                                str_start = c+1;
                            } else {
                                if(!isalnum(t)) {
                                    return VSPrintError(buffer, n, parser, "unexpected non alphanum character '%c'", t);
                                }
                                if(state == FSM_EXPECT_SHORTNAMES && current_arg != 0) {
                                    TRY_PROCESS_ARG(current_arg, 0); // No value, it's a flag
                                    current_arg = 0;
                                }
                                current_arg = LookupShortArg(parser, t);
                                if(current_arg == 0) {
                                    return VSPrintError(buffer, n, parser, "invalid short parameter -%c", t);
                                }
                                if(ArgExpectsValue(current_arg)) {
                                    // If we're not in the end of the token,
                                    // it means the next characters contains the
                                    // value, otherwise, it'll be in next work.
                                    if(c < strlen(token) - 1) {
                                        str_start = c+1;
                                        state = FSM_EXPECT_VALUE;
                                    } else {
                                        state = FSM_EXPECT_SHORTNAMES;
                                    }
                                } else {
                                    state = FSM_EXPECT_SHORTNAMES;
                                }
                            }
                            break;

                        case FSM_EXPECT_LONGNAME:
                            if(t == '=') {
                                assert(current_arg == 0);
                                current_arg = LookupArg(parser, token+str_start, c - str_start);
                                if(current_arg == 0) {
                                    return VSPrintError(buffer, n, parser, "invalid long parameter --%s", token+str_start);
                                }
                                // Finding and = while reading a parameter that doesn't accept values is an error.
                                if(!ArgExpectsValue(current_arg)) {
                                    return VSPrintError(buffer, n, parser, "assignment for non-value long parameter --%s", token+str_start);
                                }
                                str_start = c+1;
                                state = FSM_EXPECT_VALUE;
                            }
                            
                            break; // TODO



                        case FSM_EXPECT_VALUE:
                            break;
                    }
                }

                // End of token, process whatever state was ongoing
                // TODO This is the equivalent of having t == 0, so maybe this 
                // switch could be merged into the one above?
                switch(state) {
                    // Was reading a value. Submit it.
                    case FSM_EXPECT_VALUE:
                        assert(current_arg != 0);
                        TRY_PROCESS_ARG(current_arg, token+str_start);
                        current_arg = 0;
                        state = FSM_IDLE;
                        break;
                    // Was reading short names glues together.
                    case FSM_EXPECT_SHORTNAMES:
                        assert(current_arg != 0);
                        if(ArgExpectsValue(current_arg)) {
                            state = FSM_EXPECT_VALUE;
                        } else {
                            TRY_PROCESS_ARG(current_arg, 0); // No value, it's a flag
                            state = FSM_IDLE;
                            current_arg = 0;
                        }
                        break;
                    case FSM_EXPECT_SHORTNAME:
                        return VSPrintError(buffer, n, parser, "expected argument after '-'");
                        break;
                    case FSM_IDLE:
                        break; // Can happen is argument was empty?
                    case FSM_EXPECT_LONGNAME:
                        assert(current_arg == 0);
                        current_arg = LookupArg(parser, token+str_start, strlen(token+str_start));
                        if(current_arg == 0) {
                            return VSPrintError(buffer, n, parser, "invalid long parameter --%s", token+str_start);
                        }
                        if(ArgExpectsValue(current_arg)) {
                            state = FSM_EXPECT_VALUE;
                        } else {
                            TRY_PROCESS_ARG(current_arg, 0); // No value, it's a flag
                            state = FSM_IDLE;
                            current_arg = 0;
                        }

                        break;


                }
                break;

            case FSM_EXPECT_VALUE:
                assert(current_arg != 0);
                TRY_PROCESS_ARG(current_arg, token);
                state = FSM_IDLE;
                break;

            default:
                return VSPrintError(buffer, n, parser, "internal error");
        }
    }

    // Last consistency check
    switch(state) {
        case FSM_IDLE:
            break;
        case FSM_EXPECT_VALUE:
            assert(argc > 1);
            return VSPrintError(buffer, n, parser, "missing value after %s", argv[argc-1]);
        default:
            return VSPrintError(buffer, n, parser, "internal error");
    }

    // Check if all required positional arguments are set
    if(n_positional_found < n_positional_required) {
        return VSPrintError(buffer, n, parser, "required %ld positional arguments, got only %ld", n_positional_required, n_positional_found);
    }

    return 0;
#undef TRY_PROCESS_ARG
}

int HOOPSArgParserBuild(HOOPSArgParser* parser, int argc, char* argv[], FILE* output) {
    const int errsize = HOOPSArgParserBuild_SN(parser, argc, argv, 0, 0);
    if(errsize) {
        char errbuf[errsize+1];
        HOOPSArgParserBuild_SN(parser, argc, argv, errbuf, errsize+1);
        fprintf(output ? output : stderr, "%s\n", errbuf);
    }
    return errsize;
}


int HOOPSArgParserStoreCString(const struct HOOPSArgParser_T* parser, const struct HOOPSArgParserArgument_T* arg, const char* value, void* dest, char* buffer, size_t n) {
    if(value == 0) {
        return snprintf(buffer, n, "string value not found");
    }
    *(const char**)dest = value;
    return 0;
}

int HOOPSArgParserStoreDouble(const struct HOOPSArgParser_T* parser, const struct HOOPSArgParserArgument_T* arg, const char* value, void* dest, char* buffer, size_t n) {
    if(value == 0) {
        return snprintf(buffer, n, "float value not found");
    }
    char* end;
    *(double*)dest = strtod(value, &end);

    int invalid = (value == end);
    while (*end != '\0' && isspace((unsigned char)*end)) ++end;
    if (*end != '\0') {
        invalid = 1;
    }

    if (invalid) {
        return snprintf(buffer, n, "invalid integer value '%s'", value);
    }
    return 0;
}

int HOOPSArgParserStoreInt(const struct HOOPSArgParser_T* parser, const struct HOOPSArgParserArgument_T* arg, const char* value, void* dest, char* buffer, size_t n) {
    if(value == 0) {
        return snprintf(buffer, n, "int value not found");
    }

    char* endptr;
    errno = 0;

    long lval = strtol(value, &endptr, 0);
    if(errno == ERANGE || lval  < INT_MIN || lval > INT_MAX) {
        return snprintf(buffer, n, "out of range integer, expected [%d ; %d], got %s", INT_MIN, INT_MAX, value);
    }

    int invalid = (value == endptr);
    while (*endptr != '\0' && isspace((unsigned char)*endptr)) ++endptr;
    if (*endptr != '\0') {
        invalid = 1;
    }

    if (invalid) {
        return snprintf(buffer, n, "invalid integer value '%s'", value);
    }

    *(int*)dest = (int)lval;
    return 0;
}

int HOOPSArgParserStoreBool(const struct HOOPSArgParser_T* parser, const struct HOOPSArgParserArgument_T* arg,
                        const char* value, void* dest, char* buffer, size_t n) {
    if (value == NULL) {
        return snprintf(buffer, n, "boolean value not found");
    }

    if (strcasecmp(value, "true")   == 0
        || strcasecmp(value, "1")   == 0
        || strcasecmp(value, "yes") == 0
        || strcasecmp(value, "on")  == 0) {
        *(int*)dest            = 1;
        return 0;
    }

    if (strcasecmp(value, "false")  == 0
        || strcasecmp(value, "0")   == 0
        || strcasecmp(value, "no")  == 0
        || strcasecmp(value, "off") == 0) {
        *(int*)dest = 0;
        return 0;
    }

    return snprintf(buffer, n, "invalid boolean value '%s'", value);
}


int HOOPSArgParserStoreUInt(const struct HOOPSArgParser_T* parser, const struct HOOPSArgParserArgument_T* arg, const char* value, void* dest, char* buffer, size_t n) {
    if (value == 0) {
        return snprintf(buffer, n, "unsigned int value not found");
    }

    char* endptr;
    errno = 0;

    unsigned long ulval = strtoul(value, &endptr, 0);
    if (errno == ERANGE || ulval > UINT_MAX) {
        return snprintf(buffer, n, "out of range unsigned int, expected [0 ; %u], got %s", UINT_MAX, value);
    }

    int invalid = (value == endptr);
    while (*endptr != '\0' && isspace((unsigned char)*endptr)) ++endptr;
    if (*endptr != '\0') {
        invalid = 1;
    }

    if (invalid) {
        return snprintf(buffer, n, "invalid unsigned int value '%s'", value);
    }

    *(unsigned int*)dest = (unsigned int)ulval;
    return 0;
}

int HOOPSArgParserPrintHelp(const struct HOOPSArgParser_T* parser, const struct HOOPSArgParserArgument_T* arg, const char* value, void* dest, char* buffer, size_t n) {
    HOOPSArgParserFilePrintHelp(stdout, parser);
    return 0;
}

int HOOPSParseStandardArguments(HOOPSArgParserStandardArgs* args, int argc, char* argv[]) {
    return HOOPSParseExtendedArguments(args, argc, argv, 0);
}

int HOOPSParseExtendedArguments(HOOPSArgParserStandardArgs* args, int argc, char* argv[], HOOPSArgParser* user_parser) {
    if(!args) {
        return 0;
    }
    
    const HOOPSArgParserArgument standard_args[] = {
        {.shortname='h', .name = "help",     .help = "show this help", .action=HOOPSArgParserPrintHelp},
        {.shortname='v', .name = "verbose",  .help = "verbose mode",   .dest=&args->verbose},
    };
    const size_t standard_args_len = sizeof(standard_args) / sizeof(HOOPSArgParserArgument);

    // Build the final arg table
    const size_t parser_len = standard_args_len + (user_parser ? user_parser->arguments_len : 0);
    HOOPSArgParserArgument arguments[parser_len];

    for(size_t a = 0 ; a < parser_len ; ++a) {
        arguments[a] = (a < standard_args_len)
            ?  standard_args[a]
            :  user_parser->arguments[a-standard_args_len];
    }

    HOOPSArgParser parser = {
        .prog          = user_parser ? user_parser->prog : argv[0],
        .description   = user_parser ? user_parser->description : 0,
        .epilog        = user_parser ? user_parser->epilog : 0,
        .arguments_len = parser_len,
        .arguments     = arguments
    };

    if(HOOPSArgParserBuild(&parser, argc, argv, 0) > 0) {
        return 0;
    }

    return 1;
}
