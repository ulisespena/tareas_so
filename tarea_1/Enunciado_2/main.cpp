/*
* Ulises Peña B.
* Prof. Martín Gutiérrez
* Sistemas Operativos
* Sección 02
* Tarea 1 - Enunciado 2
*/

/*
* Créditos a '1der' y 'Chris Dodd' por código sobre cómo imprimir syscalls en pantalla.
* URL : 'https://stackoverflow.com/questions/11081859/how-to-trace-a-process-for-system-calls'
* El código podría no compilar en un SO de 32 bits, en ese caso cambiar 'ORIG_RAX' por 'ORIG_EAX'.
*
* Output de las syscalls en el archivo 'syscalls.txt'.
* Output de notas en 'notas.txt'.
* Output de toma en 'log_toma.txt'.
* Input de armas y sillas en 'armas.txt'
*
* COMPILACIÓN : 'g++ -o main main.cpp -pthread'
*/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "alumno.h"

int main() {
  int status;
  long orig_rax;

  std::ofstream syscalls("syscalls.txt",std::ofstream::trunc);

  pid_t alumno = fork();

  if (0 == alumno) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    raise(SIGCONT);

    Alumno("notas.txt","armas.txt");
  }
  else {
    while(1){
      wait(&status);
      if(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP){
        orig_rax = ptrace(PTRACE_PEEKUSER,alumno,sizeof(long)*ORIG_RAX,NULL);
        syscalls << "El proceso 'alumno' realizó la syscall " << orig_rax << '.' << std::endl;
      }
      if(WIFEXITED(status) || WIFSIGNALED(status)){
        break;
      }

      ptrace(PTRACE_SYSCALL,alumno,0,0);
    }
  }
  syscalls.close();
  return 0;
}