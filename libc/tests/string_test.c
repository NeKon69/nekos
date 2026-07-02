#include "unity.h"
// Our own implementation
#include <stddef.h>
#include <string.h>

void setUp(void) {
  // Run before each test
}

void tearDown(void) {
  // Run after each test
}

void test_strlen_empty(void) { TEST_ASSERT_EQUAL_UINT(0, strlen("")); }

void test_strlen_hello(void) { TEST_ASSERT_EQUAL_UINT(5, strlen("hello")); }

void test_strlen_with_nulls(void) {
  char buf[] = {'h', 'i', '\0', 'x', 'y', 'z'};
  TEST_ASSERT_EQUAL_UINT(2, strlen(buf));
}

void test_strcpy_empty_src(void) {
  char buf[1] = {'A'};
  strcpy(buf, "");
  TEST_ASSERT_EQUAL_CHAR('\0', buf[0]);
}

void test_strcpy_normal(void) {
  char buf[16];
  char *ret = strcpy(buf, "hello");
  TEST_ASSERT_EQUAL_STRING("hello", buf);
  TEST_ASSERT_EQUAL_PTR(buf, ret);
}

void test_strcpy_exact_fit(void) {
  char buf[6];
  strcpy(buf, "hello");
  TEST_ASSERT_EQUAL_STRING("hello", buf);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[5]);
}

void test_strcpy_null_terminator_written(void) {
  char buf[8] = "xxxxxxxx";
  strcpy(buf, "hi");
  TEST_ASSERT_EQUAL_STRING("hi", buf);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[2]);
}

void test_strcpy_replaces_longer_dest(void) {
  char buf[20] = "long_initial_string";
  strcpy(buf, "ab");
  TEST_ASSERT_EQUAL_STRING("ab", buf);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[2]);
  TEST_ASSERT_EQUAL_CHAR('g', buf[3]);
}

void test_strcpy_single_char(void) {
  char buf[2];
  strcpy(buf, "q");
  TEST_ASSERT_EQUAL_STRING("q", buf);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[1]);
}

void test_strncpy_returns_dest(void) {
  char buf[16];
  TEST_ASSERT_EQUAL_PTR(buf, strncpy(buf, "hello", 16));
}

void test_strncpy_src_shorter_than_count(void) {
  char buf[8];
  strncpy(buf, "hi", 8);
  TEST_ASSERT_EQUAL_STRING("hi", buf);
  for (int i = 2; i < 8; i++)
    TEST_ASSERT_EQUAL_CHAR('\0', buf[i]);
}

void test_strncpy_src_longer_than_count(void) {
  char buf[4];
  strncpy(buf, "hello", 4);
  TEST_ASSERT_EQUAL_STRING_LEN("hell", buf, 4);
  TEST_ASSERT_NOT_EQUAL('\0', buf[3]);
}

void test_strncpy_count_zero(void) {
  char buf[5] = "abcd";
  strncpy(buf, "hello", 0);
  TEST_ASSERT_EQUAL_STRING("abcd", buf);
}

void test_strncpy_src_fits_in_count(void) {
  char buf[6];
  strncpy(buf, "hello", 6);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[5]);
}

void test_strncpy_empty_src_pads(void) {
  char buf[6];
  strncpy(buf, "", 6);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[0]);
  for (int i = 1; i < 6; i++)
    TEST_ASSERT_EQUAL_CHAR('\0', buf[i]);
}

void test_strcat_returns_dest(void) {
  char buf[32] = "foo";
  TEST_ASSERT_EQUAL_PTR(buf, strcat(buf, "bar"));
}

void test_strcat_empty_src(void) {
  char buf[8] = "hello";
  strcat(buf, "");
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_strcat_empty_dest(void) {
  char buf[8] = "";
  strcat(buf, "world");
  TEST_ASSERT_EQUAL_STRING("world", buf);
}

void test_strcat_both_empty(void) {
  char buf[2] = "";
  strcat(buf, "");
  TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_strcat_normal(void) {
  char buf[16] = "hello";
  strcat(buf, " world");
  TEST_ASSERT_EQUAL_STRING("hello world", buf);
}

void test_strcat_exact_fit(void) {
  char buf[11] = "hello";
  strcat(buf, "world");
  TEST_ASSERT_EQUAL_STRING("helloworld", buf);
  TEST_ASSERT_EQUAL_CHAR('\0', buf[10]);
}

void test_strcat_multiple_appends(void) {
  char buf[32] = "a";
  strcat(buf, "b");
  strcat(buf, "c");
  strcat(buf, "d");
  TEST_ASSERT_EQUAL_STRING("abcd", buf);
}

void test_strcmp_equal(void) { TEST_ASSERT_EQUAL_INT(0, strcmp("abc", "abc")); }

void test_strcmp_empty(void) { TEST_ASSERT_EQUAL_INT(0, strcmp("", "")); }

void test_strcmp_lhs_before_rhs(void) {
  int result = strcmp("abc", "abd");
  TEST_ASSERT_TRUE(result < 0);
}

void test_strcmp_lhs_after_rhs(void) {
  int result = strcmp("abd", "abc");
  TEST_ASSERT_TRUE(result > 0);
}

void test_strcmp_prefix_shorter_lhs(void) {
  int result = strcmp("abc", "abcd");
  TEST_ASSERT_TRUE(result < 0);
}

void test_strcmp_prefix_shorter_rhs(void) {
  int result = strcmp("abcd", "abc");
  TEST_ASSERT_TRUE(result > 0);
}

void test_strcmp_case_sensitive(void) {
  TEST_ASSERT_TRUE(strcmp("A", "a") < 0);
  TEST_ASSERT_TRUE(strcmp("a", "A") > 0);
}

void test_strcmp_different_first_char(void) {
  int result = strcmp("b", "a");
  TEST_ASSERT_EQUAL_INT(('b' - 'a'), result);
}

void test_strcmp_same_pointer(void) {
  const char *s = "hello";
  TEST_ASSERT_EQUAL_INT(0, strcmp(s, s));
}

void test_strncmp_equal(void) {
  TEST_ASSERT_EQUAL_INT(0, strncmp("abc", "abc", 3));
}

void test_strncmp_empty(void) { TEST_ASSERT_EQUAL_INT(0, strncmp("", "", 5)); }

void test_strncmp_count_zero(void) {
  TEST_ASSERT_EQUAL_INT(0, strncmp("abc", "xyz", 0));
}

void test_strncmp_lhs_before_rhs(void) {
  TEST_ASSERT_TRUE(strncmp("abc", "abd", 3) < 0);
}

void test_strncmp_lhs_after_rhs(void) {
  TEST_ASSERT_TRUE(strncmp("abd", "abc", 3) > 0);
}

void test_strncmp_count_larger_than_strings(void) {
  TEST_ASSERT_EQUAL_INT(0, strncmp("hi", "hi", 100));
}

void test_strncmp_prefix_equal_within_count(void) {
  TEST_ASSERT_EQUAL_INT(0, strncmp("abc", "abcd", 3));
}

void test_strncmp_prefix_differs_within_count(void) {
  TEST_ASSERT_TRUE(strncmp("abc", "abd", 3) < 0);
}

void test_strncmp_ignores_after_null(void) {
  char a[] = {'a', 'b', '\0', 'x', 'y'};
  char b[] = {'a', 'b', '\0', 'z', 'w'};
  TEST_ASSERT_EQUAL_INT(0, strncmp(a, b, 5));
}

void test_strncmp_case_sensitive(void) {
  TEST_ASSERT_TRUE(strncmp("A", "a", 1) < 0);
}

void test_strncmp_different_first_char(void) {
  int result = strncmp("b", "a", 1);
  TEST_ASSERT_EQUAL_INT(('b' - 'a'), result);
}

void test_strchr_found_at_beginning(void) {
  const char *s = "hello";
  char *result = strchr(s, 'h');
  TEST_ASSERT_EQUAL_PTR(s, result);
  TEST_ASSERT_EQUAL_CHAR('h', *result);
}

void test_strchr_found_in_middle(void) {
  const char *s = "hello";
  char *result = strchr(s, 'l');
  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_CHAR('l', *result);
  TEST_ASSERT_EQUAL_PTR(&s[2], result);
}

void test_strchr_found_at_end(void) {
  const char *s = "hello";
  char *result = strchr(s, 'o');
  TEST_ASSERT_EQUAL_PTR(&s[4], result);
}

void test_strchr_not_found(void) { TEST_ASSERT_NULL(strchr("hello", 'z')); }

void test_strchr_null_terminator(void) {
  const char *s = "hello";
  char *result = strchr(s, '\0');
  TEST_ASSERT_EQUAL_PTR(&s[5], result);
}

void test_strchr_empty_string(void) {
  const char *s = "";
  TEST_ASSERT_NULL(strchr(s, 'a'));
}

void test_strchr_empty_string_null(void) {
  const char *s = "";
  char *result = strchr(s, '\0');
  TEST_ASSERT_EQUAL_PTR(s, result);
}

void test_strchr_multiple_occurrences(void) {
  const char *s = "banana";
  char *result = strchr(s, 'a');
  TEST_ASSERT_EQUAL_PTR(&s[1], result);
}

void test_strrchr_found_at_beginning(void) {
  const char *s = "hello";
  char *result = strrchr(s, 'h');
  TEST_ASSERT_EQUAL_PTR(s, result);
  TEST_ASSERT_EQUAL_CHAR('h', *result);
}

void test_strrchr_found_in_middle(void) {
  const char *s = "hello";
  char *result = strrchr(s, 'l');
  TEST_ASSERT_NOT_NULL(result);
  TEST_ASSERT_EQUAL_CHAR('l', *result);
  TEST_ASSERT_EQUAL_PTR(&s[3], result);
}

void test_strrchr_found_at_end(void) {
  const char *s = "hello";
  char *result = strrchr(s, 'o');
  TEST_ASSERT_EQUAL_PTR(&s[4], result);
}

void test_strrchr_not_found(void) { TEST_ASSERT_NULL(strrchr("hello", 'z')); }

void test_strrchr_null_terminator(void) {
  const char *s = "hello";
  char *result = strrchr(s, '\0');
  TEST_ASSERT_EQUAL_PTR(&s[5], result);
}

void test_strrchr_empty_string(void) {
  const char *s = "";
  TEST_ASSERT_NULL(strrchr(s, 'a'));
}

void test_strrchr_empty_string_null(void) {
  const char *s = "";
  char *result = strrchr(s, '\0');
  TEST_ASSERT_EQUAL_PTR(s, result);
}

void test_strrchr_multiple_occurrences(void) {
  const char *s = "banana";
  char *result = strrchr(s, 'a');
  TEST_ASSERT_EQUAL_PTR(&s[5], result);
}

void test_strspn_all_match(void) {
  TEST_ASSERT_EQUAL_UINT(3, strspn("abc", "abcdef"));
}

void test_strspn_none_match(void) {
  TEST_ASSERT_EQUAL_UINT(0, strspn("xyz", "abc"));
}

void test_strspn_partial(void) {
  TEST_ASSERT_EQUAL_UINT(4, strspn("hellx", "hello"));
}

void test_strspn_empty_dest(void) {
  TEST_ASSERT_EQUAL_UINT(0, strspn("", "abc"));
}

void test_strspn_empty_src(void) {
  TEST_ASSERT_EQUAL_UINT(0, strspn("abc", ""));
}

void test_strspn_single_char(void) {
  TEST_ASSERT_EQUAL_UINT(3, strspn("aaa", "a"));
}

void test_strspn_mixed(void) {
  TEST_ASSERT_EQUAL_UINT(2, strspn("ab123", "abc"));
}

void test_strspn_all_same(void) {
  TEST_ASSERT_EQUAL_UINT(5, strspn("aaaaa", "abc"));
}

void test_strcspn_none_found(void) {
  TEST_ASSERT_EQUAL_UINT(5, strcspn("hello", "xyz"));
}

void test_strcspn_first_found(void) {
  TEST_ASSERT_EQUAL_UINT(0, strcspn("hello", "h"));
}

void test_strcspn_partial(void) {
  TEST_ASSERT_EQUAL_UINT(2, strcspn("ab123", "1"));
}

void test_strcspn_empty_dest(void) {
  TEST_ASSERT_EQUAL_UINT(0, strcspn("", "abc"));
}

void test_strcspn_empty_src(void) {
  TEST_ASSERT_EQUAL_UINT(5, strcspn("hello", ""));
}

void test_strcspn_both_empty(void) {
  TEST_ASSERT_EQUAL_UINT(0, strcspn("", ""));
}

void test_strcspn_all_found(void) {
  TEST_ASSERT_EQUAL_UINT(0, strcspn("abc", "abc"));
}

void test_strcspn_midpoint_found(void) {
  TEST_ASSERT_EQUAL_UINT(3, strcspn("abchello", "h"));
}

void test_strpbrk_first_match(void) {
  const char *s = "hello";
  char *result = strpbrk(s, "e");
  TEST_ASSERT_EQUAL_PTR(&s[1], result);
  TEST_ASSERT_EQUAL_CHAR('e', *result);
}

void test_strpbrk_middle_match(void) {
  const char *s = "hello";
  char *result = strpbrk(s, "l");
  TEST_ASSERT_EQUAL_PTR(&s[2], result);
}

void test_strpbrk_last_match(void) {
  const char *s = "hello";
  char *result = strpbrk(s, "o");
  TEST_ASSERT_EQUAL_PTR(&s[4], result);
}

void test_strpbrk_no_match(void) { TEST_ASSERT_NULL(strpbrk("hello", "xyz")); }

void test_strpbrk_empty_dest(void) { TEST_ASSERT_NULL(strpbrk("", "abc")); }

void test_strpbrk_empty_breakset(void) {
  TEST_ASSERT_NULL(strpbrk("hello", ""));
}

void test_strpbrk_both_empty(void) { TEST_ASSERT_NULL(strpbrk("", "")); }

void test_strpbrk_multiple_matches(void) {
  const char *s = "ddbbaa";
  char *result = strpbrk(s, "abc");
  TEST_ASSERT_EQUAL_PTR(&s[2], result);
}

void test_strstr_found_at_beginning(void) {
  const char *s = "hello world";
  char *result = strstr(s, "hello");
  TEST_ASSERT_EQUAL_PTR(s, result);
}

void test_strstr_found_in_middle(void) {
  const char *s = "hello world";
  char *result = strstr(s, "wo");
  TEST_ASSERT_EQUAL_PTR(&s[6], result);
}

void test_strstr_found_at_end(void) {
  const char *s = "hello world";
  char *result = strstr(s, "orld");
  TEST_ASSERT_EQUAL_PTR(&s[7], result);
}

void test_strstr_not_found(void) { TEST_ASSERT_NULL(strstr("hello", "xyz")); }

void test_strstr_empty_substr(void) {
  const char *s = "hello";
  TEST_ASSERT_EQUAL_PTR(s, strstr(s, ""));
}

void test_strstr_empty_str(void) { TEST_ASSERT_NULL(strstr("", "abc")); }

void test_strstr_both_empty(void) { TEST_ASSERT_EQUAL_PTR("", strstr("", "")); }

void test_strstr_substr_longer(void) {
  TEST_ASSERT_NULL(strstr("hi", "hello"));
}

void test_strstr_multiple_occurrences(void) {
  const char *s = "abcabc";
  char *result = strstr(s, "abc");
  TEST_ASSERT_EQUAL_PTR(&s[0], result);
}

void test_strncat_returns_dest(void) {
  char buf[32] = "foo";
  TEST_ASSERT_EQUAL_PTR(buf, strncat(buf, "bar", 3));
}

void test_strncat_normal(void) {
  char buf[16] = "hello";
  strncat(buf, " world", 6);
  TEST_ASSERT_EQUAL_STRING("hello world", buf);
}

void test_strncat_src_shorter_than_count(void) {
  char buf[16] = "hello";
  strncat(buf, "hi", 10);
  TEST_ASSERT_EQUAL_STRING("hellohi", buf);
}

void test_strncat_src_longer_than_count(void) {
  char buf[16] = "hello";
  strncat(buf, "world", 3);
  TEST_ASSERT_EQUAL_STRING("hellowor", buf);
}

void test_strncat_count_zero(void) {
  char buf[16] = "hello";
  strncat(buf, "world", 0);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_strncat_empty_src(void) {
  char buf[16] = "hello";
  strncat(buf, "", 5);
  TEST_ASSERT_EQUAL_STRING("hello", buf);
}

void test_strncat_empty_dest(void) {
  char buf[16] = "";
  strncat(buf, "world", 5);
  TEST_ASSERT_EQUAL_STRING("world", buf);
}

void test_strncat_both_empty(void) {
  char buf[16] = "";
  strncat(buf, "", 5);
  TEST_ASSERT_EQUAL_STRING("", buf);
}

void test_strncat_exact_fit(void) {
  char buf[11] = "hello";
  strncat(buf, "world", 5);
  TEST_ASSERT_EQUAL_STRING("helloworld", buf);
}

void test_strncat_multiple_appends(void) {
  char buf[32] = "a";
  strncat(buf, "b", 1);
  strncat(buf, "c", 1);
  strncat(buf, "d", 1);
  TEST_ASSERT_EQUAL_STRING("abcd", buf);
}

void test_strncat_n_equals_strlen(void) {
  char buf[11] = {'h', 'e', 'l', 'l', 'o', 0, 1, 1, 1, 1, 1};
  strncat(buf, "world", 5);
  TEST_ASSERT_EQUAL_UINT(10, strlen(buf));
}

void test_strlen(void) {
  RUN_TEST(test_strlen_empty);
  RUN_TEST(test_strlen_hello);
  RUN_TEST(test_strlen_with_nulls);
}

void test_strcpy(void) {
  RUN_TEST(test_strcpy_empty_src);
  RUN_TEST(test_strcpy_normal);
  RUN_TEST(test_strcpy_exact_fit);
  RUN_TEST(test_strcpy_null_terminator_written);
  RUN_TEST(test_strcpy_replaces_longer_dest);
  RUN_TEST(test_strcpy_single_char);
}

void test_strncpy(void) {
  RUN_TEST(test_strncpy_returns_dest);
  RUN_TEST(test_strncpy_src_shorter_than_count);
  RUN_TEST(test_strncpy_src_longer_than_count);
  RUN_TEST(test_strncpy_count_zero);
  RUN_TEST(test_strncpy_src_fits_in_count);
  RUN_TEST(test_strncpy_empty_src_pads);
}

void test_strcat(void) {
  RUN_TEST(test_strcat_returns_dest);
  RUN_TEST(test_strcat_empty_src);
  RUN_TEST(test_strcat_empty_dest);
  RUN_TEST(test_strcat_both_empty);
  RUN_TEST(test_strcat_normal);
  RUN_TEST(test_strcat_exact_fit);
  RUN_TEST(test_strcat_multiple_appends);
}

void test_strcmp(void) {
  RUN_TEST(test_strcmp_equal);
  RUN_TEST(test_strcmp_empty);
  RUN_TEST(test_strcmp_lhs_before_rhs);
  RUN_TEST(test_strcmp_lhs_after_rhs);
  RUN_TEST(test_strcmp_prefix_shorter_lhs);
  RUN_TEST(test_strcmp_prefix_shorter_rhs);
  RUN_TEST(test_strcmp_case_sensitive);
  RUN_TEST(test_strcmp_different_first_char);
  RUN_TEST(test_strcmp_same_pointer);
}

void test_strncmp(void) {
  RUN_TEST(test_strncmp_equal);
  RUN_TEST(test_strncmp_empty);
  RUN_TEST(test_strncmp_count_zero);
  RUN_TEST(test_strncmp_lhs_before_rhs);
  RUN_TEST(test_strncmp_lhs_after_rhs);
  RUN_TEST(test_strncmp_count_larger_than_strings);
  RUN_TEST(test_strncmp_prefix_equal_within_count);
  RUN_TEST(test_strncmp_prefix_differs_within_count);
  RUN_TEST(test_strncmp_ignores_after_null);
  RUN_TEST(test_strncmp_case_sensitive);
  RUN_TEST(test_strncmp_different_first_char);
}

void test_strchr(void) {
  RUN_TEST(test_strchr_found_at_beginning);
  RUN_TEST(test_strchr_found_in_middle);
  RUN_TEST(test_strchr_found_at_end);
  RUN_TEST(test_strchr_not_found);
  RUN_TEST(test_strchr_null_terminator);
  RUN_TEST(test_strchr_empty_string);
  RUN_TEST(test_strchr_empty_string_null);
  RUN_TEST(test_strchr_multiple_occurrences);
}

void test_strrchr(void) {
  RUN_TEST(test_strrchr_found_at_beginning);
  RUN_TEST(test_strrchr_found_in_middle);
  RUN_TEST(test_strrchr_found_at_end);
  RUN_TEST(test_strrchr_not_found);
  RUN_TEST(test_strrchr_null_terminator);
  RUN_TEST(test_strrchr_empty_string);
  RUN_TEST(test_strrchr_empty_string_null);
  RUN_TEST(test_strrchr_multiple_occurrences);
}

void test_strspn(void) {
  RUN_TEST(test_strspn_all_match);
  RUN_TEST(test_strspn_none_match);
  RUN_TEST(test_strspn_partial);
  RUN_TEST(test_strspn_empty_dest);
  RUN_TEST(test_strspn_empty_src);
  RUN_TEST(test_strspn_single_char);
  RUN_TEST(test_strspn_mixed);
  RUN_TEST(test_strspn_all_same);
}

void test_strcspn(void) {
  RUN_TEST(test_strcspn_none_found);
  RUN_TEST(test_strcspn_first_found);
  RUN_TEST(test_strcspn_partial);
  RUN_TEST(test_strcspn_empty_dest);
  RUN_TEST(test_strcspn_empty_src);
  RUN_TEST(test_strcspn_both_empty);
  RUN_TEST(test_strcspn_all_found);
  RUN_TEST(test_strcspn_midpoint_found);
}

void test_strpbrk(void) {
  RUN_TEST(test_strpbrk_first_match);
  RUN_TEST(test_strpbrk_middle_match);
  RUN_TEST(test_strpbrk_last_match);
  RUN_TEST(test_strpbrk_no_match);
  RUN_TEST(test_strpbrk_empty_dest);
  RUN_TEST(test_strpbrk_empty_breakset);
  RUN_TEST(test_strpbrk_both_empty);
  RUN_TEST(test_strpbrk_multiple_matches);
}

void test_strstr(void) {
  RUN_TEST(test_strstr_found_at_beginning);
  RUN_TEST(test_strstr_found_in_middle);
  RUN_TEST(test_strstr_found_at_end);
  RUN_TEST(test_strstr_not_found);
  RUN_TEST(test_strstr_empty_substr);
  RUN_TEST(test_strstr_empty_str);
  RUN_TEST(test_strstr_both_empty);
  RUN_TEST(test_strstr_substr_longer);
  RUN_TEST(test_strstr_multiple_occurrences);
}

void test_strncat(void) {
  RUN_TEST(test_strncat_returns_dest);
  RUN_TEST(test_strncat_normal);
  RUN_TEST(test_strncat_src_shorter_than_count);
  RUN_TEST(test_strncat_src_longer_than_count);
  RUN_TEST(test_strncat_count_zero);
  RUN_TEST(test_strncat_empty_src);
  RUN_TEST(test_strncat_empty_dest);
  RUN_TEST(test_strncat_both_empty);
  RUN_TEST(test_strncat_exact_fit);
  RUN_TEST(test_strncat_multiple_appends);
  RUN_TEST(test_strncat_n_equals_strlen);
}

int main(void) {
  UNITY_BEGIN();
  test_strlen();
  test_strcpy();
  test_strncpy();
  test_strcat();
  test_strncat();
  test_strcmp();
  test_strncmp();
  test_strchr();
  test_strrchr();
  test_strspn();
  test_strcspn();
  test_strpbrk();
  test_strstr();
  return UNITY_END();
}
