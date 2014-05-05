/*  $Id: BoolExpr.h,v 1.13 2003/03/12 01:09:35 sarrazip Exp $
    BoolExpr.h - Boolean expression binary tree node

    boolstuff - Disjunctive Normal Form boolean expression library
    Copyright (C) 2002 Pierre Sarrazin <http://sarrazip.com/>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.
*/

#ifndef BOOLEXPR_H
#define BOOLEXPR_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>


template <class T>
class BoolExpr
{
public:
    enum Type { VALUE, AND, OR, NOT };

    BoolExpr(const T &initValue = T());
    /*
	Creates a VALUE node with the given initial value.
	Type T must have a constructor that requires no arguments.
	If the print() method is called, there must be a method
	of the form operator << (ostream &, const T &).

	This library expects all BoolExpr objects to be allocated
	by operator new.
    */

    BoolExpr(Type t, BoolExpr *l, BoolExpr *r);
    /*
	Creates a node of type 't' (which must be AND, OR or NOT)
	and whose left and right children are 'l' and 'r'.

	Example:
	    BoolExpr<string> *left = new BoolExpr<string>("left subtree");
	    BoolExpr<string> *right = new BoolExpr<string>("right subtree");
	    BoolExpr<string> *root = new BoolExpr<string>(
					BoolExpr<string>::AND, left, right);
	    delete root;
    */

    ~BoolExpr();
    /*
	Calls delete on the left and right subtrees of this node.

	This library expects all BoolExpr objects to be destroyed
	by operator delete.
    */

          Type      getType() const;
    const BoolExpr *getLeft() const;
          BoolExpr *getLeft();
    const BoolExpr *getRight() const;
          BoolExpr *getRight();
    const T        &getValue() const;
          T        &getValue();
    /*
	Return properties of the current tree node.
	Operator delete should not be called on the subtrees returned
	by getLeft() or getRight().  Only the root of a tree should
	be the target of a destruction.
	getValue() should only be called on a node for which getType()
	returns BoolExpr::VALUE.
    */

    void setType(Type t);
    void setLeft(BoolExpr *subtree);
    void setRight(BoolExpr *subtree);
    void setValue(const T &v);
    /*
	Be careful when changing an existing tree node.

	setLeft() and setRight() make this node point to a new subtree,
	but if there was already a left or right subtree, it is NOT
	destroyed.  Avoid node leaks...  The pointer passed to these
	two methods may be NULL.

	Nodes of type AND or OR must have left and right subtrees.
	Nodes of type NOT must have a right subtree but have no left subtree.
	Nodes of type VALUE must NOT have subtrees.
	Any value given to nodes of types other than VALUE will be ignored.
    */

    static BoolExpr *cloneTree(const BoolExpr *root);
    /*
	Returns a copy of the designated tree.
	If 'root' is NULL, NULL is returned.
	All nodes in the returned tree are independent copies of those
	in the original tree.
	All the cloned nodes are created with operator new.
	The caller must eventually destroy the cloned tree by calling
	operator delete on its root node.
    */

    static BoolExpr *getDisjunctiveNormalForm(BoolExpr *root);
    /*
	Transforms the designated tree into its Disjunctive Normal Form.
	The proper way to call this method is the following:

	    root = BoolExpr<SomeType>::getDisjunctiveNormalForm(root);

	The original tree root does not necessarily remain the root
	of the transformed tree.

	A simplification is applied: when a term of the form
	a&!a&<something> is seen, it is deleted unless if it the
	root of the whole tree.

	CAUTION: this method can return a NULL pointer; such a result
	should be interpreted as a "false" boolean expression.
	Examples are when the original (or resulting) tree is
	a&!a, or a&!a|b&!b.
	This method also returns NULL if 'root' is NULL.
    */
 
    static BoolExpr *getRawDNF(BoolExpr *root);
    /*
	Like getDisjunctiveNormalForm(), but without simplifications.
    */

    bool isDisjunctiveNormalForm() const;
    /*
	Determines if the boolean expression tree rooted at this node
	is in the Disjunctive Normal Form.
    */

    template <class OutputIter>
    OutputIter getDNFTermRoots(OutputIter dest) const;
    /*
	The DNF is a sum of products.  Each term in this sum is
	represented by a subtree of the tree rooted at the current node.
	This method stores through 'dest' the BoolExpr<T> pointers that
	represent the roots of the term subtrees.

	Returns the iterator at the position past the last insertion.

	The tree must first be in DNF.  See getDisjunctiveNormalForm().

	For example, if the current node is the root a of DNF tree
	representing the expression a&b | c | d&e, then three pointers
	will be stored: one for the 'a&b' subtree, one for the 'c'
	subtree (a single node) and one for the 'd&e' subtree.

	If the tree is a single node, then 'this' designates the
	only term in the sum and 'this' is stored in 'dest'.

	'dest' must support the notation *dest++ and the expression
	*dest must be of type 'const BoolExpr<T> *'.

	The stored pointers must not be destroyed directly.

	Example:
	    vector<const BoolExpr<string> *> termRoots;
	    dnfRoot->getDNFTermRoots(inserter(termRoots, termRoots.end()));
    */

    void getTreeVariables(std::set<T> &positives, std::set<T> &negatives) const;
    /*
	Stores in 'positives' the T values of the nodes that are used
	positively in the tree rooted at this node.
	Stores in 'negatives' the T values of the nodes that are used
	negatively in the tree rooted at this node.

	Example: with T == string and the expression a&b&!a&!c,
	the 'positives' set will contain "a" and "b" and the
	'negatives' set will contain "a" and "c".

	When the intersection between the two sets is not empty
	and the only binary operator used in the tree is AND, the
	tree always evaluates to false (because we have an expression
	of the form (a&!a)&(whatever)).
	If the only binary operator is OR, the tree always evaluates
	to true.
    */

    bool isDNFTermUseful() const;
    /*
	Must only be called on a term of a DNF tree.
	See the getDNFTermRoots() method.
	Returns *false* if and only if the term always evaluates to false
	(e.g., a&b&!a).
    */

    void print(std::ostream &out) const;
    /*
	Prints the boolean expression tree rooted at this node
	in the designated text stream.
	Does not print a newline afterwards.
	Uses no unnecessary parentheses.
	Uses '!', '|' and '&' as the NOT, OR and AND operator.

	However, consider this:

	"Wouldn't the sentence 'I want to put a hyphen between the
	words Fish and And and And and Chips in my Fish-And-Chips
	sign' have been clearer if quotation marks had been placed
	before Fish, and between Fish and and, and and and And,
	and And and and, and and and And, and And and and, and
	and and Chips, as well as after Chips?"

	If this method is called, there must be a method
	of the form operator << (ostream &, const T &).
    */

    std::string print() const;
    /*
	Like the previous method, but prints to a string instead,
	and returns this string.
    */

private:
    Type type;
    T value;
    BoolExpr *left;
    BoolExpr *right;

    friend class BoolExprParser;

    static void destroyDNFOrNodes(BoolExpr<T> *root);
    static BoolExpr<T> *joinTreesWithOrNodes(
				const std::vector<BoolExpr<T> *> &trees);
};


template <class T>
inline
std::ostream &
operator << (std::ostream &out, const BoolExpr<T> *root)
/*
    Prints nothing if 'root' is NULL.
*/
{
    if (root != NULL)
	root->print(out);
    return out;
}


#include "Bool_expr.cpp"


#endif  /* _H_BoolExpr */
