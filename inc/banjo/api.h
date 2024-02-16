#pragma once

#ifndef NDEBUG
#  include <assert.h>
#endif
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BJ_DEFINE_HANDLE(object) typedef struct object##_T* object

#define BJ_NULL_HANDLE 0

#define BJ_VERSION_MAJOR 0
#define BJ_VERSION_MINOR 0
#define BJ_VERSION_PATCH 1

#define BJ_MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

#define BJ_VERSION BJ_MAKE_VERSION(BJ_VERSION_MAJOR, BJ_VERSION_MINOR, BJ_VERSION_PATCH)

#define BJ_NAME "Banjo"

#ifdef BANJO_STATIC
#  define BANJO_EXPORT
#  define BANJO_NO_EXPORT
#else
#  ifndef BANJO_EXPORT
#    ifdef BANJO_EXPORTS
        /* We are building this library */
#      define BANJO_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define BANJO_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef BANJO_NO_EXPORT
#    define BANJO_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

typedef char      byte;
typedef uint16_t  c16;
typedef float     f32;
typedef double    f64;
typedef int8_t    i8;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef ptrdiff_t size;
typedef size_t    usize;
typedef uintptr_t uptr;

#ifdef NDEBUG
#    define bjAssert(cond)
#else
#    define bjAssert(cond) assert(cond)
#endif

