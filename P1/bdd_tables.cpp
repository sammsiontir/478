/*
 * File bdd_tables.cpp
 *
 * Created 5/2003 by Karl Rosaen
 *
 * Contains the implementation of the class methods of bdd_tables, defined
 * in bdd_tables.h
 */
 
#include "bdd_tables.h"
#include <iostream>
#include <iomanip> // for setw

using namespace std;


bool computed_table_key::operator<(const computed_table_key& other) const
{
  return( (op < other.op) ||
          (op == other.op && make_pair(lhs, rhs) < make_pair(other.lhs, other.rhs))
        );
}

// determines how to order unique_table_keys based on the structure members
bool unique_table_key::operator< (const unique_table_key& rhs) const
{
  return( (var < rhs.var) ||
          (var == rhs.var && neg_cf_id < rhs.neg_cf_id) ||
          (var == rhs.var && neg_cf_id == rhs.neg_cf_id && pos_cf_id < rhs.pos_cf_id)
        );
}


// Meyers Singleton Accessor
bdd_tables& bdd_tables::getInstance()
{
  // create the tables only the first time this is called
  // (happens automatically since the_tables is static)
  static bdd_tables the_tables;
  return the_tables;
}

// checks to see if the result exists in the computed table.  If so, it returns
// a pointer to the resulting node.  otherwise, returns 0
bdd_ptr bdd_tables::find_in_computed_table(const string& op, bdd_ptr bdd1, bdd_ptr bdd2)
{
  computed_table_t::iterator it = computed_table.find(
    computed_table_key(op, bdd1->get_id(), bdd2->get_id()));
  if (it != computed_table.end())
  {
    return(*it).second;
  }
    
  return 0;
}

// adds an entry to the computed table
void bdd_tables::insert_computed_table(const string& op, bdd_ptr bdd1, bdd_ptr bdd2, bdd_ptr computed_node)
{
  computed_table[computed_table_key(op, bdd1->get_id(), bdd2->get_id())] = computed_node;
}

// looks in the unique table and sees if an entry already exists for a variable and 
// two prospective children.  returns the bdd_node ptr if it finds it, otherwise
// returns 0
bdd_ptr bdd_tables::find_in_unique_table(char var, bdd_ptr left, bdd_ptr right)
{
  unique_table_t::iterator it = unique_table.find(unique_table_key(var, left->get_id(), right->get_id()));

  // if a unique entry exists, return that
  if (it != unique_table.end())
  {
    return(*it).second;
  }
  return 0;
}

// create a new node with var "var", and children "left" and "right", and insert it into the unique table
// returns a pointer to the new node
bdd_ptr bdd_tables::create_and_add_to_unique_table(char var, bdd_ptr left, bdd_ptr right)
{
  // make a new entry, insert it and return it
  bdd_ptr new_node = new bdd_node();
  new_node->var = var;
  
  new_node->neg_cf = left;
  new_node->pos_cf = right;

  unique_table[unique_table_key(var, left->get_id(), right->get_id())] = new_node;


  return new_node;
}

ostream& operator<<(ostream& os, const computed_table_key& ctk)
{
  os << "{" << setw(3) << ctk.op << ", ";
  os << setw(3) << ctk.lhs << ", ";
  os << setw(3) << ctk.rhs << "}";
  return os;
}

void bdd_tables::print_computed_table()
{
  computed_table_t::iterator it = computed_table.begin();
  computed_table_t::iterator end = computed_table.end();

  cout << "computed_table:" << endl;

  for (; it != end; ++it)
  {
    cout << it->first << "  -->  " << it->second->get_id() << endl;
  }
}

ostream& operator<<(ostream& os, const unique_table_key& utk)
{
  os << "{" << utk.var << ", ";
  os << setw(3) << utk.neg_cf_id << ", ";
  os << setw(3) << utk.pos_cf_id << "}";
  return os;
}

void bdd_tables::print_unique_table()
{
  unique_table_t::iterator it = unique_table.begin();
  unique_table_t::iterator end = unique_table.end();

  cout << "unique_table:" << endl;
  
  for (; it != end; ++it)
  {
    cout << it->first << "  -->  " << it->second->get_id() << endl;
  }
}



