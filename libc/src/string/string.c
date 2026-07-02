#include <stdbool.h>
#include <string.h>

size_t strlen(const char *str) {
  size_t len = 0;
  while (*str++)
    len++;
  return len;
}

char *strcpy(char *dst, const char *src) {
  char *ret = dst;
  while (*src)
    *dst++ = *src++;
  *dst = '\0';
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *ret = dst;
  while (n && *src) {
    *dst++ = *src++;
    n--;
  }
  while (n--)
    *dst++ = '\0';
  return ret;
}

char *strcat(char *dest, const char *src) {
  char *ret = dest;
  while (*dest)
    dest++;
  strcpy(dest, src);
  return ret;
}

char *strncat(char *dest, const char *src, size_t n) {
  char *ret = dest;
  while (*dest)
    dest++;
  size_t i = 0;
  while (i < n && src[i])
    *dest++ = src[i++];
  *dest = '\0';
  return ret;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s2 && *s1 == *s2)
    s1++, s2++;
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n--) {
    if (!*s1 || !*s2 || *s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }
  return 0;
}

char *strchr(const char *str, int ch) {
  while (true) {
    if (*str == ch)
      return (char *)str;
    if (!*str)
      return NULL;
    str++;
  }
  return NULL;
}

char *strrchr(const char *str, int ch) {
  const char *start = str;
  while (*str)
    str++;
  // Advance one further to account for the null terminator (it can be returned
  // when searching for occurences of a character).
  str++;
  while (str-- != start)
    if (*str == ch)
      return (char *)str;
  return NULL;
}

size_t strspn(const char *s1, const char *s2) {
  size_t count = 0;
  while (*s1 && strchr(s2, *s1))
    s1++, count++;
  return count;
}

size_t strcspn(const char *s1, const char *s2) {
  // Idk, we *could* just call strspn, then strlen on s1 and subtract from it,
  // but that would probably be less efficent, so whatever.
  size_t count = 0;
  while (*s1 && !strchr(s2, *s1))
    s1++, count++;
  return count;
}

char *strpbrk(const char *dest, const char *breakset) {
  while (*dest) {
    if (strchr(breakset, *dest))
      return (char *)dest;
    dest++;
  }
  return NULL;
}

char *strstr(const char *str, const char *substr) {
  if (!*substr)
    return (char *)str;
  while (*str) {
    if (!strncmp(str, substr, strlen(substr)))
      return (char *)str;
    str++;
  }
  return NULL;
}
