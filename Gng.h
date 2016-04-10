#ifndef GNG_H
#define GNG_H

#include <vector>
#include "Node.h"

using namespace std;

struct Gng{
  vector<Node*> nodes;
  vector< vector<float> > signals;
  Node *S;
  Node *T;

  int labda;
  int aMax;
  float EW;
  float EN;
  int iterations;
  unsigned max_nodes;
  float alpha;
  float beta;
  
  Gng(vector< vector<float> > signals, int length, int limit, int labda, int amax, float ew, float en, unsigned max_nodes, float alpha, float beta){
    this->labda = labda;
    this->aMax = amax;
    this->EW = ew;
    this->EN = en;
    this->iterations = 1;
    this->max_nodes = max_nodes;
    this->alpha = alpha;
    this->beta = beta;
    S = NULL;
    T = NULL;
    
    // INIT:  Create two randomly positioned nodes, connect them with a zero age edge and set their errors to 0.
    vector<float> rv1 = RandomReferenceVector(length, limit);
    vector<float> rv2 = RandomReferenceVector(length, limit);
    Node * one = new Node (rv1, 0);
    Node * two = new Node (rv2, 0);

    // Add nodes
    this->nodes.push_back(one);
    this->nodes.push_back(two);
    // Create edges
    one->AddNode(two, 0);
    two->AddNode(one, 0);
    
    // Generate Vector X
    this->signals = signals;
  }
  
  vector<float> RandomReferenceVector(unsigned length, int limit){
    vector<float> final;
    for(unsigned i=0; i<length; i++){
      final.push_back(rand() % limit);
    }
    return final;
  }

  float DistancePoints(vector<float> w, vector<float> x){
    float value = 0;
    for(unsigned i=0; i<w.size(); ++i){
      value += pow((w[i]-x[i]),2);
    }
    return sqrt(value);
  }

  void Learning(){
    if(nodes.size() > max_nodes) return;
    
    while((iterations%labda)!=0){
      for(unsigned i=0; i<signals.size(); ++i){
	float min_distance = NodesST(signals[i]);
	//Update Error S
	S->error=S->error+min_distance;
	//Move Nodes
	S->MoveMe(EW,signals[i]);
	S->MoveNeighbours(EN,signals[i]);

	//Increment Ages
	S->IncrementAges();

	//S is neighbour??
	S->SEdgeT(T);

	S = NULL;
	T = NULL;
	
      }
      iterations++;
    }
    
    // Find node u with largest error
    //PrintNodesNeighbours();
    
    CreateNewNode();
    DecreaseErrorNodes();
    
    //PrintNodes();

    Learning(); 
  }
  
  float NodesST(vector<float> x){
    vector<Node*>::iterator it;
    S=(*nodes.begin()); // init S node;
    float min = DistancePoints((*nodes.begin())->reference_vector, x);
    float actual = 0;
    for(it=nodes.begin(); it != nodes.end(); it++){
      actual=DistancePoints((*it)->reference_vector, x);
      if(actual < min){
	T=S;
	S=*it;
	min=actual;
      }else{
	T=*it;
      }
    }
    return min;
  }

  void CreateNewNode(){
    // Find Node U. The Node with latgest error in all the graph
    vector<Node*>::iterator it;
    float maxError = (*nodes.begin())->error;
    Node *U = (*nodes.begin());
    for(it=nodes.begin(); it!=nodes.end(); it++){
      if((*it)->error > maxError){
	maxError = (*it)->error;
	U = (*it);
      }
    }
    
    cout << "maxError: " << maxError << endl;
    // Find Node V. Nieghbour's U with largest error
    Node *V = U->FindNeighbourV();

    // Insert Node R
    // Reference vector R
    vector<float> wR = NewRV(U->reference_vector, V->reference_vector);
    float errorR = 0;
    // UpdateErrors
    U->error = alpha * (U->error);
    V->error = alpha * (U->error);
    errorR = U->error;
    // Create Node R
    Node * R = new Node (wR, errorR);
    // Add Node in the graph
    nodes.push_back(R);
    // Create Edge;
    U->AddNode(R, 0); // U->R
    V->AddNode(R, 0); // V->R
    R->AddNode(U, 0);
    R->AddNode(V, 0);

    U->DeleteEdge(V);
    V->DeleteEdge(U);
    
  }

  vector<float> NewRV(vector<float> wu, vector<float> wv){
    vector<float> wr;
    for(unsigned i=0; i<wu.size(); i++) wr.push_back((wu[i]+wv[i])/2);
    return wr;
  }

  void DecreaseErrorNodes(){
    for(unsigned i=0; i<nodes.size(); i++) nodes[i]->error = nodes[i]->error-(beta * nodes[i]->error);
  }
  
  void PrintSignals(){
    for(unsigned i=0; i<signals.size(); i++){
      for(unsigned j=0; j<signals[j].size(); j++){
	cout << signals[i][j] << " ";
      }
      cout << endl;   
    }
  }

  void PrintNodesNeighbours(){
    cout << "Print Nodes and Neightbours" << endl;
    vector<Node*>::iterator it;
    for(it=nodes.begin();it!=nodes.end();++it) (*it)->Print();
  }
  
  void PrintNodes(){
    cout << "Print Nodes" << endl;
    vector<Node*>::iterator it;
    for(it=nodes.begin();it!=nodes.end();++it) (*it)->PrintRV();
  }
  
};

#endif
