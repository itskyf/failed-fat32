#include <cmath>

#include "ext2.h"

uint32_t div_roundup(uint32_t size, uint32_t n) {
  // Overflow-resistant
  uint32_t res = size / n;
  if (res * n != size) ++res;
  return res;
}

ext2_super_block::ext2_super_block(int fSize_b) {
  uint32_t inodeSize = 128;
  uint32_t blockSize = 1024;
  uint32_t inodeRatio = 16384;
  // TODO
  // if (fSize_b < 512 * 1024 * 1024) inodeRatio = 4096;  // 512 mb
  // if (fSize_b < 3 * 1024 * 1024) inodeRatio = 8192;    // 3 mb
  uint32_t nBlocks = fSize_b / blockSize;
  uint32_t firstBlock = (EXT2_MIN_BLOCK_SIZE == blockSize);
  uint32_t blocksPerGroup = 8 * blockSize;
  uint32_t nGroups = div_roundup(nBlocks - firstBlock, blocksPerGroup);
  uint32_t nInodes = fSize_b / inodeRatio;
  uint32_t inodesPerGroup = div_roundup(nInodes, nGroups);
  // minimum number because the first EXT2_GOOD_OLD_FIRST_INO-1 are reserved
  if (inodesPerGroup < 16) inodesPerGroup = 16;
  // a block group can't have more inodes than blocks
  if (inodesPerGroup > blocksPerGroup) inodesPerGroup = blocksPerGroup;
  // adjust inodes per group so they completely fill the inode table blocks in
  // the descriptor
  inodesPerGroup =
      (div_roundup(inodesPerGroup * inodeSize, blockSize) * blockSize) /
      inodeSize;
  // make sure the number of inodes per group is a multiple of 8
  inodesPerGroup &= ~7;

  s_inodes_count = inodesPerGroup * nGroups;
  s_blocks_count = nBlocks;
  // TODO s_free_blocks_count
  s_free_inodes_count =
      inodesPerGroup * nGroups - EXT2_GOOD_OLD_FIRST_INO;  // TOFIND
  s_first_data_block = firstBlock;
  s_log_block_size = log2(blockSize);
  s_blocks_per_group = blocksPerGroup;
  s_inodes_per_group = inodesPerGroup;
}
