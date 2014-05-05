#ifndef __NODE_H__
#define __NODE_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

#include "truthTable.h"

// define node type
enum nodeType {PRIMARY_INPUT, PRIMARY_OUTPUT, INTERNAL, ZERO_NODE, ONE_NODE};

// define value type
class Circuit;

class Node
{
  friend class Circuit;
  
  private:
    string name;
    nodeType type;
    vector<Node*> fanin;
    vector<Node*> fanout;
    TruthTable tt;
    int fanin_num;
    int value;

  public:
    // constructors
    Node():type(INTERNAL) {}
    
    Node(const string &nodeName):name(nodeName), type(INTERNAL) {fanin_num = 0;}
    
    // destructor
    ~Node() {};
    
    // returns and sets the name of the node
    string getName() { return name; }
    int setName(const string &n) { name = n; return 0; }
    
    // returns and sets the type of the node
    nodeType getType() { return type; }
    int setType(nodeType t) { type = t; return 0; }
    
    // returns the vector of fanin nodes
    vector<Node*> getFanin() { return fanin; }
    vector<Node*> getFanout() { return fanout; }
    // adds a fanin node
    int addFanin(Node* &inNode) { fanin.push_back(inNode); fanin_num++; return 0; }
    int addFanout(Node* &inNode) { fanout.push_back(inNode); return 0; }
    // returns the number of fanin nodes (variables)
    unsigned getNumFanin() { return fanin.size(); }
    
    // clear functions
    int clearName() { name = ""; return 0; }
    int clearFanin() { fanin.clear(); return 0; }
    int clearTT() { tt.clear(); return 0; }
    int clear() { name = ""; clearFanin(); clearTT(); return 0; }
    
    // prints node information
    int print()
    {
      cout << "Name: " << name << " [TYPE = ";
      switch(type)
      {
        case PRIMARY_INPUT : cout << "PRIMARY_INPUT";  break;
        case PRIMARY_OUTPUT: cout << "PRIMARY_OUTPUT"; break;
        case INTERNAL      : cout << "INTERNAL";       break;
        case ZERO_NODE     : cout << "ZERO_NODE";      break;
        case ONE_NODE      : cout << "ONE_NODE";       break;
      }
      cout << "]" << endl;
      
      if (type == PRIMARY_OUTPUT || type == INTERNAL)
      {
        cout << "Fanin nodes: ";
        for (unsigned i = 0; i < fanin.size(); ++i)
          cout << fanin[i]->name << " ";
        cout << endl;
        tt.print();
      }
      
      return 0;
    }

    void dec_fanin_num(){fanin_num--;}
    int fanin_sat()
    {
	if(fanin_num==0)
		return 1;
	else
		return 0;
    }
    void set_value(int i){value = i;}
    int get_value(){return value;}
    int get_type()
	{
		if(tt.getNumVars()==1)//not gate
			return 3;
		
		if(tt.getNumEntries()==1)//and gate
			return 0;
		else if(tt.getNumEntries()==(pow(2,tt.getNumVars())-1))//or gate
			return 1;
		else if(tt.getNumEntries()==(pow(2,tt.getNumVars())/2))//xor gate
			return 2;
		else
			return -1;
	}
};

#endif

