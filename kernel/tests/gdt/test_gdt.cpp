#include "gdt/gdt_helpers.hpp"
#include "test.hpp"

uint64_t buildEntry(uint32_t Limit, uint32_t Base, uint8_t AccessByte,
                    uint8_t Flags) {
  return tests::gdt::GDTAccess::buildEntry(Limit, Base, AccessByte, Flags);
}

void test_gdt_loads() {
  GDT Gdt;

  const tests::gdt::GDTR Gdtr = tests::gdt::readGDTR();
  NEKOS_EXPECT_TRUE(Gdtr.Base != 0);
}

void test_gdt_loads_expected_descriptor() {
  GDT Gdt;

  NEKOS_EXPECT_GDTR_LIMIT(3 * sizeof(uint64_t) - 1);
  NEKOS_EXPECT_GDT_ENTRY(0, tests::gdt::NullEntry);
  NEKOS_EXPECT_GDT_ENTRY(1, tests::gdt::KernelCodeEntry);
  NEKOS_EXPECT_GDT_ENTRY_ACCESSED(2);
}

void test_gdt_build_entry_encodes_null_descriptor() {
  NEKOS_EXPECT_TRUE(buildEntry(0, 0, 0, 0) == tests::gdt::NullEntry);
}

void test_gdt_build_entry_encodes_kernel_code_descriptor() {
  NEKOS_EXPECT_TRUE(
      buildEntry(tests::gdt::FlatSegmentLimit, tests::gdt::FlatSegmentBase,
                 tests::gdt::KernelCodeAccess,
                 tests::gdt::FlatSegmentFlags) == tests::gdt::KernelCodeEntry);
}

void test_gdt_build_entry_encodes_kernel_data_descriptor() {
  NEKOS_EXPECT_TRUE(
      buildEntry(tests::gdt::FlatSegmentLimit, tests::gdt::FlatSegmentBase,
                 tests::gdt::KernelDataAccess,
                 tests::gdt::FlatSegmentFlags) == tests::gdt::KernelDataEntry);
}

void test_gdt_build_entry_encodes_base_and_limit_bits() {
  NEKOS_EXPECT_TRUE(
      buildEntry(0xABCDE, 0x12345678, tests::gdt::KernelCodeAccess,
                 tests::gdt::FlatSegmentFlags) == 0x12CA98345678BCDE);
}

namespace tests {

void runGDTTests() {
  test_gdt_loads();
  test_gdt_loads_expected_descriptor();
  test_gdt_build_entry_encodes_null_descriptor();
  test_gdt_build_entry_encodes_kernel_code_descriptor();
  test_gdt_build_entry_encodes_kernel_data_descriptor();
  test_gdt_build_entry_encodes_base_and_limit_bits();
}

} // namespace tests
