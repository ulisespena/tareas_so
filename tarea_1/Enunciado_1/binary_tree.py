# -*- coding: utf-8 -*-

class Node :
  def __init__(self,pid) :
    self.pid = pid
    self.left = None
    self.right = None

class BinaryTree :
  def __init__(self) :
    self.root = None

    self.lines = []

  def insert(self,pid,path) :
    if self.root == None :
      self.root = Node(pid)
    else :
      node = self.root
      if len(path) > 1 :
        for _dir in path[:-1] :
          if _dir == 'l' :
            node = node.left
          elif _dir == 'r' :
            node = node.right
      if path[-1] == 'l' :
        node.left = Node(pid)
      elif path[-1] == 'r' :
        node.right = Node(pid)

  def exists(self,path) :
    if path == '' :
      return False
    return self._exists(self.root,path)

  def _exists(self,node,path) :
    if node != None and path == '' :
      return True
    elif node == None and path != '' :
      return False
    else :
      _dir = path[0]
      if _dir == 'l' :
        self._exists(node.left,path[1:])
      elif _dir == 'r' :
        self._exists(node.right,path[1:])

  def print_tree(self) :
    self.lines = []
    if self.root == None :
      return
    else :
      self.lines.append(self.root.pid)
      self._print_tree(self.root.left,1)
      self._print_tree(self.root.right,1)

      for i in range(1,len(self.lines)) :
        line = self.lines[i]

        index = 0
        for j in range(len(line)) :
          if line[j] == 'L' :
            index = j
            break

        if index != 0 :
          for k in range(i-1,0,-1) :
            if self.lines[k][index] == ' ' :
              str_list = list(self.lines[k])
              str_list[index] = '|'
              self.lines[k] = ''.join(str_list)
            else :
              break

      for i in range(len(self.lines)-1,0,-1) :
        line = self.lines[i]
        index = 0

        for j in range(len(line)) :
          if line[j] in ['L','|'] :
            index = j

          if index != 0 :
            if self.lines[i-1][index] == 'L' :
              line_list = list(self.lines[i-1])
              line_list[index] = 'T'
              self.lines[i-1] = ''.join(line_list)

      for line in self.lines :
        for c in line :
          if c in ['L','-','T','|'] :
            line_list = list(line)
            index = line_list.index(c)
            if c == 'L' :
              line_list[index] = '└'
              line = ''.join(line_list)
            elif c == '-' :
              line_list[index] = '─'
              line = ''.join(line_list)
            elif c == 'T' :
              line_list[index] = '├'
              line = ''.join(line_list)
            elif c == '|' :
              line_list[index] = '│'
              line = ''.join(line_list)

        print(line)

  def _print_tree(self,node,depth) :
    if node == None :
      return
    else :
      indent = ''
      for i in range(depth) :
        indent += '  '
      self.lines.append(indent + 'L' + '-' + node.pid)
      self._print_tree(node.left,depth+1)
      self._print_tree(node.right,depth+1)
