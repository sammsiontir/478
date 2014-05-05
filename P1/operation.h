#ifndef OPERATION_H
#define OPERATION_H
#include <map>
#include <string>

#include "bdd_node.h"

// operation is a function object (overloads operator () ), that is used
// by the apply function one two bdd_nodes.  Operation manages boolean operations.
// An Operation object allows you to specify what operation you want to apply before
// calling it with two Bdd_nodes.  In general, the operations will return a pointer
// to a Bdd_node if the operation was terminal, or 0 if not (the operation will only
// work if at least one of the nodes is terminal)
//
// Currently, AND and OR are supported, but you can define more if you want, by adding
// a prototype to this definition, providing the function definition in operation.cpp,
// and updating init_operation_map() appropriately. 
class operation
{
public:

  // default operation = and
  operation();

  // specify an existing cmd
  operation(std::string cmd);

  // type: ptr to member function that operates on bdd nodes
  typedef bdd_ptr (operation::*operation_t) (bdd_ptr, bdd_ptr);

  // calls the current operation on left, right. returns 0 if non-terminal
  bdd_ptr operator() (bdd_ptr left, bdd_ptr right);

  // User command management
  bool set_operation(std::string cmd);

  std::string get_operation() {return command;}

  void print_available_operations();

  // prints out the available commands and prompts the user for a valid command
  // loops until one is provided.  the prefered way to set an operation.
  void prompt_for_operation();

  std::string current_operation() {return command;}
private:
  std::string command; // name of current operation
  operation_t current_op; // current operation

   // a mapping from the name of the operation to a pointer to the function
  // that implements it
  typedef std::map<std::string, operation_t> operation_map_t;
  operation_map_t operation_map;  

  // initializes operation_map with the available operation names and pointers
  void init_operation_map();

  // predefined commands

  // return 0 if not terminal
  bdd_ptr and_func(bdd_ptr left, bdd_ptr right);

  bdd_ptr or_func(bdd_ptr left, bdd_ptr right);

  bdd_ptr xor_func(bdd_ptr left, bdd_ptr right);
};

#endif

