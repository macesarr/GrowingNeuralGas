#include <iostream>
#include <vector>
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <string>

using namespace std;

typedef pair<float, float> point;

struct Node;

struct Edge{
  int age;
  Node * to;
  Edge(Node * to, int age){
    this->age = age;
    this->to = to;
  }
};

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
    vector<Edge>::iterator ite = edges.begin();
    for(ite; ite!=edges.end(); ++ite){
      cout << "**************" << endl;
      cout << "Age: " << (*ite).age << endl;
      (*ite).to->PrintRV();
      cout << "**************" << endl;
    }
    cout << "- - - - - - - - - - - - " << endl;
  }
  
  void PrintRV(){
    cout << "Error: " << error << " | RV: ";
    vector<float>::iterator it = reference_vector.begin();
    for(it; it!=reference_vector.end(); ++it) cout<<(*it)<<",";
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
    vector<Edge>::iterator it = edges.begin();
    for(it; it!=edges.end(); it++){
      (*it).to->MoveMe(en, x);
      (*it).age++;
    }
  }

  void IncrementAges(){
    for(unsigned i=0; i<edges.size(); i++) edges[i].age++;
  }

  void SEdgeT(Node* t){
    vector<Edge>::iterator it = edges.begin();
    for(it; it!=edges.end(); it++){
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
  int max_nodes;
  float alpha;
  float beta;
  
  Gng(vector< vector<float> > signals, int length, int limit, int labda, int amax, float ew, float en, int max_nodes, float alpha, float beta){
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
  
  vector<float> RandomReferenceVector(int length, int limit){
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
    vector<Node*>::iterator it = nodes.begin();
    S=*it; // init S node;
    float min = DistancePoints((*it)->reference_vector, x);
    float actual = 0;
    for(it; it != nodes.end(); it++){
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
    vector<Node*>::iterator it = nodes.begin();
    float maxError = (*it)->error;
    Node *U = (*it);
    for(it; it!=nodes.end(); it++){
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
    for(int i=0; i<signals.size(); i++){
      for(int j=0; j<signals[j].size(); j++){
	cout << signals[i][j] << " ";
      }
      cout << endl;   
    }
  }

  void PrintNodesNeighbours(){
    cout << "Print Nodes and Neightbours" << endl;
    vector<Node*>::iterator it = nodes.begin();
    for(it;it!=nodes.end();++it) (*it)->Print();
  }
  
  void PrintNodes(){
    cout << "Print Nodes" << endl;
    vector<Node*>::iterator it = nodes.begin();
    for(it;it!=nodes.end();++it) (*it)->PrintRV();
  }
  
};


float stof(const char* s){
  float rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1; 
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (float)d;
    };
  };
  return rez * fact;
};

vector< vector<float> > loadFile(){
  vector< vector<float> > vectorFinal;
  int count = 0;
  vector<string> vectorString;
  vector<float> vectorFloat;
  ifstream file ("Iris-setosa.data");
  string value;
  
  while(getline(file, value, ',')){
    vectorString.push_back(value);
  }

  file.close();
  
  for(int i=0; i<vectorString.size(); i++){
    if(count<4){
      if(count==3){
	const char *cstr = value.c_str();
	float temp = stof(cstr);
	vectorFloat.push_back(temp);
	vectorFinal.push_back(vectorFloat);
	vectorFloat.clear();
	count=0;
      }else{
	const char *cstr = vectorString[i].c_str();
	float temp = stof(cstr);
	vectorFloat.push_back(temp);
	count++;
      }
    }
  }
  

  return vectorFinal;
}

int main(){
  srand (time(NULL));
  vector< vector<float> > x = loadFile();

  // Gng(vector< vector<float> > signals, int length, int limit, int labda, int amax, float ew, float en, int max_nodes, float alpha, float beta)
  
  Gng gng_(x, 4, 10, 10, 50, 0.05, 0.0006, 80, 0.5, 0.0005);
  //gng_.PrintSignals();
  
  gng_.Learning();

  //gng_.PrintNodesNeighbours();

  /*
    - Distancia euclidiana
    - Una iteracion es pasar por todo el vector de señales.
    - Se condiciona la expación al número de iteración.
  */
  return 0;
}
