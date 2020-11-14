#ifndef UTIL
#define UTIL

#include <fstream>
#include <stdio.h>

bool exists_in_file(char* line, std::string file_name) {
  std::ifstream file(file_name);

  std::string _line;

  while(!file.eof()) {
    std::getline(file,_line);
    if(line == _line) return true;
  }
  file.close();
  return false;
}
#endif