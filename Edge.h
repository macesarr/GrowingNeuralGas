#ifndef EDGE_H
#define EDGE_H

#include "Node.h"

struct Node;

struct Edge{
  int age;
  Node * to;
  Edge(Node * to, int age){
    this->age = age;
    this->to = to;
  }
};

#endif
