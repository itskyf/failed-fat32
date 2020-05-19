#include "ext2.h"

void ext2_super_block::write(std::ofstream &fo) {
  std::streampos start = fo.tellp();
  fo.write(reinterpret_cast<char *>(s_inodes_count), sizeof(s_inodes_count));
  fo.write(reinterpret_cast<char *>(s_blocks_count), sizeof(s_blocks_count));
  fo.write(reinterpret_cast<char *>(s_free_blocks_count),
           sizeof(s_free_blocks_count));
  fo.write(reinterpret_cast<char *>(s_free_inodes_count),
           sizeof(s_free_inodes_count));
  fo.write(reinterpret_cast<char *>(s_first_data_block),
           sizeof(s_first_data_block));
  fo.write(reinterpret_cast<char *>(s_log_block_size),
           sizeof(s_log_block_size));
  fo.write(reinterpret_cast<char *>(s_blocks_per_group),
           sizeof(s_blocks_per_group));
  fo.write(reinterpret_cast<char *>(s_inodes_per_group),
           sizeof(s_inodes_per_group));
  fo.write(reinterpret_cast<char *>(s_magic), sizeof(s_magic));
  std::streampos allocated = fo.tellp() - start;
  size_t seekSize = 1023 - allocated;
  fo.seekp(seekSize, fo.cur);
  fo.write("", 1);
}