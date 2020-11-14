
/*
* Ulises Peña B.
* Sección 2
*
* Compilar: 'g++ -o main2 main2.cpp -lpthread'
*
* uso : './main2 m n k j step'
* donde m y n : dimensiones de Chale,
* k : número ciudadanos moderados,
* j : número de ciudadanos flaites,
* step : tiempo de espera en ms entre cada iteración.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#include "class_n_vars.h"

std::mutex lock_it;
unsigned long int iteracion = 0;

class Monitor {
  private:
    int step_ms;

    std::mutex m;
    std::condition_variable cv;

  public:
    Monitor(int step_ms) {
      this->step_ms = step_ms;
    }

    void notify_one() { this->cv.notify_one(); }

    // bool : se realizó el movimiento o no
    // string : información acerca de su situación (si ha sido atrapado y por quién)
    std::pair<bool,std::string> p_mov(std::pair<int,int> pos, std::pair<int,int> mov, Profugo* p) {
      if(atrapados[0] && atrapados[1]) { kill_threads = true; }

      // thread espera su turno para ser atendido por el monitor
      std::unique_lock<std::mutex> lock(this->m);
      this->cv.wait(lock);

      lock_it.lock();
      iteracion++;
      lock_it.unlock();

      std::this_thread::sleep_for(std::chrono::milliseconds(this->step_ms));

      Casilla* casilla = Chale[mov.first][mov.second];

      // comprobar si ha sido atrapado
      Ciudadano* c = Chale[pos.first][pos.second]->get_ciudadano();
      if(c) {
        // lo atraparon, no se realiza el movimiento
        this->cv.notify_one();
        return std::make_pair(false, c->get_tipo());
      }
      else {
        // no lo han atrapado, se puede mover
        if(!casilla->get_profugo() && !casilla->esta_saqueada()) {
          // en la casilla no hay otro prófugo y no está saqueada
          Chale[pos.first][pos.second]->set_profugo(NULL);
          Chale[mov.first][mov.second]->set_profugo(p);
          this->cv.notify_one();
          return std::make_pair(true,"");
        }
        this->cv.notify_one();
        return std::make_pair(false,"");
      }
    }

    // bool : si se realizó o no el movimiento
    bool c_mov(std::pair<int,int> pos, std::pair<int,int> mov, Ciudadano* c) {
      if(atrapados[0] && atrapados[1]) { kill_threads = true; }

      // thread espera su turno para ser atendido por el monitor
      std::unique_lock<std::mutex> lock(this->m);
      this->cv.wait(lock);

      lock_it.lock();
      iteracion++;
      lock_it.unlock();

      std::this_thread::sleep_for(std::chrono::milliseconds(this->step_ms));

      Casilla* casilla = Chale[mov.first][mov.second];
      std::string tipo = c->get_tipo();

      if(casilla->get_ciudadano() || (tipo == "moderado" && casilla->esta_saqueada())) {
        // casilla está ocupada,
        // o está saqueada y el ciudadano es moderado
        this->cv.notify_one();
        return false;
      }

      if(tipo == "flaite") { Chale[mov.first][mov.second]->saquear(); }

      Chale[pos.first][pos.second]->set_ciudadano(NULL);
      Chale[mov.first][mov.second]->set_ciudadano(c);
      this->cv.notify_one();
      return true;
    }
};

void Ciudadano_thread(std::string tipo, std::pair<int,int> init_pos, Monitor* monitor) {
  std::pair<int,int> pos = init_pos;

  // dimensiones de Chale
  int w = Chale[0].size();
  int h = Chale.size();

  Ciudadano* self = new Ciudadano(tipo);

  Chale[pos.first][pos.second]->set_ciudadano(self);

  while(!ready) {}

  while(true) {
    if(kill_threads) { return; }

    // determinamos un movimiento vecindad de Von Neumann
    int choice = rand() % 4;
    int x = movs[choice][0];
    int y = movs[choice][1];

    // comprobamos por si se sale de los márgenes
    if(pos.first+y < 0 || pos.first+y >= h || pos.second+x < 0 || pos.second+x >= w) {
      continue;
    }

    std::pair<int,int> mov = std::make_pair(pos.first+y, pos.second+x);

    // si el monitor lo movió, este actualiza su posición
    if(monitor->c_mov(pos, mov, self)) { pos = mov; }
  }
}

void Profugo_thread(std::string nombre, std::pair<int,int> init_pos, Monitor* monitor) {
  std::pair<int,int> pos = init_pos;

  // dimensiones de Chale
  int w = Chale[0].size();
  int h = Chale.size();

  Profugo* self = new Profugo(nombre);

  Chale[pos.first][pos.second]->set_profugo(self);

  while(!ready) {}

  while(true) {
    // determinamos un movimiento vecindad de Von Neumann
    int choice = rand() % 4;
    int x = movs[choice][0];
    int y = movs[choice][1];

    // comprobamos por si se sale de los márgenes
    if(pos.first+y < 0 || pos.first+y >= h || pos.second+x < 0 || pos.second+x >= w) {
      continue;
    }

    std::pair<int,int> mov = std::make_pair(pos.first+y, pos.second+x);

    std::pair<bool,std::string> status = monitor->p_mov(pos, mov, self);

    if(status.first) { pos = mov; }
    if(!status.first && status.second != "") {
      // lo atraparon
      if(status.second == "flaite") {
        Chale[pos.first][pos.second]->set_profugo(NULL);
        atrapados[nombre == "Piñata" ? 0 : 1] = true;
        logs.push_back((nombre == "Piñata" ? pinata_col : sandwich_col) + nombre + reset_col + " pal lobby.");
        return;
      }
      else if(status.second == "moderado") {
        int atrapado = rand() % 2;

        if(atrapado) {
          atrapados[nombre == "Piñata" ? 0 : 1] = true;
          logs.push_back((nombre == "Piñata" ? pinata_col : sandwich_col) + nombre + reset_col + " redactando la carta magna.");
          while(true) {
            if(kill_threads) { return; }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }
        }
        else {
          logs.push_back((nombre == "Piñata" ? pinata_col : sandwich_col) + nombre + reset_col + " se escapó por poquiiito.");
        }
      }
    }
  }
}

void print_map(int M, int N, int step_ms, Monitor* monitor) {

  while(true) {
    if(kill_threads) { return; }
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
    std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));
  }
}

void descolgador(int step_ms, Monitor* monitor) {
  int it = iteracion;

  while(true) {
    if(kill_threads) { return; }
    std::this_thread::sleep_for(std::chrono::milliseconds(2*step_ms));
    if(it == iteracion) {
      // std::cout << "Se había colgao" << std::endl;
      monitor->notify_one();
    }
    else { it = iteracion; }
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

  srand(time(NULL));

  std::vector<int> casillas_disp(M*N);
  for(int i = 0; i < casillas_disp.size(); i++) { casillas_disp[i] = i; }

  std::vector<std::pair<int,int>> spawn_pts;
  for(int i = 0; i < P+K+J; i++) {
    int j = rand() % casillas_disp.size();
    int index = casillas_disp[j];
    casillas_disp.erase(casillas_disp.begin() + j);
    spawn_pts.push_back(std::make_pair(index/M,index%M));
  }

  // hilos prófugos;
  std::thread profugos[P];

  // hilos ciudadanos;
  std::thread ciudadanos[K+J];

  // monitor
  Monitor* monitor = new Monitor(step_ms);

  int size = spawn_pts.size();

  // inicializar prófugos
  profugos[0] = std::thread(Profugo_thread, "Piñata", spawn_pts[--size], monitor);
  spawn_pts.pop_back();

  profugos[1] = std::thread(Profugo_thread, "Sandwich", spawn_pts[--size], monitor);
  spawn_pts.pop_back();

  // inicializar ciudadanos
  for(int i = 0; i < K+J; i++) {
    ciudadanos[i] = std::thread(Ciudadano_thread, (i < K ? "moderado" : "flaite"), spawn_pts[--size], monitor);
  }

  // thread imprimir mapa
  std::thread print_Chale(print_map, M, N, step_ms, monitor);

  // thread descolgador
  std::thread descolg(descolgador, step_ms, monitor);

  // damos tiempo para que todos se posicionen en sus casillas
  // y notificamos para que empiecen a moverse
  std::this_thread::sleep_for(std::chrono::seconds(1));
  ready = true;

  profugos[0].join();
  profugos[1].join();

  for(int i = 0; i < K+J; i++) {
    ciudadanos[i].join();
  }

  print_Chale.join();

  descolg.join();

  return 0;
}