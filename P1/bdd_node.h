#ifndef BDD_NODE_H
#define BDD_NODE_H
/*
 * File bdd_node.h
 *
 * Created 5/2003 by Karl Rosaen
 *
 * Modified 9/22/2004 by Karl Rosaen
 *  - added use of smart pointers
 *
 * Contains definition of class Bdd_node and prototypes for
 * helper functions
 */

#include <iostream>
#include <set>

#include "smart_pointer.h"

class bdd_node;
class bdd_tables;

// all pointers to Bdd_nodes will be handeled 
// with smart pointers
typedef smart_pointer<bdd_node> bdd_ptr;

// bdd_node
// The buiding block for BDDs.  Contains a variable that is split on
// pointers to Bdd_nodes that are the negative and positive cofactors
// w.r.t var, and class methods that act on entire BDDs that are made
// up of the Bdd_node and its children
//
// Two static nodes are declared that are the two unique terminal nodes, 
// one and zero
class bdd_node : public Reference_Counted_Object
{
public:

  ~bdd_node();

  // the bdd_ctr used to give each created node its unique id
  static int id_ctr;
  static std::set<int> free_ids;

  // the terminal nodes used by every BDD
  static bdd_ptr one;
  static bdd_ptr zero;

  // this node's unique id
  int get_id() { return id; }

  // only the terminal nodes (one and zero) will have null children
  bool is_terminal() { return (neg_cf == 0); }

  bool is_zero() {return (this == zero); }
  bool is_one() {return (this == one); }

  // prints the contents of the BDD by recursively visiting all of the nodes
  void print(std::ostream& os = std::cout);

  // read and set print mode
  static bool print_mode() {return print_verbose;}
  static void set_print_mode(bool val) {print_verbose = val;}

  // returns whether or not the tree has a certain variable
  bool has_var(char thevar);
  
  // the var that is split on.  irrelevant for static (class wide) nodes one and zero
  char var;

  // children = smart pointers to bdd_nodes representing the positive and
  // negative cofactors with respect to var
  // both nodes = 0 means this is a leaf node
  bdd_ptr neg_cf;
  bdd_ptr pos_cf;
  
  float probability; //holds the probability of each node
		    // must be assigned after each node is created
  


  friend class bdd_tables;

private:
  // can only be created by Bdd_tables
  bdd_node();

  
  int id;  // the unique id of this node
  
  static bool print_verbose; // whether or not to print the node's ids
};


// allows a Bdd to be fed into a stream, i.e cout << bdd1
std::ostream& operator<< (std::ostream& os, bdd_ptr bnode);

// returns the next var to be split on (used in Apply).  Lower value vars
// have precedence
char find_next_var(bdd_ptr bdd1, bdd_ptr bdd2);


#endif

