#include "ext2.h"

#include <iostream>

using std::ios;

void Allocate(uint8_t* bitmap, uint32_t blocksize, uint32_t start,
              uint32_t end) {
  memset(bitmap, 0, blocksize);
  uint32_t i = start / 8;
  memset(bitmap, 0xFF, i);
  bitmap[i] = (1 << (start & 7)) - 1;  // 0..7 => 00000000..01111111
  i = end / 8;
  bitmap[blocksize - i - 1] |=
      0x7F00 >> (end & 7);                  // 0..7 => 00000000..11111110
  memset(bitmap + blocksize - i, 0xFF, i);  // N.B. no overflow here!
}

bool BlockHasSuper(uint32_t x) {
  // 0, 1 and powers of 3, 5, 7 up to 2^32 limit
  static const uint32_t supers[] = {
      0,          1,
      3,          5,
      7,          9,
      25,         27,
      49,         81,
      125,        243,
      343,        625,
      729,        2187,
      2401,       3125,
      6561,       15625,
      16807,      19683,
      59049,      78125,
      117649,     177147,
      390625,     531441,
      823543,     1594323,
      1953125,    4782969,
      5764801,    9765625,
      14348907,   40353607,
      43046721,   48828125,
      129140163,  244140625,
      282475249,  387420489,
      1162261467, 1220703125,
      1977326743, 3486784401 /* >2^31 */,
  };
  const uint32_t* sp = supers + sizeof(supers) / sizeof(uint32_t);
  while (true) {
    --sp;
    if (x == *sp) return true;
    if (x > *sp) return false;
  }
}

uint32_t DivRoundup(uint32_t size, uint32_t n) {
  // Overflow-resistant
  uint32_t res = size / n;
  if (res * n != size) ++res;
  return res;
}

ext2::~ext2() { file.close(); }

ext2::ext2(size_t fSize_mb, std::string const& fileName)
    : file(fileName, ios::out | ios::binary) {
  size_t fSize_b = fSize_mb << 20;
  file.seekp(fSize_b - 1);
  file.write("", 1);
  file.seekp(file.beg);

  // Default
  uint32_t inodeSize = sizeof(struct ext2_inode);  // 76
  uint32_t blockSize = 1024;
  uint8_t* buf = new uint8_t[blockSize];
  uint32_t inodeRatio = 16384;

  uint32_t nBlocks = fSize_b / blockSize;
  uint32_t firstBlock = (EXT2_MIN_BLOCK_SIZE == blockSize);  // 1
  uint32_t blocksPerGroup = 8 * blockSize;
  uint32_t nGroups = DivRoundup(nBlocks - firstBlock, blocksPerGroup);
  uint32_t nInodes = fSize_b / inodeRatio;
  uint32_t inodesPerGroup = DivRoundup(nInodes, nGroups);
  // minimum number because the first EXT2_GOOD_OLD_FIRST_INO-1 are reserved
  if (inodesPerGroup < 16) inodesPerGroup = 16;
  // a block group can't have more inodes than blocks
  if (inodesPerGroup > blocksPerGroup) inodesPerGroup = blocksPerGroup;
  // adjust inodes per group so they completely fill the inode table blocks in
  // the descriptor
  inodesPerGroup =
      DivRoundup(inodesPerGroup * inodeSize, blockSize) * blockSize / inodeSize;
  // make sure the number of inodes per group is a multiple of 8
  inodesPerGroup &= ~7;
  uint32_t groupDescBlocks =
      DivRoundup(nGroups, blockSize / sizeof(gd));  // TOFIND
  uint32_t inodeTableBlocks = DivRoundup(inodesPerGroup * inodeSize, blockSize);

  // TODO maybe in-function
  sb.reset(new ext2_super_block);
  gd.reset(new ext2_group_desc[nGroups]);

  sb->s_inodes_count = inodesPerGroup * nGroups;
  sb->s_blocks_count = nBlocks;
  // TODO
  sb->s_free_blocks_count = 0;
  // Calculate each groupd descriptor
  for (uint32_t i = 0, pos = firstBlock, n = nBlocks - firstBlock; i < nGroups;
       ++i, pos += blocksPerGroup, n -= blocksPerGroup) {
    uint32_t overhead = pos + (BlockHasSuper(i) ? (groupDescBlocks + 1) : 0);
    uint32_t freeBlocks;
    // fill group descriptors
    gd[i].bg_block_bitmap = overhead;
    gd[i].bg_inode_bitmap = overhead + 1;
    gd[i].bg_inode_table = overhead + 2;
    overhead = overhead - pos + 2 + inodeTableBlocks;  // 2 bitmap
    /* "/" in 1st block, occupies 1 block, thus increased overhead for the first
     * block group and 1 used directories */
    gd[i].bg_free_inodes_count = inodesPerGroup;
    if (!i) {
      ++overhead;
      gd[i].bg_used_dirs_count = 1;
    }
    // cache free block count of the group
    freeBlocks = (n < blocksPerGroup ? n : blocksPerGroup) - overhead;
    // mark preallocated blocks as allocated
    Allocate(buf, blockSize,
             // reserve "overhead" blocks
             overhead,
             // mark unused trailing blocks
             blocksPerGroup - (freeBlocks + overhead));
    // write block bitmap
    gd[i].bg_free_blocks_count = freeBlocks;
    // mark preallocated inodes as allocated
    Allocate(buf, blockSize,
             // mark reserved inodes
             inodesPerGroup - gd[i].bg_free_inodes_count,
             // mark unused trailing inodes
             blocksPerGroup - inodesPerGroup);
    // count overall free blocks
    sb->s_free_blocks_count += freeBlocks;
  }

  sb->s_free_inodes_count =
      inodesPerGroup * nGroups - EXT2_GOOD_OLD_FIRST_INO;  // TOFIND
  sb->s_first_data_block = firstBlock;
  sb->s_log_block_size = log2(blockSize);
  sb->s_blocks_per_group = blocksPerGroup;
  sb->s_inodes_per_group = inodesPerGroup;
  sb->s_magic = EXT2_SUPER_MAGIC;

  delete[] buf;
}

// ext2::ext2(std::string const& fName) {}
