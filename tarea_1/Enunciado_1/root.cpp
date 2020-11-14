/*
* Ulises Peña B.
* Prof. Martín Gutiérrez
* Sistemas Operativos
* Sección 02
* Tarea 1
*/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

#include "util.h"

int main(int argc, char** argv) {

  int n = atoi(argv[1]);

  std::ofstream process_tree;
  process_tree.open("process_tree.txt",std::ofstream::trunc);

  if(n < 1) {
    process_tree.close();
    return 0;
  }

  process_tree << getpid() << std::endl;
  process_tree.close();

  pid_t left_child = fork();

  if(left_child == 0) {
    execlp("./spawn","./spawn","l",NULL);
  }else {
    if(n > 2) {
      pid_t right_child = fork();

      if(right_child == 0) {
        execlp("./spawn","./spawn","r",NULL);
      }
    }
  }

  wait(NULL);
  return 0;
}