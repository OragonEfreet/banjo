#ifndef BJ_POSIX_H
#define BJ_POSIX_H

/* Must be included before any system header in files that use POSIX APIs.
 * Instructs glibc to expose POSIX declarations hidden under -std=c99. */
#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE

#endif
