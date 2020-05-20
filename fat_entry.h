#pragma once
#include <cstdint>

struct FatEntry {
  enum FAT_TABLE : uint32_t {
    bad = 0x0FFFFFF7,
    eof = 0x0FFFFFFF,
    used_beg = 0x2,
    used_end = 0x0FFFFFFF,
    Empty = 0x0,
  } value;
};
