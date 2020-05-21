#include "sub_menu.h"

#include "utils.h"

void Menu_1(FAT32 *fat) {
  size_t size_Mb;
  string VolName;
  cout << " enter size of Volume: ";
  cin >> size_Mb;
  cout << " enter Volume Name: ";
  Flush(cin);
  getline(cin, VolName);
  fat = new FAT32(size_Mb, VolName);
  cout << " Create Volume success" << endl;
  Menu_sub(fat);
}

void Menu_2(FAT32 *fat) {
  string VolName;
  cout << " enter Volume Name: ";
  Flush(cin);
  getline(cin, VolName);
  fat = new FAT32(VolName);
  Menu_sub(fat);
}

void MenuImport(FAT32 *fat) {
  std::string fPath;
  std::cout << "File to import: ";
  Flush(std::cin);
  std::getline(std::cin, fPath);

}

void Menu_sub(FAT32 *fat) {
  string cmd;
 
  cout << "/";
  getline(cin, cmd);
  if (cmd == "import") {
  }
  if (cmd == "ls") {
    Menu_showFile(fat);
  }
  if (cmd == "rm") {
  }
  if (cmd == "pass") {
  }
  if (cmd == "export") {
  }
  if (cmd == "cd") {
  }
  if (cmd == "exit") {
  }
}
void Menu_showFile(FAT32 *f) {
  /*string imgPath;
  cout<<"enter name Volume: ";
  getline(cin,imgPath);
  FAT32 f=FAT32(imgPath)*/
  std::vector<MainEntry *> root = f->ReadRoot();
  f->printRoot(root);
}
void Menu_Password() {
  string fileName;
  cout << " enter name of file need password\n";
  getline(cin, fileName);
}

void Menu_3() {}
void Menu_4() {}
void Menu_5() {}
void Menu_6() {}