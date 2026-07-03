#include "vga/vga_assert.hpp"

// This test file may be moved to a separate dir if it stops using VGA. For now,
// keep it in here.

// --- plain text ---

void test_kprintf_plain_text(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("hello");
  NEKOS_EXPECT_VGA_TEXT("hello");
}

void test_kprintf_plain_empty(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("");
  NEKOS_EXPECT_VGA_TEXT("");
}

void test_kprintf_plain_spaces(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("hello world");
  NEKOS_EXPECT_VGA_TEXT("hello world");
}

// --- %s ---

void test_kprintf_s_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("str=%s", "abc");
  NEKOS_EXPECT_VGA_TEXT("str=abc");
}

void test_kprintf_s_empty(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("str=%s", "");
  NEKOS_EXPECT_VGA_TEXT("str=");
}

void test_kprintf_s_in_middle(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("before %s after", "mid");
  NEKOS_EXPECT_VGA_TEXT("before mid after");
}

// --- %c ---

void test_kprintf_c_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("c=%c", 'A');
  NEKOS_EXPECT_VGA_TEXT("c=A");
}

void test_kprintf_c_space(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("c=%c", ' ');
  NEKOS_EXPECT_VGA_TEXT("c= ");
}

void test_kprintf_c_digit(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("c=%c", '9');
  NEKOS_EXPECT_VGA_TEXT("c=9");
}

// --- %d ---

void test_kprintf_d_positive(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%d", 42);
  NEKOS_EXPECT_VGA_TEXT("n=42");
}

void test_kprintf_d_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%d", 0);
  NEKOS_EXPECT_VGA_TEXT("n=0");
}

void test_kprintf_d_negative(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%d", -10);
  NEKOS_EXPECT_VGA_TEXT("n=-10");
}

// --- %i ---

void test_kprintf_i_positive(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%i", 7);
  NEKOS_EXPECT_VGA_TEXT("n=7");
}

void test_kprintf_i_negative(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%i", -3);
  NEKOS_EXPECT_VGA_TEXT("n=-3");
}

// --- %u ---

void test_kprintf_u_positive(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%u", 99);
  NEKOS_EXPECT_VGA_TEXT("n=99");
}

void test_kprintf_u_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%u", 0);
  NEKOS_EXPECT_VGA_TEXT("n=0");
}

// --- %x ---

void test_kprintf_x_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%x", 255);
  NEKOS_EXPECT_VGA_TEXT("n=ff");
}

void test_kprintf_x_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%x", 0);
  NEKOS_EXPECT_VGA_TEXT("n=0");
}

void test_kprintf_x_large(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%x", 4096);
  NEKOS_EXPECT_VGA_TEXT("n=1000");
}

// --- %X ---

void test_kprintf_X_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%X", 255);
  NEKOS_EXPECT_VGA_TEXT("n=FF");
}

void test_kprintf_X_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%X", 0);
  NEKOS_EXPECT_VGA_TEXT("n=0");
}

// --- %o ---

void test_kprintf_o_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%o", 64);
  NEKOS_EXPECT_VGA_TEXT("n=100");
}

void test_kprintf_o_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("n=%o", 0);
  NEKOS_EXPECT_VGA_TEXT("n=0");
}

// --- %p ---

void test_kprintf_p_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("ptr=%p", (void *)0x1234);
  NEKOS_EXPECT_VGA_TEXT("ptr=0x1234");
}

void test_kprintf_p_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("ptr=%p", (void *)0x0);
  NEKOS_EXPECT_VGA_TEXT("ptr=0x0");
}

// --- %zu ---

void test_kprintf_zu_normal(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("sz=%zu", (size_t)16);
  NEKOS_EXPECT_VGA_TEXT("sz=16");
}

void test_kprintf_zu_zero(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("sz=%zu", (size_t)0);
  NEKOS_EXPECT_VGA_TEXT("sz=0");
}

// --- multiple specifiers ---

void test_kprintf_multiple_mixed(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("%s %c %d", "abc", 'Z', 10);
  NEKOS_EXPECT_VGA_TEXT("abc Z 10");
}

void test_kprintf_multiple_same(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("%d %d %d", 1, 2, 3);
  NEKOS_EXPECT_VGA_TEXT("1 2 3");
}

// --- zero values for all types ---

void test_kprintf_all_zeros(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("d=%d u=%u x=%x o=%o", 0, 0, 0, 0);
  NEKOS_EXPECT_VGA_TEXT("d=0 u=0 x=0 o=0");
}

// --- kputs ---

void test_kputs_plain(void) {
  NEKOS_VGA_TEST_BEGIN();
  kputs("hello");
  NEKOS_EXPECT_VGA_TEXT("hello");
}

void test_kputs_empty(void) {
  NEKOS_VGA_TEST_BEGIN();
  kputs("");
  NEKOS_EXPECT_VGA_TEXT("");
}

void test_kputs_spaces(void) {
  NEKOS_VGA_TEST_BEGIN();
  kputs("hello world");
  NEKOS_EXPECT_VGA_TEXT("hello world");
}

void test_kputs_single_char(void) {
  NEKOS_VGA_TEST_BEGIN();
  kputs("A");
  NEKOS_EXPECT_VGA_TEXT("A");
}

void test_kputs_long(void) {
  NEKOS_VGA_TEST_BEGIN();
  kputs("the quick brown fox jumps over the lazy dog");
  NEKOS_EXPECT_VGA_TEXT("the quick brown fox jumps over the lazy dog");
}

void test_kputs_special_chars(void) {
  NEKOS_VGA_TEST_BEGIN();
  kputs("!@#$%^&*()");
  NEKOS_EXPECT_VGA_TEXT("!@#$%^&*()");
}

// --- kclear ---

void test_kclear_clears_all_text(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("hello");
  kclear();
  NEKOS_EXPECT_VGA_TEXT("");
}

void test_kclear_after_multiple_outputs(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("first");
  kputs("second");
  kclear();
  NEKOS_EXPECT_VGA_TEXT("");
}

void test_kclear_multiple_times(void) {
  NEKOS_VGA_TEST_BEGIN();
  kprintf("hello");
  kclear();
  kclear();
  kclear();
  NEKOS_EXPECT_VGA_TEXT("");
}

namespace tests {

void runKprintfTests() {
  test_kprintf_plain_text();
  test_kprintf_plain_empty();
  test_kprintf_plain_spaces();
  test_kprintf_s_normal();
  test_kprintf_s_empty();
  test_kprintf_s_in_middle();
  test_kprintf_c_normal();
  test_kprintf_c_space();
  test_kprintf_c_digit();
  test_kprintf_d_positive();
  test_kprintf_d_zero();
  test_kprintf_d_negative();
  test_kprintf_i_positive();
  test_kprintf_i_negative();
  test_kprintf_u_positive();
  test_kprintf_u_zero();
  test_kprintf_x_normal();
  test_kprintf_x_zero();
  test_kprintf_x_large();
  test_kprintf_X_normal();
  test_kprintf_X_zero();
  test_kprintf_o_normal();
  test_kprintf_o_zero();
  test_kprintf_p_normal();
  test_kprintf_p_zero();
  test_kprintf_zu_normal();
  test_kprintf_zu_zero();
  test_kprintf_multiple_mixed();
  test_kprintf_multiple_same();
  test_kprintf_all_zeros();
  test_kputs_plain();
  test_kputs_empty();
  test_kputs_spaces();
  test_kputs_single_char();
  test_kputs_long();
  test_kputs_special_chars();
  test_kclear_clears_all_text();
  test_kclear_after_multiple_outputs();
  test_kclear_multiple_times();
}

} // namespace tests
