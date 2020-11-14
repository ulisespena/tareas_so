/*
* Ulises Peña B.
* Prof. Martín Gutiérrez
* Sistemas Operativos
* Sección 02
* Tarea 1 - Enunciado 2
*/

#ifndef ALUMNO
#define ALUMNO

#include <string>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <thread>

// variables globales compartidas entre los threads
std::string estado = "normal";
bool running_thread = true;

std::ofstream notas;
std::ifstream armas_toma;
std::ofstream log_toma;

std::string last_action = "";

void clases(std::string);
void paro();
void toma(std::string);
void asamblea();
void info();

void Alumno(std::string notas_file_name, std::string armas_file_name) {
  // reset notas
  std::ofstream notas(notas_file_name, std::ofstream::trunc);
  notas.close();

  log_toma.open("log_toma.txt", std::ofstream::trunc);
  log_toma.close();

  srand(time(NULL));

  //inicializar en modo clases
  std::thread current_thread(clases, notas_file_name);
  std::thread update_info(info);

  while(estado != "exit") {
    asamblea();

    if(!running_thread) {
      current_thread.join();
      running_thread = true;

      if(estado == "normal") {
        current_thread = std::thread(clases, notas_file_name);
      }else if(estado == "paro") {
        current_thread = std::thread(paro);
      }else if(estado == "toma") {
        current_thread = std::thread(toma, armas_file_name);
      }
    }
  }
}

void clases(std::string notas_file_name) {
  if(!notas.is_open()) notas.open(notas_file_name, std::ios::app);

  while(estado == "normal") {
    usleep(500000);
    int nota = rand()%7 + 1;
    notas << "El alumno se ha sacado un " << nota << ".\n";
    last_action = "El alumno se ha sacado un " + std::to_string(nota) + '.';
  }
}

void paro() {
  if(notas.is_open()) notas.close();
}

void toma(std::string armas_file_name) {
  std::ifstream armas(armas_file_name);
  log_toma.open("log_toma.txt", std::ios::app);
  std::string line;

  while(estado == "toma" && std::getline(armas,line)) {
    usleep(500000);
    log_toma << "Alumno utilizó " << line << ".\n";
    last_action = "Alumno utilizó " + line + ".";
  }
  log_toma.close();
  armas.close();
}

void asamblea() {
  std::string eleccion;
  std::cin >> eleccion;

  while(!(eleccion == "1" || eleccion == "2" || eleccion == "3" || eleccion == "4")) {
    std::cin >> eleccion;
  }

  switch(atoi(eleccion.c_str())) {
    case 1 :
      if(estado == "normal") break;
      estado = "normal";
      running_thread = false;
      break;
    case 2 :
    if(estado == "paro") break;
      estado = "paro";
      running_thread = false;
      break;
    case 3 :
      if(estado == "toma") break;
      estado = "toma";
      running_thread = false;
      break;
    case 4 :
      estado = "exit";
      exit(0);
  }
}

void info() {
  while(true) {
    usleep(50000);
    system("clear");
    std::cout << "\n[Estado actual] : " << estado << "\n";
    std::cout << "[Última actualización] : " << last_action << "\n\n";
    std::cout << "-- SE HA CONVOCADO A ASAMBLEA --\n\n";
    std::cout << "\t[1] Ir a clases.\n";
    std::cout << "\t[2] Ir a paro.\n";
    std::cout << "\t[3] Ir a toma.\n";
    std::cout << "\t[4] Finalizar la ejecución.\n";
    std::cout << "\tHaz tu elección: 'input'+[ENTER].\n";
  }
}

#endif