#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Des fonctions utiles
static const char* fname(const char* path) {
#ifdef _MSC_VER
    const char* fptr = strrchr(path, '/');  // For Unix-like paths
#else
    const char* fptr = strrchr(path, '\\'); // For Windows paths
#endif
    if (fptr == 0) { fptr = path; } else { fptr++; }
    return fptr;
}

#ifndef ON_EXIT
#   define ON_EXIT(...) return EXIT_FAILURE
#endif

#ifndef PRINT
#   define PRINT(...) printf(__VA_ARGS__)
#endif

#define ASSERT(cond) do { \
    if(!(cond)) { \
        PRINT("%s:%d: FAILED: ASSERT( %s )\n", fname(__FILE__), __LINE__, #cond);  \
        ON_EXIT(cond); \
    }\
} while(0)
