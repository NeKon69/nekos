#pragma once
#include "kprintf.hpp"
#include <stdint.h>

struct Tag {
  uint32_t Type;
  uint32_t Size;
};

struct Multiboot2Info {
  uint32_t Flags;
  uint32_t MemLower;
  uint32_t MemUpper;
  uint32_t BootDevice;
  uint32_t CommandLine;
  uint32_t ModsCount;
  uint32_t ModsAddr;
  // Bit 2 (0x8) of Flags: ELF sections
  uint32_t ElfSections[4];
  // Bit 3 (0x10) of Flags: memory map
  uint32_t MmapLen;
  uint32_t MmapAddr;
  // Bit 4 (0x20) of Flags: drive info
  uint32_t DrivesLen;
  uint32_t DrivesAddr;
  // Bit 5 (0x40) of Flags: config table
  uint32_t ConfigTable;
  // Bit 6 (0x80) of Flags: boot loader name
  uint32_t BootLoaderName;
  // Bit 7 (0x100) of Flags: APM table
  uint32_t ApmTable;
  // Bit 8 (0x200) of Flags: VBE
  uint32_t VbeControlInfo;
  uint32_t VbeModeInfo;
  uint32_t VbeModeSegment;
  uint32_t VbeControlAccess;
  uint32_t VbeModeAccess;
  uint32_t VbeModeTotal;
  // Bit 9 (0x400) of Flags: framebuffer
  uint64_t FramebufferAddr;
  uint32_t FramebufferPitch;
  uint32_t FramebufferWidth;
  uint32_t FramebufferHeight;
  uint8_t FramebufferBpp;
  uint8_t FramebufferType;
  uint16_t Reserved;
  uint8_t FramebufferRedMask;
  uint8_t FramebufferGreenMask;
  uint8_t FramebufferBlueMask;
  uint32_t TagList;
  void dump() {
    kprintf("Flags: %x\n", Flags);
    kprintf("MemLower: %x\n", MemLower);
    kprintf("MemUpper: %x\n", MemUpper);
    kprintf("BootDevice: %x\n", BootDevice);
    kprintf("CommandLine: %x\n", CommandLine);
    kprintf("ModsCount: %x\n", ModsCount);
    kprintf("ModsAddr: %x\n", ModsAddr);
    kprintf("ElfSections: %x\n", ElfSections);
    kprintf("MmapLen: %x\n", MmapLen);
    kprintf("MmapAddr: %x\n", MmapAddr);
    kprintf("DrivesLen: %x\n", DrivesLen);
    kprintf("DrivesAddr: %x\n", DrivesAddr);
    kprintf("ConfigTable: %x\n", ConfigTable);
    kprintf("BootLoaderName: %x\n", BootLoaderName);
    kprintf("ApmTable: %x\n", ApmTable);
    kprintf("VbeControlInfo: %x\n", VbeControlInfo);
    kprintf("VbeModeInfo: %x\n", VbeModeInfo);
    kprintf("VbeModeSegment: %x\n", VbeModeSegment);
    kprintf("VbeControlAccess: %x\n", VbeControlAccess);
    kprintf("VbeModeAccess: %x\n", VbeModeAccess);
    kprintf("VbeModeTotal: %x\n", VbeModeTotal);
    kprintf("FramebufferAddr: %x\n", FramebufferAddr);
    kprintf("FramebufferPitch: %x\n", FramebufferPitch);
    kprintf("FramebufferWidth: %x\n", FramebufferWidth);
    kprintf("FramebufferHeight: %x\n", FramebufferHeight);
    kprintf("FramebufferBpp: %x\n", FramebufferBpp);
    kprintf("FramebufferType: %x\n", FramebufferType);
    kprintf("Reserved: %x\n", Reserved);
    kprintf("FramebufferRedMask: %x\n", FramebufferRedMask);
    kprintf("FramebufferGreenMask: %x\n", FramebufferGreenMask);
    kprintf("FramebufferBlueMask: %x\n", FramebufferBlueMask);
    kprintf("TagList: %x\n", TagList);
  }
} __attribute__((packed));
