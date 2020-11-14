# -*- coding: utf-8 -*-

##
# Ulises Peña B.
# Prof. Martín Gutiérrez
# Sistemas Operativos
# Sección 02
# Tarea 1 - Enunciado 1
##

import sys
import itertools

def get_depth(i) :
  num_of_nodes = 0
  current_depth = 0

  while True :
    num_of_nodes += 2**current_depth

    if num_of_nodes >= i :
      return current_depth

    current_depth += 1

n = int(sys.argv[1])

if n < 1 :
  sys.exit(1)

dirs = ['l','r']
paths = ['']

for current_depth in range(1,get_depth(n)+1) :
  combs = itertools.product(dirs, repeat=current_depth)

  for comb in combs :
    path = ''

    for _dir in comb :
      path += _dir

    paths.append(path)

file = open('paths.txt','w')

for i in range(n) :
  file.write(paths[i]+'\n')

file.close()
