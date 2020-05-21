#pragma once
#include "fat_entry.h"
#include "dir_entry.h"
#include "FAT32.h"
#include<iostream>
#include<string>
using namespace std;

void Menu_1(FAT32 *fat);
void Menu_2(FAT32 *fat);
void Menu_sub(FAT32 *fat);
void MenuImport(FAT32 *fat);
void Menu_showFile(FAT32*fat);
void Menu_Password();



