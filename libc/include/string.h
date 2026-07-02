#ifndef NEKOS_LIBC_STRING_H
#define NEKOS_LIBC_STRING_H

#include <extern_c.h>
#include <stddef.h>

EXTERN_C_BEGIN

#ifndef NULL
#define NULL 0
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef __SIZE_TYPE__ size_t;
#endif

/// Returns the length of the string `str`.
/// \param str The string to get the length of (must be non-null).
size_t strlen(const char *str [[gnu::nonnull]]);

/// Copies the string `src` into the buffer `dst`.
/// \param dst The buffer to copy the string into (must be non-null).
/// \param src The string to copy (must be non-null).
char *strcpy(char *dst [[clang::lifetimebound]] [[gnu::nonnull]],
             const char *src [[gnu::nonnull]]);

/// Copies the first `n` characters of the string `src` into the buffer `dst`.
/// \param dst The buffer to copy the string into (must be non-null).
/// \param src The string to copy (must be non-null).
/// \param n The number of characters to copy.
char *strncpy(char *dst [[clang::lifetimebound]] [[gnu::nonnull]],
              const char *src [[gnu::nonnull]], size_t n);

/// Appends the string `src` to the end of the string `dest`.
/// \param dest The string to append to (must be non-null).
/// \param src The string to append (must be non-null).
char *strcat(char *dest [[clang::lifetimebound]] [[gnu::nonnull]],
             const char *src [[gnu::nonnull]]);

/// Appends the first `n` characters of the string `src` to the end of the
/// string `dest`.
/// \param dest The string to append to (must be non-null).
/// \param src The string to append (must be non-null).
/// \param n The number of characters to append.
char *strncat(char *dest [[clang::lifetimebound]] [[gnu::nonnull]],
              const char *src [[gnu::nonnull]], size_t n);

/// Compares the strings `s1` and `s2`.
/// \param s1 The first string to compare (must be non-null).
/// \param s2 The second string to compare (must be non-null).
int strcmp(const char *s1 [[gnu::nonnull]], const char *s2 [[gnu::nonnull]]);

/// Compares the first `n` characters of the strings `s1` and `s2`.
/// \param s1 The first string to compare (must be non-null).
/// \param s2 The second string to compare (must be non-null).
/// \param n The number of characters to compare.
int strncmp(const char *s1 [[gnu::nonnull]], const char *s2 [[gnu::nonnull]],
            size_t n);

/// Returns the index of the first occurrence of the character `ch` in the
/// string `str`.
/// \param str The string to search (must be non-null).
/// \param ch The character to search for.
char *strchr(const char *str [[clang::lifetimebound]] [[gnu::nonnull]], int ch);

/// Returns the index of the last occurrence of the character `ch` in the string
/// `str`.
/// \param str The string to search (must be non-null).
/// \param ch The character to search for.
char *strrchr(const char *str [[clang::lifetimebound]] [[gnu::nonnull]],
              int ch);

/// Returns the count of characters in the string `s1` that are in the string
/// `s2`.
/// \param str The string to search (must be non-null).
/// \param list The string to search for (must be non-null).
size_t strspn(const char *str [[gnu::nonnull]],
              const char *list [[gnu::nonnull]]);

/// Returns the count of characters in the string `s1` that are not in the
/// string `s2`.
/// \param str The string to search (must be non-null).
/// \param list The string to search for (must be non-null).
size_t strcspn(const char *str [[gnu::nonnull]],
               const char *list [[gnu::nonnull]]);

/// Returns the pointer to the first character in the string `dest` that is in
/// the string `breakset`.
/// \param dest The string to search (must be non-null).
/// \param breakset The string to search for (must be non-null).
char *strpbrk(const char *dest [[clang::lifetimebound]] [[gnu::nonnull]],
              const char *breakset [[gnu::nonnull]]);

/// Returns the pointer to the first occurrence of the string `substr` in the
/// string `str`.
/// \param str The string to search (must be non-null).
/// \param substr The string to search for (must be non-null).
char *strstr(const char *str [[clang::lifetimebound]] [[gnu::nonnull]],
             const char *substr [[gnu::nonnull]]);

EXTERN_C_END

#endif
