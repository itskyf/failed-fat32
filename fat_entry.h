#pragma once
#include <cstdint>

enum FatTable : uint32_t {
  bad = 0x0FFFFFF7,
  eof = 0x0FFFFFFF,
  used_beg = 0x2,
  used_end = 0x0FFFFFFF,
  fempty = 0x0,
};

struct FatEntry {
  uint32_t state;
};
