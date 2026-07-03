#ifndef NEKOS_LIBC_ATTRIBUTES_H
#define NEKOS_LIBC_ATTRIBUTES_H

#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x) 0
#endif

#ifndef __has_c_attribute
#define __has_c_attribute(x) 0
#endif

#if defined(__cplusplus) && __has_cpp_attribute(clang::lifetimebound)
#define NEKOS_LIFETIMEBOUND [[clang::lifetimebound]]
#elif !defined(__cplusplus) && __has_c_attribute(clang::lifetimebound)
#define NEKOS_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define NEKOS_LIFETIMEBOUND
#endif

#endif
