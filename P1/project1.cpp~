/*
 * Contains the the apply function, the cofactors and quantification functions.
 *
 * For Project 1, implement
 * (1) apply, also handles probabilities
 * (2) negative_cofactor
 * (3) positive_cofactor, 
 * (4) boolean_difference
 * (5) sort by influence
 */

#include "project1.h"
#include <algorithm>
#include <vector>

using namespace std;

// wrapper function to allow calling with the operation, i.e apply(bdd1, bdd2, "or")
bdd_ptr apply(bdd_ptr bdd1, bdd_ptr bdd2, string o)
{
  operation dop;
  if (!dop.set_operation(o))
  { 
    return 0;
  }
  return apply(bdd1, bdd2, dop);
}

// apply implements an arbitrary operation (specified in op) on two BDDs
// bdd_tables is used to handle the book keeping (see bdd_tables.h).
//
// apply works recursively one the idea that given an arbitrary operation $, 
// and functions f and g: f $ g = a'(fa' $ ga') + a(fa $ ga), 
// where a is a variable, fa' is the negative cofactor etc.
bdd_ptr apply(bdd_ptr bdd1, bdd_ptr bdd2, operation &op)
{
  // get reference to tables
  bdd_tables& tables = bdd_tables::getInstance();
  // ... your code goes here
  bdd_ptr res;

  res = tables.find_in_computed_table(op.get_operation(), bdd1, bdd2);
  if (res) return res;

  // terminal case  
  res = op(bdd1,bdd2);
  if (res) {
    if (res->is_zero()) res->probability = 0;
    else if(res->is_one()) res->probability = 1;
    else if(bdd1->is_terminal()) res->probability = bdd2->probability;
    else res->probability = bdd1->probability;
    return res;
  }

  // not a terminal case
  char var = find_next_var(bdd1, bdd2);

  if (bdd1->var == var && bdd2->var == var) 
    res = tables.create_and_add_to_unique_table(var, apply(bdd1->neg_cf, bdd2->neg_cf, op), apply(bdd1->pos_cf, bdd2->pos_cf, op));
  else if (bdd1->var == var && bdd2->var != var) 
    res = tables.create_and_add_to_unique_table(var, apply(bdd1->neg_cf, bdd2, op), apply(bdd1->pos_cf, bdd2, op));
  else   //bdd1->var != var && bdd2->var == var
    res = tables.create_and_add_to_unique_table(var, apply(bdd1, bdd2->neg_cf, op), apply(bdd1, bdd2->pos_cf, op));

  if (res->neg_cf->get_id() == res->pos_cf->get_id()) return res->neg_cf;

  tables.insert_computed_table(op.get_operation(), bdd1, bdd2, res);
  res->probability = 0.5*res->neg_cf->probability + 0.5*res->pos_cf->probability;
  // change the return value when you're finished  
  return res;
}

// negative_cofactor takes the BDD pointed to by np, 
// and returns the negative cofactor with respect to var.
bdd_ptr negative_cofactor(bdd_ptr np, char var)
{
  // get reference to tables
  bdd_tables& tables = bdd_tables::getInstance();
  
  //... your code goes here
  if (np->is_terminal()) return np;
  if(np->var==var) return np->neg_cf;
  if (!(np->has_var(var))) return np;

  bdd_ptr root = tables.find_in_unique_table(np->var, bdd_node::zero, bdd_node::one);
  bdd_ptr root_bar = apply(root, bdd_node::one, "xor");
  bdd_ptr left = apply( root_bar, negative_cofactor(np->neg_cf, var) , "and");
  bdd_ptr right = apply( root, negative_cofactor(np->pos_cf, var), "and");
  return apply( left, right, "or");
}

// posative_cofactor takes the BDD pointed to by np, 
// and returns the posative cofactor with respect to var.
bdd_ptr positive_cofactor(bdd_ptr np, char var)
{
  // get reference to tables
  bdd_tables& tables = bdd_tables::getInstance();
  
  //... your code goes here
  if (np->is_terminal()) return np;
  if(np->var==var) return np->pos_cf;
  if (!(np->has_var(var))) return np;

  bdd_ptr root = tables.find_in_unique_table(np->var, bdd_node::zero, bdd_node::one);
  bdd_ptr root_bar = apply(root, bdd_node::one, "xor");
  bdd_ptr left = apply( root_bar, positive_cofactor(np->neg_cf, var), "and");
  bdd_ptr right = apply( root, positive_cofactor(np->pos_cf, var), "and");
  return apply( left, right, "or");
}

// boolean_difference takes the BDD pointed to by np, 
// and returns the boolean difference with respect to var.
bdd_ptr boolean_difference(bdd_ptr np, char var)
{
  // get reference to tables
  //bdd_tables& tables = bdd_tables::getInstance();
  
  //... your code goes here
  return apply( positive_cofactor(np, var), negative_cofactor(np, var), "xor");
}


// sort_by_influence calculates the influence of all the variables in np
// and displays them in descending order (most influent variable is
// shown first). For this task you can assume the number of variable
// is no greater than 20.

class varList {
public:
  varList(){}
  varList(float prob, char var) {
    this->prob = prob; 
    this->var = var;
  }

  float prob;
  char var;
};

bool Greater(const varList& a, const varList& b) {
  if(a.prob == b.prob)  return a.var < b.var;
  return a.prob > b.prob;
}

bdd_ptr sort_by_influence(bdd_ptr np)
{
  //... your code goes here
  char var_all[64] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_"};
  vector<varList> vecList;

  float prob;
  for (int i = 0; i < 64; i++) {
    if (np->has_var(var_all[i])) {
      bdd_ptr tmp = boolean_difference(np, var_all[i]);
      prob = tmp->probability;

      vecList.push_back(varList(prob,var_all[i]));
    }
  }

  sort(vecList.begin(),vecList.end(),Greater);
  for(size_t i=0; i<vecList.size(); ++i)
    cout << vecList[i].var << "," << vecList[i].prob << endl;

  // this function does not alter the current node, so np must be
  // returned at the end
  return np; 
}


