
/*
* Ulises Peña B.
* Sección 2
*
* Compilar: 'g++ -o main1 main1.cpp -lpthread'
*
* uso : './main1 m n k j step'
* donde m y n : dimensiones de Chale,
* k : número ciudadanos moderados,
* j : número de ciudadanos flaites,
* step : tiempo de espera en ms entre cada iteración.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include <chrono>

#include "class_n_vars.h"

void Ciudadano_thread(std::pair<int,int> init_pos, std::string tipo, bool last, std::mutex* locks_c, int step_ms) {
  std::pair<int,int> pos = init_pos;

  Ciudadano* self = new Ciudadano(tipo);

  int w = Chale[0].size();
  int h = Chale.size();

  int last_index = (pos.first*w)+pos.second;

  locks_c[last_index].lock();
  Chale[pos.first][pos.second]->set_ciudadano(self);

  if(last) { ready = true; }

  // busy waiting :( arreglar si es posible
  while(!ready) {}

  while(true) {
    if(kill_threads) { return; }
    std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));

    int choice = rand() % 4; // movimiento vecindad de Von Neumann
    int x = movs[choice][0];
    int y = movs[choice][1];

    last_index = (pos.first*w)+pos.second;

    // comprobamos por si se sale de los márgenes
    if(pos.first+y < 0 || pos.first+y >= h || pos.second+x < 0 || pos.second+x >= w) {
      continue;
    }

    std::pair<int,int> mov = std::make_pair(pos.first+y, pos.second+x);

    int index = (mov.first*w)+mov.second;

    if(locks_c[index].try_lock()) {
      if(tipo == "moderado" && Chale[mov.first][mov.second]->esta_saqueada()) {
        locks_c[index].unlock();
        continue;
      }

      Chale[pos.first][pos.second]->set_ciudadano(NULL);
      Chale[mov.first][mov.second]->set_ciudadano(self);
      locks_c[last_index].unlock();
      pos = mov;

      if(tipo == "flaite") { Chale[mov.first][mov.second]->saquear(); }
    }
  }
}

void Profugo_thread(std::pair<int,int> init_pos, std::string nombre, std::mutex* locks_p, std::mutex* locks_c, int step_ms) {
  std::pair<int,int> pos = init_pos;

  Profugo* self = new Profugo(nombre);

  int w = Chale[0].size();
  int h = Chale.size();

  int last_index = (pos.first*w)+pos.second;

  locks_p[last_index].lock();
  Chale[pos.first][pos.second]->set_profugo(self);

  // busy waiting :( arreglar si es posible
  while(!ready) {}

  while(true) {
    // if(kill_threads) { return; }
    std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));

    int choice = rand() % 4; // movimiento vecindad de Von Neumann
    int x = movs[choice][0];
    int y = movs[choice][1];

    last_index = (pos.first*w)+pos.second;

    // comprobamos por si se sale de los márgenes
    if(pos.first+y < 0 || pos.first+y >= h || pos.second+x < 0 || pos.second+x >= w) {
      continue;
    }

    std::pair<int,int> mov = std::make_pair(pos.first+y, pos.second+x);

    int index = (mov.first*w)+mov.second;

    if(locks_p[index].try_lock()) {
      if(Chale[mov.first][mov.second]->esta_saqueada()) {
        locks_p[index].unlock();
      }
      else {
        Chale[pos.first][pos.second]->set_profugo(NULL);
        Chale[mov.first][mov.second]->set_profugo(self);
        locks_p[last_index].unlock();
        pos = mov;
      }
    }

    int in = (pos.first*w)+pos.second;

    // comprueba si lo atraparon
    if(locks_c[in].try_lock()) {
      locks_c[in].unlock();
      continue;
    }
    else {
      Ciudadano* c = Chale[pos.first][pos.second]->get_ciudadano();

      if(!c) {
        continue;
      }
      else {
        std::string tipo = c->get_tipo();

        if(tipo == "flaite") {
          Chale[pos.first][pos.second]->set_profugo(NULL);
          locks_p[in].unlock();
          logs.push_back((nombre == "Piñata" ? pinata_col : sandwich_col) + nombre + reset_col + " pal lobby.");
          atrapados[nombre == "Piñata" ? 0 : 1] = true;
          return;
        }
        else if(tipo == "moderado") {
          int atrapado = rand() % 2;

          if(atrapado) {
            logs.push_back((nombre == "Piñata" ? pinata_col : sandwich_col) + nombre + reset_col + " redactando la carta magna.");
            atrapados[nombre == "Piñata" ? 0 : 1] = true;
            while(true) {
              if(kill_threads) { return; }
              std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));
            }
          }
          else {
            logs.push_back((nombre == "Piñata" ? pinata_col : sandwich_col) + nombre + reset_col + " se escapó por poquiiito.");
          }
        }
      }
    }
  }
}

void print_Chale(int N, int M, int step_ms) {
  while(true) {

    system("clear");
    // leyenda
    std::cout << std::endl;
    std::cout << "  " << pinata_col << pinata << reset_col << ": Piñata" << std::endl;
    std::cout << "  " << sandwich_col << sandwich << reset_col << ": Sandwich" << std::endl;
    std::cout << "  " << c_mod_col << c_mod << reset_col << ": Ciudadano moderado" << std::endl;
    std::cout << "  " << c_flaite_col << c_flaite << reset_col << ": Ciudadano flaite" << std::endl;
    std::cout << "  " << cas_vacia_col << cas_vacia << reset_col << ": Casilla vacía" << std::endl;
    std::cout << "  " << cas_saqueada_col << cas_saqueada << reset_col << ": Casilla saqueada" << std::endl;

    std::cout << std::endl;
    for(int i = 0; i < N; i++) {
      std::cout << "  ";
      for(int j = 0; j < M; j++) {
        Ciudadano* c = Chale[i][j]->get_ciudadano();
        Profugo* p = Chale[i][j]->get_profugo();
        bool saqueada = Chale[i][j]->esta_saqueada();

        char ch;
        std::string color;

        if(p) {
          ch = p->get_nombre() == "Piñata" ? pinata : sandwich;
          if(c) {
            color = c->get_tipo() == "moderado" ? c_mod_col : c_flaite_col;
          }
          else {
            color = p->get_nombre() == "Piñata" ? pinata_col : sandwich_col;
          }
        }
        else if(!p && c) {
          ch = c->get_tipo() == "moderado" ? c_mod : c_flaite;
          color = c->get_tipo() == "moderado" ? c_mod_col : c_flaite_col;
        }
        else {
          if(saqueada) {
            ch = cas_saqueada;
            color = cas_saqueada_col;
          }
          else {
            ch = cas_vacia;
            color = cas_vacia_col;
          }
        }

        std::cout << color << ch << reset_col << " ";
      }
      std::cout << std::endl;
    }
    if(!logs.empty()) {
      std::cout << std::endl << "  ==Logs==" << std::endl;
      int s = logs.size();
      if(s > 3) {
        for(int i = s-3; i < s; i++) {
          std::cout << "  - " << logs[i] << std::endl;
        }
      }
      else {
        for(int i = 0; i < s; i++) {
          std::cout << "  - " << logs[i] << std::endl;
        }
      }
    }
    // comprueba si han los prófugos han sido atrapados
    if(atrapados[0] && atrapados[1]) {
      // terminar el programa
      kill_threads = true;
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));
  }
}

int main(int argc, char* argv[]) {

  if(argc < 6) {
    std::cerr << "Uso: './main m n k j step'." << std::endl;
    return 1;
  }

  const int M = atoi(argv[1]);
  const int N = atoi(argv[2]);
  const int K = atoi(argv[3]);
  const int J = atoi(argv[4]);

  // step time
  const int step_ms = atoi(argv[5]);

  const int P = 2; // 2 prófugos

  if(M == 0 || N == 0 || K == 0 || J == 0 || K+J+P > M*N) {
    std::cerr << "M, N, K o J no válidos." << std::endl;
    return 1;
  }

  // inicializar casillas
  for(int i = 0; i < N; i++) {
    std::vector<Casilla*> temp;
    for(int j = 0; j < M; j++) {
      temp.push_back(new Casilla());
    }
    Chale.push_back(temp);
  }

  const size_t E = M*N;
  // locks acceso a casillas
  std::mutex *locks_c = new std::mutex[E]; // locks ciudadanos
  std::mutex *locks_p = new std::mutex[E]; // locks prófugos

  srand(time(NULL));

  std::vector<int> casillas_disp(E);

  for(int i = 0; i < casillas_disp.size(); i++) { casillas_disp[i] = i; }

  std::vector<std::pair<int,int>> spawn_pts;
  for(int i = 0; i < P+K+J; i++) {
    int j = rand() % casillas_disp.size();
    int index = casillas_disp[j];
    casillas_disp.erase(casillas_disp.begin() + j);
    spawn_pts.push_back(std::make_pair(index/M,index%M));
  }

  // hilos prófugos
  std::thread profugos[P];

  //inicializar prófugos
  profugos[0] = std::thread(Profugo_thread,spawn_pts.at(spawn_pts.size()-1),"Piñata",locks_p,locks_c,step_ms);
  spawn_pts.pop_back();
  profugos[1] = std::thread(Profugo_thread,spawn_pts.at(spawn_pts.size()-1),"Sandwich",locks_p,locks_c,step_ms);
  spawn_pts.pop_back();

  // hilos ciudadanos
  std::thread ciudadanos[K+J];

  // inicializar ciudadanos moderados
  for(int i = 0; i < K; i++) {
    ciudadanos[i] = std::thread(Ciudadano_thread,spawn_pts.at(spawn_pts.size()-1),"moderado",false,locks_c,step_ms);
    spawn_pts.pop_back();
  }

  // inicializar ciudadanos flaites
  for(int i = K; i < K+J; i++) {
    ciudadanos[i] = std::thread(Ciudadano_thread,spawn_pts.at(spawn_pts.size()-1),"flaite",(i==K+J-1?true:false),locks_c,step_ms);
    spawn_pts.pop_back();
  }

  std::thread print_map(print_Chale,N,M,step_ms);

  for(int i = 0; i < P; i++) {
    profugos[i].join(); 
  }

  for(int i = 0; i < K+J; i++) {
    ciudadanos[i].join();
  }

  print_map.join();

  delete [] locks_c;
  delete [] locks_p;

  return 0;
}