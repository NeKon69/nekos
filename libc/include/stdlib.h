#ifndef NEKOS_LIBC_STDLIB_H
#define NEKOS_LIBC_STDLIB_H

#include <attributes.h>
#include <extern_c.h>
#include <stdint.h>

EXTERN_C_BEGIN

/// Converts the integer `value` into a string.
/// \param value The integer to convert.
/// \param str The buffer to write the string into (must be non-null).
/// \param base The base to use for the conversion (must be 2-36).
char *itoa(int value, char *str NEKOS_LIFETIMEBOUND [[gnu::nonnull]],
           int base);

/// Converts the unsigned integer `value` into a string.
/// \param value The integer to convert.
/// \param str The buffer to write the string into (must be non-null).
/// \param base The base to use for the conversion (must be 2-36).
char *utoa(uint32_t value, char *str NEKOS_LIFETIMEBOUND [[gnu::nonnull]],
           int base);

EXTERN_C_END

#endif
