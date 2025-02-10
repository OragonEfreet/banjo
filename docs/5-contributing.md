# Contributing to Banjo {#contributing}

\brief Rules and guidelines

## Language

The code is entirely in _C99_.

### Header Files and Inclusions

Header files are all located directly within the _include/_ folder without any subfolder.

Header files must parse by themself without any other header.

A header file must only include the necessary headers that are needed to parse it.

The header file inclusion is grouped in different sections, each alphabetically sorted and separated by a blank line, in this order:

1. The header file corresponding to the current implementation file, if applicable.
2. Internal header files from Banjo API, using quotes `""` syntax.
3. Public header files from Banjo API
4. 3rd-party libraries header files.
5. Standard library headers







