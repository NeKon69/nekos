#ifndef NEKOS_LIBC_STRING_H
#define NEKOS_LIBC_STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t strlen(const char *str [[gnu::nonnull]]);

#ifdef __cplusplus
}
#endif

#endif
