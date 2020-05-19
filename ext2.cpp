#include "ext2.h"

ext2::ext2(int fSize_mb) : superBlock(fSize_mb * 1024 * 1024) {}

// ext2::ext2(std::string const& fName) {}
