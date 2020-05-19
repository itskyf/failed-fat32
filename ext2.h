#pragma once
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#define EXT2_MIN_BLOCK_LOG_SIZE 10 /* 1024 */
#define EXT2_MIN_BLOCK_SIZE (1 << EXT2_MIN_BLOCK_LOG_SIZE)
#define EXT2_GOOD_OLD_FIRST_INO \
  11  // First non-reserved inode for old ext2 filesystems
#define EXT2_SUPER_MAGIC 0xEF53

struct ext2_group_desc {
  uint32_t bg_block_bitmap;      /* Blocks bitmap block */
  uint32_t bg_inode_bitmap;      /* Inodes bitmap block */
  uint32_t bg_inode_table;       /* Inodes table block */
  uint16_t bg_free_blocks_count; /* Free blocks count */
  uint16_t bg_free_inodes_count; /* Free inodes count */
  uint16_t bg_used_dirs_count;   /* Directories count */
  uint16_t bg_pad;
  uint32_t bg_reserved[3];
};

#define EXT2_NDIR_BLOCKS 12
#define EXT2_IND_BLOCK EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS (EXT2_TIND_BLOCK + 1)

struct ext2_inode {
  uint16_t i_mode;   /* File mode */
  uint32_t i_size;   /* Size in bytes */
  uint32_t i_blocks; /* Blocks count */
  uint32_t i_flags;  /* File flags */
  uint32_t i_block[EXT2_N_BLOCKS]; /* Pointers to blocks */
};

// TODO tuy chon
struct ext2_super_block {
  uint32_t s_inodes_count;      /* Inodes count */
  uint32_t s_blocks_count;      /* Blocks count */
  uint32_t s_free_blocks_count; /* Free blocks count */
  uint32_t s_free_inodes_count; /* Free inodes count */
  uint32_t s_first_data_block;  /* First Data Block */
  uint32_t s_log_block_size;    /* Block size */
  uint32_t s_blocks_per_group;  /* # Blocks per group */
  uint32_t s_inodes_per_group;  /* # Inodes per group */
  uint16_t s_magic;
  // uint16_t s_errors;           TODO /* Behaviour when detecting errors */
  void write(std::ofstream &fo);
};

struct ext2_dir_entry {
  uint32_t inode;   /* Inode number */
  uint16_t rec_len; /* Directory entry length */
  uint8_t name_len; /* Name length */
  uint8_t file_type;
  // TODO password
  char name[]; /* File name, up to EXT2_NAME_LEN */
};

class ext2 {
 public:
  ext2(size_t fSize_mb, std::string const& fileName);
  ext2(std::string const& fileName);
  virtual ~ext2();

 private:
  void WriteToDisk();

  std::unique_ptr<ext2_super_block> sb;
  std::unique_ptr<ext2_group_desc[]> gd;
  std::fstream file;
  std::string curDir;
};