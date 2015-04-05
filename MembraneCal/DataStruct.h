
#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#define MAXLABLE 10
using namespace std;


struct Rule{
	map<string , int> in;//条件
	map<string , int> out;//结果
	map<string , int> condition;//前提

	map<string , int> ins;//
	map<string , int> outs;//
	
	map<string ,map<string,int>> cpys;
	map<string ,map<string,int>> sons;
	string inNum;
	string outNum;

	int priovity;
	Rule(){
		priovity= 1;//default priovity
		inNum ="";
		outNum ="";
	}
};

struct Membrane{
	Membrane *next;
	vector<Membrane *> sons;
	Membrane *parent;
	map<string,int> data;
	string name;
	vector<Rule> rules;	
	int type;
	int thick;
	int state;
	Membrane(){
		thick = 1;
		next=NULL;
		state=1;
		parent=NULL;
	}
};
struct Move{
	Membrane * from;
	Membrane * to;
	map<string,int> data;
};
vector<Membrane *> dels;
map<Membrane *,	map<string ,map<string,int>> > cpys;

map<Membrane *,	map<string ,map<string,int>> > sons;

vector<Move> outs;
vector<Move> ins;
map<string ,Membrane* > m;
map<string,vector<Rule>> MembraneRules;
map<string,map<string,int>> objects;
map<Membrane *,int> addThick;
map<Membrane *,int> delThick;
int newId=10000;
