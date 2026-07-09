#pragma once

#include <stdint.h>

namespace nekos {
[[noreturn]] void assertFail(const char *Expression, const char *File,
                             uint32_t Line, const char *Function);
}

#ifndef NDEBUG
#define nekosAssert(Expression)                                               \
  ((Expression)                                                               \
       ? static_cast<void>(0)                                                 \
       : ::nekos::assertFail(#Expression, __FILE__, __LINE__, __func__))
#else
#define nekosAssert(Expression) static_cast<void>(0)
#endif
