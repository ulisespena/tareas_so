/*
* Ulises Peña B.
* Prof. Martín Gutiérrez
* Sistemas Operativos
* Sección 02
* Tarea 1 - Enunciado 1
*/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "util.h"

int main(int argc, char** argv) {

  if(exists_in_file(argv[1],"paths.txt")) {
    std::ofstream process_tree("process_tree.txt",std::ios::app);
    process_tree << getpid() << ' ' << argv[1] << std::endl;
    process_tree.close();

    pid_t left_child = fork();

    if(left_child == 0) {
      char* path = argv[1];
      strcat(path,"l");
      execlp("./spawn","./spawn",path,NULL);
    }

    pid_t right_child = fork();

    if(right_child == 0) {
      char* path = argv[1];
      strcat(path,"r");
      execlp("./spawn","./spawn",path,NULL);
    }
  }

  wait(NULL);
  return 0;
}