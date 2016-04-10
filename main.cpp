#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "Gng.h"

using namespace std;

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
  ifstream file ("Datasets/Iris-setosa.data");
  string value;
  
  while(getline(file, value, ',')){
    vectorString.push_back(value);
  }

  file.close();
  
  for(unsigned i=0; i<vectorString.size(); i++){
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
  
  Gng gng_(x, 4, 10, 50, 50, 0.05, 0.0006, 100, 0.5, 0.0005);
  //gng_.PrintSignals();
  
  gng_.Learning();

  // gng_.PrintNodes();

  /*
    - Distancia euclidiana
    - Una iteracion es pasar por todo el vector de señales.
    - Se condiciona la expación al número de iteración.
  */
  return 0;
}
