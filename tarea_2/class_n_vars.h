#ifndef CLASS_N_VARS
#define CLASS_N_VARS

#include <string>
#include <vector>

// ==Clases==

class Profugo {
  private:
    std::string nombre;

  public:
    Profugo(std::string nombre) {
      this->nombre = nombre;
    }

    std::string get_nombre() {
      return this->nombre;
    }
};

class Ciudadano {
  private:
    std::string tipo;

  public:
    Ciudadano(std::string tipo) {
      this->tipo = tipo;
    }

    std::string get_tipo() {
      return this->tipo;
    }
};

class Casilla {
  private:
    bool saqueada;
    Profugo* profugo;
    Ciudadano* ciudadano;

  public:
    Casilla() {
      this->saqueada = false;
      this->profugo = NULL;
      this->ciudadano = NULL;
    }

    void saquear() {
      this->saqueada = true;
    }

    bool esta_saqueada() {
      return this->saqueada;
    }

    Profugo* get_profugo() {
      return this->profugo;
    }

    void set_profugo(Profugo* p) {
      this->profugo = p;
    }

    Ciudadano* get_ciudadano() {
      return this->ciudadano;
    }

    void set_ciudadano(Ciudadano* c) {
      this->ciudadano = c;
    }
};

// ======

// ==Variables globales==
bool ready{false};

bool atrapados[2] = {false,false};

bool kill_threads = false;

// Chale, país MxN
std::vector<std::vector<Casilla*>> Chale;

// símbolos
const char cas_vacia = '~';
const char cas_saqueada = '#';
const char c_mod = 'M';
const char c_flaite = 'F';
const char pinata = 'P';
const char sandwich = 'S';

// colores
const std::string reset_col = "\033[0;0m";
const std::string cas_vacia_col = "\033[38;2;230;230;230m";
const std::string cas_saqueada_col = "\033[38;2;100;100;100m";
const std::string c_mod_col = "\033[38;2;55;196;86m";
const std::string c_flaite_col = "\033[38;2;222;117;31m";
const std::string pinata_col = "\033[38;2;55;62;196m";
const std::string sandwich_col = "\033[38;2;196;55;170m";

// movimientos vecindad de Von Neumann
const int movs[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};

// info de eventos para imprimir en pantalla
std::vector<std::string> logs;

#endif