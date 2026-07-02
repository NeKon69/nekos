#ifndef NEKOS_LIBC_STRING_H
#define NEKOS_LIBC_STRING_H

#include <stddef.h>
#include <extern_c.h>

EXTERN_C_BEGIN

size_t strlen(const char *str [[gnu::nonnull]]);

EXTERN_C_END

#endif
