# -*- coding: utf-8 -*-

###########################
# Ulises Peña B.          #
# Prof. Martín Gutiérrez  #
# Sistemas Operativos     #
# Sección 02              #
# Tarea 1 - Enunciado 1   #
###########################

## Explicación código : ########################################################################
#  - Primero genero un árbol binario de n nodos con el script 'tree.py'.                       #
#  - Imprimo en un archivo el 'path' de cada nodo (cómo llegar a ellos, ej : 'lrl').           #
#  - Se ejecuta el programa escrito en C++, el cual genera procesos en forma de árbol binario. #
#  - Mediante los argumentos del programa, se le va pasando el 'path' del nodo a crear.        #
#  - Si el 'path' del nodo NO se encuentra en el archivo, entonces el nodo no se crea          #
#  - y se termina la ejecución.                                                                #
#  - Ya con los PID's y sus respectivos 'paths',                                               #
#  - se imprime una representación visual del árbol de procesos.                               #
################################################################################################

import subprocess
import sys
import importlib
import time

binary_tree = importlib.import_module('binary_tree')

n = sys.argv[1]

if int(n) <= 0 :
  print('Ingrese un número válido.')
  sys.exit(1)

gen_paths = subprocess.Popen(['python','tree.py',n])
gen_paths.wait()

if gen_paths.returncode != 0 :
  print('El programa terminó su ejecución. [status 1]')
  sys.exit(1)

process = subprocess.Popen(['./root',n])
process.wait()

if process.returncode != 0 :
  print('El programa terminó su ejecución. [status 1]')
  sys.exit(1)

time.sleep(1)

file = open('process_tree.txt','r')

paths = []

for line in file :
  line_arr = line.split()

  if len(line_arr) == 1 :
    paths.append((line_arr[0],'',0))
  else :
    paths.append((line_arr[0],line_arr[1],len(line_arr[1])))

file.close()

for i in range(len(paths)) :
  min_index = i
  for j in range(i+1,len(paths)) :
    if paths[j][2] < paths[min_index][2] :
      min_index = j
  paths[i], paths[min_index] = paths[min_index], paths[i]

process_tree = binary_tree.BinaryTree()

for process in paths :
  process_tree.insert(process[0],process[1])

process_tree.print_tree()

## ---------------- ##

def file_len(fname):
  with open(fname) as f:
    for i, l in enumerate(f):
      pass
  return i + 1

print('Procesos generados : ' + str(file_len('process_tree.txt')))
