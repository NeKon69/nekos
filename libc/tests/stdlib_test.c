#include "stdlib.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

// --- zero ---

void test_itoa_zero_base10(void) {
  char buf[32];
  itoa(0, buf, 10);
  TEST_ASSERT_EQUAL_STRING("0", buf);
}

void test_itoa_zero_base16(void) {
  char buf[32];
  itoa(0, buf, 16);
  TEST_ASSERT_EQUAL_STRING("0", buf);
}

// --- positive numbers ---

void test_itoa_positive_base10(void) {
  char buf[32];
  itoa(42, buf, 10);
  TEST_ASSERT_EQUAL_STRING("42", buf);
}

void test_itoa_one(void) {
  char buf[32];
  itoa(1, buf, 10);
  TEST_ASSERT_EQUAL_STRING("1", buf);
}

void test_itoa_large_positive(void) {
  char buf[32];
  itoa(12345, buf, 10);
  TEST_ASSERT_EQUAL_STRING("12345", buf);
}

void test_itoa_single_digit(void) {
  char buf[32];
  itoa(7, buf, 10);
  TEST_ASSERT_EQUAL_STRING("7", buf);
}

// --- negative numbers ---

void test_itoa_negative_base10(void) {
  char buf[32];
  itoa(-42, buf, 10);
  TEST_ASSERT_EQUAL_STRING("-42", buf);
}

void test_itoa_negative_one(void) {
  char buf[32];
  itoa(-1, buf, 10);
  TEST_ASSERT_EQUAL_STRING("-1", buf);
}

void test_itoa_negative_base16(void) {
  char buf[32];
  itoa(-10, buf, 16);
  TEST_ASSERT_EQUAL_STRING("-a", buf);
}

// --- base 2 (binary) ---

void test_itoa_base2(void) {
  char buf[32];
  itoa(10, buf, 2);
  TEST_ASSERT_EQUAL_STRING("1010", buf);
}

void test_itoa_base2_zero(void) {
  char buf[32];
  itoa(0, buf, 2);
  TEST_ASSERT_EQUAL_STRING("0", buf);
}

// --- base 8 (octal) ---

void test_itoa_base8(void) {
  char buf[32];
  itoa(64, buf, 8);
  TEST_ASSERT_EQUAL_STRING("100", buf);
}

// --- base 16 (hex) ---

void test_itoa_base16(void) {
  char buf[32];
  itoa(255, buf, 16);
  TEST_ASSERT_EQUAL_STRING("ff", buf);
}

void test_itoa_base16_large(void) {
  char buf[32];
  itoa(4096, buf, 16);
  TEST_ASSERT_EQUAL_STRING("1000", buf);
}

// --- return value ---

void test_itoa_returns_dest(void) {
  char buf[32];
  TEST_ASSERT_EQUAL_PTR(buf, itoa(42, buf, 10));
}

// --- max/min values ---

void test_itoa_max_int(void) {
  char buf[32];
  itoa(2147483647, buf, 10);
  TEST_ASSERT_EQUAL_STRING("2147483647", buf);
}

void test_itoa_min_int(void) {
  char buf[32];
  itoa(-2147483648, buf, 10);
  TEST_ASSERT_EQUAL_STRING("-2147483648", buf);
}

// --- base 36 ---

void test_itoa_base36(void) {
  char buf[32];
  itoa(35, buf, 36);
  TEST_ASSERT_EQUAL_STRING("z", buf);
}

void test_itoa_base36_max(void) {
  char buf[32];
  itoa(781, buf, 36);
  TEST_ASSERT_EQUAL_STRING("lp", buf);
}

// --- test suites ---

void test_itoa(void) {
  RUN_TEST(test_itoa_zero_base10);
  RUN_TEST(test_itoa_zero_base16);
  RUN_TEST(test_itoa_positive_base10);
  RUN_TEST(test_itoa_one);
  RUN_TEST(test_itoa_large_positive);
  RUN_TEST(test_itoa_single_digit);
  RUN_TEST(test_itoa_negative_base10);
  RUN_TEST(test_itoa_negative_one);
  RUN_TEST(test_itoa_negative_base16);
  RUN_TEST(test_itoa_base2);
  RUN_TEST(test_itoa_base2_zero);
  RUN_TEST(test_itoa_base8);
  RUN_TEST(test_itoa_base16);
  RUN_TEST(test_itoa_base16_large);
  RUN_TEST(test_itoa_returns_dest);
  RUN_TEST(test_itoa_max_int);
  RUN_TEST(test_itoa_min_int);
  RUN_TEST(test_itoa_base36);
  RUN_TEST(test_itoa_base36_max);
}

int main(void) {
  UNITY_BEGIN();
  test_itoa();
  return UNITY_END();
}
