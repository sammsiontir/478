#include "circuit.h"
#include <math.h>
#include <stdlib.h>
#include <utility>
#include <iostream>
#include <fstream>

using namespace std;

void usage(const char* exename);

//funcion implementing topological sort
void topological(Circuit &c,int &size,Node** &seq);

//function implementing simulation
void simulation(Circuit &c,int &size,Node** &seq,vector<string> PIs,vector<int> PIs_value);


int main(int argc, char **argv)
{
  // parsing inputsW
  if (argc < 2)
  {
    usage(argv[0]);
  }
  
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i] == string("-h") || argv[i] == string("-help"))
    {
      usage(argv[0]);
    }
    
    else if (argv[i] == string("-topoSort"))
    {
      if (i + 1 < argc)
      {
        string inFilename = string(argv[++i]);
        Circuit c(inFilename);
	// your code here
	Node** seq = NULL;
	int size;
	
	//call topological sorting to obtain the sequence
	topological(c,size,seq);	

	//output the sequence
	cout<<"*** Topological order:"<<endl;	
	for(int i = 0;i<size;i++)
	{
		cout<<seq[i]->getName()<<" ";
	}

	cout<<endl;

      }
      else
      {
        cout << "option -topoSort requires an additional argument." << endl;
        usage(argv[0]);
      }
    }
    else if (argv[i] == string("-simulate"))
    {
      if (i + 2 < argc)
      {
        string inFilename = string(argv[++i]);
        Circuit c(inFilename);
        string inputFile(argv[++i]);
        // your code here
        Node** seq = NULL;
	int size;
	//first call topological sorting	
	topological(c,size,seq);	
	
	//then read the input data file
	ifstream indata;
	indata.open(inputFile.c_str());
	vector<string> PIs;
	vector<int> PIs_value;
	vector<Node*> POs;
	string ss;
	int i;
	indata>>ss;
	indata>>i;	
	while(!indata.eof())
	{	
		PIs.push_back(ss);
		PIs_value.push_back(i);		
		//cout<<ss<<"  "<<i<<endl;
		indata>>ss;
		indata>>i;
	}  

   
	//call simulation functions
	simulation(c,size,seq,PIs,PIs_value);	
	
	//gather the primary outputs and show the results
	POs = c.getPOs();
	cout<<"*** Outputs:"<<endl;		
	cout<<POs[0]->getName()<<" = "<<POs[0]->get_value();		
	for(unsigned int i = 1;i<POs.size();i++)
	{
		cout<<", "<<POs[i]->getName()<<" = "<<POs[i]->get_value();
	}
	cout<<endl;
      }
      else
      {
        cout << "option -simulate requires two additional arguments." << endl;
        usage(argv[0]);
      }
    }
    
  }
  
  return 0;
}

void usage(const char* exename)
{
  cout << "Usage: " << exename << " <options> " << endl;
  cout << "-h or -help                     prints out this help message. " << endl;
  cout << "-topoSort <inFile>              prints a topological ordering of the circuit in <inFile>. " << endl;
  cout << "-simulate <inFile> <inputs>     simulates the circuit in <inFile> with the inputs in <inputs>." << endl;
  cout << endl;
  
  exit(0);
}


//Using circuit c and size(the number of total nodes) as input 
//the results are saved in the node* array seq 

void topological(Circuit &c,int &size, Node** &seq)
{
	vector<Node*> V1,V2,topo;
	vector<Node*> *V1_ptr,*V2_ptr,*temp_ptr;	
	V1 = c.getPIs();
	topo = c.getPIs();
	V1_ptr = &V1;
	V2_ptr = &V2;
	
        //start BFS procedure
	while(V1_ptr->size()!=0)
	{	
		for(vecIter it = V1_ptr->begin();it!= V1_ptr->end();it++)
		{
			//for each fanout,marked visited by decreasing it's  fanin_num by 1
			for(unsigned int i = 0;i<(*it)->getFanout().size();i++)	
			{
				(*it)->getFanout()[i]->dec_fanin_num();	
				//if fanin_num = 0, means that all the fan-in are visited. 
				//Then push this node into topological sort sequence				
				if((*it)->getFanout()[i]->fanin_sat())	
				{
					topo.push_back((*it)->getFanout()[i]);
					V2_ptr->push_back((*it)->getFanout()[i]);
				}		
			}
		
		}
		temp_ptr = V1_ptr;
		V1_ptr = V2_ptr;
		V2_ptr = temp_ptr;
		V2_ptr->clear();
	}
	
	seq = (Node**)calloc(topo.size(),sizeof(Node*));
	
	//copy the vector elements to node* array
	for(unsigned int i = 0;i<topo.size();i++)
	{
		seq[i] = topo[i];
	}
	
	size = topo.size();

	return;
}


//unsing the result of topological sorting as input,
//

void simulation(Circuit &c,int &size,Node** &seq,vector<string> PIs,vector<int> PIs_value)
{
	//set value of primary input	
	for(unsigned int i = 0;i<PIs.size();i++)
	{
		for(unsigned int j = 0;j<PIs.size();j++)
		{
			if(PIs[j].compare(seq[i]->getName())==0)
			{	
				seq[i]->set_value(PIs_value[j]);
			}
		}
	}

	//set value to each gate
	for(int i = PIs.size();i<size;i++)
	{
		unsigned int j;
                int k;								
		
		//first call get_type() function to obtain the type of current node	
		//according to each type, use the information from fan_in to decide the fan_out value	
		switch(seq[i]->get_type())
		{
			case 0://and gate		
			for(j = 0;j<seq[i]->getNumFanin();j++)
			{
				if(seq[i]->getFanin()[j]->get_value()==0)
					break;
			}
			if(j==seq[i]->getNumFanin())
				seq[i]->set_value(1);
			else
				seq[i]->set_value(0);

			//cout<<"and  "<<seq[i]->get_value()<<endl;
			break;
			case 1://or gate
			for(j = 0;j<seq[i]->getNumFanin();j++)
			{
				if(seq[i]->getFanin()[j]->get_value()==1)
					break;
			}
			if(j==seq[i]->getNumFanin())
				seq[i]->set_value(0);
			else
				seq[i]->set_value(1);
			//cout<<"or  "<<seq[i]->get_value()<<endl;

			break;
			case 2://xor gate
			k = seq[i]->getFanin()[0]->get_value();			
			for(j = 1;j<seq[i]->getNumFanin();j++)
			{
				if(k==seq[i]->getFanin()[j]->get_value())
					k = 0;
				else
					k = 1;
			}
			seq[i]->set_value(k);
			//cout<<"xor  "<<seq[i]->get_value()<<endl;
			break;
			case 3://not gate
			if(seq[i]->getFanin()[0]->get_value()==1)
				seq[i]->set_value(0);
			else
				seq[i]->set_value(1);

			//cout<<"not  "<<seq[i]->get_value()<<endl;
				
			break;
			//default:
				//cout<<"unspecified type"<<endl;
		}
		
	}
	
}



