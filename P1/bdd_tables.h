#ifndef BDD_TABLES_H
#define BDD_TABLES_H

/*
 * File bdd_tables.h
 *
 * Created 5/2003 by Karl Rosaen
 *
 * Modified 9/23/2004 by Karl Rosaen
 *  - use of smart pointers
 *  - made bdd_tables a singleton
 *  - computed table now considers operation
 * 
 * Defines the class Bdd_tables, along with a structs computed_table_key
 * and unique_table_key
 */

#include <map>
#include <set>
#include "bdd_node.h"

// a computed table entry is uniquely identified by the operation,
// and the is of the two nodes it was performed on
struct computed_table_key
{
  computed_table_key(std::string op_in, int lhs_in, int rhs_in = 0) :
    op(op_in), lhs(lhs_in), rhs(rhs_in)
  {}

  bool operator< (const computed_table_key& rhs) const;

  std::string op;
  int lhs;
  int rhs;
};


// a unique table entry is uniquely identified by the splitting variable
// of the node, and the ids of its two children
struct unique_table_key
{
  unique_table_key(char var_in, int neg_in, int pos_in) :
  var(var_in), neg_cf_id(neg_in), pos_cf_id(pos_in)
  {}

  bool operator< (const unique_table_key& rhs) const;

  char var;
  int neg_cf_id;
  int pos_cf_id;
};

// bdd_tables handles the bookkeeping of the apply function.  It maintains two
// tables.
//
// computed_table keeps track of all of the results of calling apply on two
// bdd_nodes.  This way, you can check to see if a result already exists by
// calling find_in_computed_table.  When a result is found, you should add
// it to the computed table by calling insert_in_computed_table
// 
// unique_table keeps track of the nodes that make up the new BDD that will
// be the result of the apply function.  once you have a result for a var
// and two Bdd_nodes (one from the first BDD, another for the 2nd BDD), 
// you can check to see if this result already exists by calling
// find_in_unique_table.  If it doesn't exist, you can create the new node
// and add it the the unique table all by calling create_and_add_to_unique_table
class bdd_tables
{
public:

  // Meyers Singleton Accessor: the way to get access to the one and 
  // only bdd_tables instance
  static bdd_tables& getInstance();
  
  bdd_ptr find_in_computed_table(const std::string& op, bdd_ptr bdd1, bdd_ptr bdd2);

  void insert_computed_table(const std::string& op, bdd_ptr bdd1, bdd_ptr bdd2, bdd_ptr computed_node);

  bdd_ptr find_in_unique_table(char var, bdd_ptr left, bdd_ptr right);
  bdd_ptr create_and_add_to_unique_table(char var, bdd_ptr left, bdd_ptr right);

  void clear_computed_table()
  {
    computed_table.clear();
  }
  
  // clear all entries
  void clear()
  {
    clear_computed_table();
    unique_table.clear();
  }

  void print_computed_table();
  void print_unique_table();

private:
  // only construction should be in GetInstance()
  bdd_tables() {}
  
  // dissallow copy and assignment
  bdd_tables(const bdd_tables&);
  bdd_tables& operator=(const bdd_tables&);
  
  // <operation, node id from Bdd1, node id from Bdd2> -> <resulting node>
  typedef std::map<computed_table_key, bdd_ptr> computed_table_t;
  
  computed_table_t computed_table;

  // <var, node from BDD1, node from Bdd2> -> <node in resulting BDD> 
  typedef std::map<unique_table_key, bdd_ptr> unique_table_t;
    
  unique_table_t unique_table;
};

#endif
