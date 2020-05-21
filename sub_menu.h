#pragma once
#include <conio.h>

#include <iostream>
#include <string>

#include "FAT32.h"
#include "dir_entry.h"
#include "fat_entry.h"

void Menu_1(FAT32 *fat);
void Menu_2(FAT32 *fat);
void Menu_sub(FAT32 *fat);
void MenuImport(FAT32 *fat);
void Menu_showFile(FAT32 *fat);
void Menu_Password();
std::string read_pass(std::istream &fi);
void change_password(std::string pass);
uint8_t HashPassword(std::string const &Combine);
