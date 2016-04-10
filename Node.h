#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <math.h>
#include "Edge.h"

using namespace std;

struct Node{
  vector<float> reference_vector;
  float error;
  vector<Edge> edges;
  Node(vector<float> reference_vector, float error){
    this->reference_vector = reference_vector;
    this->error = error;
  }

  void Print(){
    cout << "Node: " << endl;
    PrintRV();
    cout << "Neighbours: " << endl;
    vector<Edge>::iterator ite;
    for(ite=edges.begin(); ite!=edges.end(); ++ite){
      cout << "**************" << endl;
      cout << "Age: " << (*ite).age << endl;
      (*ite).to->PrintRV();
      cout << "**************" << endl;
    }
    cout << "- - - - - - - - - - - - " << endl;
  }
  
  void PrintRV(){
    cout << "Error: " << error << " | RV: ";
    vector<float>::iterator it;
    for(it=reference_vector.begin(); it!=reference_vector.end(); ++it) cout<<(*it)<<",";
    cout<<endl;
  }

  void AddNode(Node * node, int age){
    Edge edge = Edge(node, age);
    edges.push_back(edge);
  }

  void DeleteEdge(Node* node){
    int position = 0;
    for(unsigned i=0; i<edges.size(); i++)
      if(edges[i].to == node)
	position = i;

    edges.erase(edges.begin()+position);
  }

  void MoveMe(float ew, vector<float> x){
    for(unsigned i=0; i<reference_vector.size(); i++){
      reference_vector[i] = reference_vector[i] + ew*(x[i]-reference_vector[i]);
    }
  }

  void MoveNeighbours(float en, vector<float> x){
    vector<Edge>::iterator it;
    for(it=edges.begin(); it!=edges.end(); it++){
      (*it).to->MoveMe(en, x);
      (*it).age++;
    }
  }

  void IncrementAges(){
    for(unsigned i=0; i<edges.size(); i++) edges[i].age++;
  }

  void SEdgeT(Node* t){
    vector<Edge>::iterator it;
    for(it=edges.begin(); it!=edges.end(); it++){
      if((*it).to == t){
	(*it).age = 0;
      }else{
	this->AddNode(t, 0);
      }
    }
  }

  Node * FindNeighbourV(){
    float maxError = edges[0].to->error;
    Node * V = edges[0].to;
    for(unsigned i=0; i<edges.size(); i++){
      if(edges[i].to->error > maxError){
	maxError = edges[i].to->error;
	V = edges[i].to;
      }
    }
    return V;
  }
  
};

#endif
