/*
 * File bdd_node.cpp
 *
 * Created 5/2003 by Karl Rosaen
 * 
 * Modified 9/23/2004 by Karl Rosaen
 *  - use of smart pointers
 *
 * Contains definitions of static variables and class methods for 
 * the class bdd_node, defined in bdd_node.h, as well as a couple
 * of helper functions
 */

#include <iostream>
#include <string>
#include <map>
#include <cassert>

#include "bdd_node.h"

using namespace std;

// definition of inital id
int bdd_node::id_ctr = 0;
set<int> bdd_node::free_ids;

// default = print out ids
bool bdd_node::print_verbose = true;

// definition of terminal nodes
bdd_ptr bdd_node::zero = new bdd_node();
bdd_ptr bdd_node::one = new bdd_node();

// default ctr
bdd_node::bdd_node() : neg_cf(0), pos_cf(0)
{
  // creation of first node, initialize free_ids
  if (id_ctr == 0)
  {
    id_ctr = 100;
    for (int i = 0; i < bdd_node::id_ctr; ++i)
    {
      bdd_node::free_ids.insert(i);
    }
  }
  
  if (free_ids.empty())
  {
    for (int i = id_ctr; i < 2*id_ctr; i++)
    {
      free_ids.insert(i);
    }
    id_ctr = 2*id_ctr;
  }
  id = *(free_ids.begin());
  free_ids.erase(id);
}


bdd_node::~bdd_node()
{
  // free up this node's id
  free_ids.insert(get_id());
}


ostream& operator<< (ostream& os, bdd_ptr bnode)
{
  bnode->print(os);
  return os;
}



// output the contents of the BDD made up of this bdd_node and 
// its children by recursively visiting all of the nodes and outputting
// their contents.  This is essentially a pre-order visitiation with
// some fancy formatting to make the output more readable
// the convention is that nodes down and to the right are the postive 
// cofactor, and nodes straight down are the negative cofactor
// verbose = true means print out each non-terminal node's id too
void bdd_node::print(ostream& os)
{
  static string spacing("");
  
  string prefix;
  if (spacing.size() != 0)
  {
    prefix = spacing.substr(0, spacing.size() - 3) + " |__";
  }
  else
  {
    prefix = spacing;
  }
  if (is_zero())
  {
    os << prefix << "0\n";
  }
  else if (is_one())
  {
    os << prefix << "1\n";
  }
  else
  {
    if (print_verbose)
    {
      os << prefix << var << " " << get_id() << endl;
    }
    else
    {
      os << prefix << var << endl;
    }
  }

  if (!is_terminal())
  {
    spacing += " | ";
    pos_cf->print(os);
    spacing.resize(spacing.size() - 3);
  
    spacing += "   ";
    neg_cf->print(os);
    spacing.resize(spacing.size() - 3);  
  }
}


// figures out whether or not the BDD pointed to by this node
// has a given variable
bool bdd_node::has_var(char thevar)
{
  if (is_terminal() || var > thevar)
  {
    return false;
  }
  if (var == thevar)
  {
    return true;
  }
  return (pos_cf->has_var(thevar) || neg_cf->has_var(thevar));
}

// the Apply function, given two BDDs (at least one of them must be non-terminal), 
// needs to be able to choose a next var to split on.  Vars that are lower
// alphabetically have precedance (i.e d is prefered over f etc.)
char find_next_var(bdd_ptr bdd1, bdd_ptr bdd2)
{
  // both bdd's aren't terminal, return lower alphabetically
  if (!bdd1->is_terminal() && !bdd2->is_terminal())
  {
    return (bdd1->var < bdd2->var) ? bdd1->var : bdd2->var;
  }
  
  // only bdd1 terminal
  if (!bdd1->is_terminal())
  {
    return bdd1->var;
  }
  else
  {
    // this assert will alert you if this function is called
    // incorrectly (on two terminal nodes)
    assert (!bdd2->is_terminal());
    return bdd2->var;
  }
}





