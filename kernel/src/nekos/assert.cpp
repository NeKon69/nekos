#include "nekos/assert.hpp"

#include "nekos/kprintf.hpp"

namespace nekos {
[[noreturn]] void assertFail(const char *Expression, const char *File,
                             uint32_t Line, const char *Function) {
  kprintf("assertion failed: %s\n", const_cast<char *>(Expression));
  kprintf("at %s:%u in %s\n", const_cast<char *>(File), Line,
          const_cast<char *>(Function));
  asm volatile("int $254");
  for (;;)
    asm volatile("hlt");
}
} // namespace nekos
