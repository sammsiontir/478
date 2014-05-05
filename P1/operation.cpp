#include "operation.h"
#include <iostream>

using std::map;
using std::string;
using std::cerr;
using std::cout;
using std::cin; 
using std::endl;

// default operation = and
operation::operation()
{
  init_operation_map();
  command = "and";
  current_op = &operation::and_func;
}

// specify an existing cmd, if it doesn't exist, use the default AND
operation::operation(string cmd)
{
  init_operation_map();

  operation_map_t::iterator it = operation_map.find(cmd);
  if (it != operation_map.end())
  {
    command = cmd;
    current_op = (*it).second;
  }
  else
  {
    cerr << "warning, command " << cmd << "isn't predefined.  Using default AND operation\n";
    command = "and";
    current_op = &operation::and_func;
  }
}

// call the current operation
bdd_ptr operation::operator() (bdd_ptr left, bdd_ptr right)
{
  return (this->*current_op)(left, right);
}

// set the operation.  if no operation exists for the string provided,
// returns false, otherwise returns true to indicate success
bool operation::set_operation(string cmd)
{
  operation_map_t::iterator it = operation_map.find(cmd);
  if (it != operation_map.end())
  {
    command = cmd;
    current_op = (*it).second;
    return true;
  }
  return false;
}

void operation::print_available_operations()
{
  for (operation_map_t::iterator it = operation_map.begin(); it != operation_map.end(); ++it)
    cout << " - " << (*it).first << endl;
}

// keeps on outputting available operations and prompting the user until
// an existing operation is specified, and then sets the current operation
// accordingly
void operation::prompt_for_operation()
{
  bool error;
  do
  {
    cout << "\nchoose one of the following operations:" << endl;  
    print_available_operations();

    string choice;
    cin >> choice;
    if (set_operation(choice))
    {
      error = false;
    }
    else
    {
      cout << "invalid selection.\n";
      error = true;
    }

  } while (error);
}

// initializes the map from operation names to operation functions
// this function must be updated if new opeartions are defined
void operation::init_operation_map()
{
  operation_map["and"] = &operation::and_func;
  operation_map["or"] = &operation::or_func;
  operation_map["xor"] = &operation::xor_func;
}


// operations that work for terminal cases
// a null pointer is returned otherwise

bdd_ptr operation::and_func(bdd_ptr left, bdd_ptr right)
{
  // f & f = f
  if (left == right)
  {
    return left;
  }
  
  if (left->is_terminal())
  {
    // if the left is one, than the result is right. otherwise
    // the result is zero (which left must be since it is 
    // terminal and not one)
    return (left->is_one()) ? right : left;
  }
  if (right->is_terminal())
  {
    // similar logic applied here
    return (right->is_one()) ? left : right;
  }

  return 0;
}

bdd_ptr operation::or_func(bdd_ptr left, bdd_ptr right)
{
  // f + f = f
  if (left == right)
  {
    return left;
  }
  
  if (left->is_terminal())
  {
    // if left is 1, return 1 (left)
    // otherwise, left is 0, so return right
    // (since anything OR 0 = anything)
    return (left->is_one()) ? left : right;
  }
  if (right->is_terminal())
  {
    // similar logic applied here
    return (right->is_one()) ? right : left;
  }
  return 0;
}


bdd_ptr operation::xor_func(bdd_ptr left, bdd_ptr right)
{
  // terminal case: f xor 0 = f
  if (left->is_zero())
  {
    return right;
  }
  if (right->is_zero())
  {
    return left;
  }

  // terminal case: 1 xor 1 = 0
  if (left->is_one() && right->is_one())
  {
    return bdd_node::zero;
  }

  return 0;
}




