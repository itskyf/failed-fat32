#include "sub_menu.h"

#include <filesystem>

#include "utils.h"
using namespace std;

void Menu_1(FAT32 *fat) {
  size_t size_Mb;
  string VolName;
  cout << "enter size of Volume: ";
  cin >> size_Mb;
  Flush(std::cin);
  cout << "enter Volume Name: ";
  getline(cin, VolName);
  fat = new FAT32(size_Mb, VolName);
  cout << "Create Volume success" << endl;
  Menu_sub(fat);
}

void Menu_2(FAT32 *fat) {
  string VolName;
  cout << "enter Volume Name: ";
  getline(cin, VolName);
  fat = new FAT32(VolName);
  Menu_sub(fat);
}

void MenuImport(FAT32 *fat) {
  std::string fPath;
  std::cout << "File to import: ";
  std::getline(std::cin, fPath);
  fat->RootWrite(std::filesystem::path(fPath));
}

void Menu_sub(FAT32 *fat) {
  string cmd;
  bool run = true;
  while (run) {
    cout << "/";
    getline(cin, cmd);
    if (cmd == "import") {
      MenuImport(fat);
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
      run = false;
    }
  }
}
void Menu_showFile(FAT32 *f) {
  /*string imgPath;
  cout<<"enter name Volume: ";
  getline(cin,imgPath);
  FAT32 f=FAT32(imgPath)*/
  std::vector<int> root = f->ReadRoot();
  f->printRoot(root);
}
void Menu_Password() {
  string fileName;
  string password;
  bool change = false;
  cout << "enter name of file need password\n";
  getline(cin, fileName);
  // TODO

  // tìm đc file
  // check đã từng có mật khẩu?
  // nếu có
  cout << "enter your password: ";
  password = read_pass(cin);
  cout << "do you want to change Password? (1.yes/0.no)";
  cin >> change;
  if (change) {
    change_password(password);
    uint8_t hash_pass = HashPassword(password);
  }
}

string read_pass(istream &fi) {
  string pass("");
  char ch;
  while ((ch = _getch()) != 13)  // 13 is enter, input enter = done
  {
    if (ch != 8) {
      pass.push_back(ch);
      cout << '*';
    } else if (pass.length() > 0) {
      pass.pop_back();
      cout << "\b \b";
    }
  }
  cout << endl;
  return pass;
}
void change_password(string password) {
  string pass("");
  cout << "Enter new password: ";
  pass = read_pass(cin);
  password = pass;
  cout << "Your password has been reset successfully!\n";
  system("pause");
}

uint8_t HashPassword(std::string const &Combine) {
  uint8_t hash[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  const uint8_t VALUE = Combine.length();
  int j = 0;
  for (auto Letter : Combine) {
    srand(VALUE * Letter);
    hash[j++] = 33 + rand() % 92;
  }
  /*for (int i = 0; i < 10; i++)
  {
          cout << hash[i];
  }*/
  return hash[10];
}