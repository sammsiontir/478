#include "bdd_node.h"
#include "operation.h"
#include "bdd_tables.h"
#include "Bool_expr_parser.h"
#include "project1.h"

#include <iostream>
#include <assert.h>
#include <stdlib.h>

using namespace std;

// prototypes
char get_command(string prompt, string cmdlist);
char get_var(bdd_ptr bdd);

bdd_ptr build_bdd_from_input(istream& is);
bdd_ptr bdd_from_expr(BoolExpr<string> *expr);

// Main function. 
// It allows the user to input a boolean expression which is then
// displayed in BDD form, and from there, the user can operate on the BDD 
// until they quit. A memory leak is checked for at the end of the program.
int main(int argc, char *argv[])
{
  cout << "\n-------------------------\n";
  cout << "Project 1: BDDs.\n\n";
  
  cout << "A valid boolean expression consists of\n"
       << "(1) single character vars, and \n";
  cout << "(2) AND, OR, NOT and parenthesis\n";
  cout << "\n example: (a & !b | !a & b) & c\n";
  cout << "-------------------------\n\n";


  bdd_node::zero->probability = 0;
  bdd_node::one->probability = 1;

  // print mode from users
  // verbose means the tables will be printed out
  bool verbose = false;
  char pm = get_command(string("Choose a print mode: (v)erbose, (n)on-verbose\n"), string("vn"));
  if (pm == 'v') verbose = true;
  
  // flush input line
  while ( cin.get() != '\n' );
  cout << endl;
  
  {
  // get the initial boolean expression
  bdd_ptr expr1 = build_bdd_from_input(cin);
  if (expr1 == NULL)
  {
    cout << "function apply not implemented." << endl;
    exit(0);
  }
  
			
  
  cout << "in bdd form:\n" << expr1 << endl;
  cout << endl <<"probability: "<< expr1->probability << endl << endl;
  
  bool done = false;
  while (!done)
  {
    if (verbose)
    {
      cout << "current state of tables:" << endl << endl;
      bdd_tables::getInstance().print_computed_table();
      cout << endl;
      bdd_tables::getInstance().print_unique_table();
      cout << endl;
    }
    
    cout << "Would you like to...\n";
    char choice = get_command(string("\n(a)pply an operation with another bdd?\
									\n(n)egative cofactor? \n(p)ositive cofactor? \
									\n(b)oolean difference? \n(s)ort by influence? \
                                    \n(q)uit\n"),string("anpbsq"));
    // flush input line
    while (cin.get() != '\n');
    cout << endl;
    
    // the result from whichever operation the user chooses
    bdd_ptr result;
    
    // execute the user's choice
    switch (choice)
    {
      case 'n':
      {
        char var = get_var(expr1);
        result = negative_cofactor(expr1, var);
        break;
      }
      case 'p':
      {
        char var = get_var(expr1);
        result = positive_cofactor(expr1, var);
        break;
      }
      case 'b':
      {
        char var = get_var(expr1);
        result = boolean_difference(expr1, var);
        break;
      }
      case 'a':
      {
        bdd_ptr expr2 = build_bdd_from_input(cin);    
        cout << "in bdd form:\n" << expr2 << endl;
  	cout << endl <<"probability: "<< expr2->probability << endl << endl;
        
        operation dop;    
        dop.prompt_for_operation();
        result = apply(expr1, expr2, dop);
        break;
      }
      case 's':
      {
        result = sort_by_influence(expr1);
        break;
      }
      case 'q':
        done = true;
        break;
    }
    
    if (!done)
    {
      // the current expr is now the result
      expr1 = result;
	
      cout << "\nresult:\n" << result << endl;
  	cout << endl <<"probability: "<< expr1->probability << endl << endl;
    }
  }
  }
  // clear out tables (and what should be all remaining references to nodes)
  bdd_tables::getInstance().clear();
  
  // check for memory leak
  if ((bdd_node::id_ctr - bdd_node::free_ids.size()) != 2) cout << "-->memory leak!" << endl;
  
  return 0;
}

// get_var prompts the user for a single character variable until a valid input is
// entered. the stream is left intact.
char get_var(bdd_ptr bdd)
{  
  bool error = true; 
  char var;
  while ( error )
  {
    error = false;
    cout << "Enter a single character variable: ";

    // get string from input, and handle errors if necessary
    if ( !(cin >> var) )
    {
      error = true; 
      cout << "Bad Command.\n";

      // clear the error bits
      cin.clear();

      // flush input line
      while ( cin.get() != '\n' );
    }
    if (!bdd->has_var(var) && !bdd->is_terminal())
    {
      cout << "Variable not part of bdd.\n\n";
      error = true;
    }
  }
  return var;
}


// get_command gets a command from the input stream.
// If a valid command is found, the rest of the stream is left intact.  
// If not, the input stream is flushed, and the user is prompted again
//
// If the last char in cmdlist is a '*', this indicates that any command not found
// in the cmdlist should be interpreted as 'n'
char get_command(string prompt,  // Prompt that is printed to screen asking for input  
                 string cmdlist) // cstring containing acceptable one character responses  
{
  bool error = true; 
  string cmds(cmdlist); // create a string with the commands
  
  if (cmds.size() > 10 || cmds.size() <= 0)
  {
    cout << "in function get_command(), either too many possible commands given,";  
    cout << "or none given at all\n"; 
    exit(1); 
  }
  
  char cmd_char;
  while (error)
  {
    error = false;
    cout << prompt;
    
    // get string from input, and handle errors if necessary
    if (!(cin >> cmd_char))
    {
      error = true;
      cout << "Bad Command.\n";
      
      // clear the error bits
      cin.clear();
      
      // flush input line
      while (cin.get() != '\n');
    }
    else
    {
      // now that we have a cmd, see if it
      // is a valid command from the cmdlist
      if (cmds.find(cmd_char) != string::npos) break;
      
      // in case of last char being *, any other input results in 'n' being returned
      if (cmds[cmds.length() - 1] == '*')
      {
        cmd_char = 'n';
        break;
      }
      cout << "Unrecognized command!\n";
      
      // flush input line
      while ( cin.get() != '\n' );
      error = true; 
    }
  }
  
  return cmd_char;
}

// build_bdd_from_input prompts the user for an expression until a valid
// one is entered.  the GNU boolstuff package is used to parse the expression.
// the BoolExpr is then passed to bdd_from_expression, and the result returned
bdd_ptr build_bdd_from_input(istream& is)
{
  BoolExprParser parser;
  string line;
  BoolExpr<string> *expr;
  bool error;
  
  do
  {
    error = false;
    cout << "Enter a boolean expression:\n";
    if (getline(is, line))
    {
      try
      {
        expr = parser.parse(line);
        //cout << "Original expression     : " << flush;
        //assert(expr != NULL);
        //expr->print(cout);
        //cout << expr;
        //cout << "\n";
      }
      catch (BoolExprParser::Error &err)
      {
        cerr << "Column " << err.index + 1 << ": error #" << err.code << endl;
        error = true;
      }
    }
    else
    {
      cerr << "Bad istream. Exiting.\n";
      exit(1);
    }
  }while (error);
  
  // turn expr into a bdd
  bdd_ptr ret_val = bdd_from_expr(expr);
  
  delete expr;
  
  return ret_val;  
}

// bdd_from_expr takes a boolean expression in the the GNU boolstuff BoolExpr
// format and turns it into a bdd by recursively navigating the expression and
// using apply.
bdd_ptr bdd_from_expr(BoolExpr<string> *expr)
{
  // objects needed for apply
  operation dop;
  bdd_tables &tables = bdd_tables::getInstance();

  switch (expr->getType())
  {
    case BoolExpr<string>::VALUE:
    {
      // Boolstuff accepts strings as variables, we'll just use the first char
      // as the var and disregard the rest
      char var = expr->getValue()[0];
      if (var == '0') return bdd_node::zero;
      
      if (var == '1') return bdd_node::one;
      
      bdd_ptr ret = tables.find_in_unique_table(var, bdd_node::zero, bdd_node::one);
      if (ret == 0) ret =  tables.create_and_add_to_unique_table(var, bdd_node::zero, bdd_node::one);
     
      ret->probability = (bdd_node::zero->probability + bdd_node::one->probability)/2;

      return ret;
      break;
    }
    case BoolExpr<string>::NOT:
    {
      bdd_ptr ret = bdd_from_expr(expr->getRight());
      
      // f XOR 1 = f'
      return apply(ret, bdd_node::one, "xor");
      break;
    }
    case BoolExpr<string>::AND:
    case BoolExpr<string>::OR:
    {
      if ( expr->getType() == BoolExpr<string>::AND ) dop.set_operation("and");
      else                                            dop.set_operation("or");
      bdd_ptr lhs = bdd_from_expr(expr->getLeft());
      bdd_ptr rhs = bdd_from_expr(expr->getRight());
      bdd_ptr ret_node = apply(lhs, rhs, dop);
      return ret_node;
      break;
    }
    default:
      assert(0);
      break;
  }
  assert(0);
  return 0;
}

