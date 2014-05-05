/*  $Id: BoolExpr.cpp,v 1.14 2003/03/12 01:09:35 sarrazip Exp $
    BoolExpr.cpp - Boolean expression binary tree node

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
#error "This file is only meant to be included by Bool_expr.h"
#endif

#include <cassert>
#include <sstream>
#include <algorithm>


static bool needsQuotes(const std::string &s)
{
    return s.find_first_not_of(
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_")
	!= std::string::npos;
}


template <class T>
BoolExpr<T>::BoolExpr(const T &initValue /*= T()*/)
  : type(VALUE),
    value(initValue),
    left(NULL),
    right(NULL)
{
}


template <class T>
BoolExpr<T>::BoolExpr(Type t, BoolExpr *l, BoolExpr *r)
  : type(t),
    value(),
    left(l),
    right(r)
{
    assert(type == AND || type == OR || type == NOT);
}


template <class T>
BoolExpr<T>::~BoolExpr()
{
    delete left;
    delete right;
}


template <class T>
inline
typename BoolExpr<T>::Type
BoolExpr<T>::getType() const
{
    return type;
}


template <class T>
inline
const BoolExpr<T> *
BoolExpr<T>::getLeft() const
{
    return left;
}


template <class T>
inline
BoolExpr<T> *
BoolExpr<T>::getLeft()
{
    return left;
}


template <class T>
inline
const BoolExpr<T> *
BoolExpr<T>::getRight() const
{
    return right;
}


template <class T>
inline
BoolExpr<T> *
BoolExpr<T>::getRight()
{
    return right;
}


template <class T>
inline
const T &
BoolExpr<T>::getValue() const
{
    return value;
}


template <class T>
inline
void
BoolExpr<T>::setType(Type t)
{
    type = t;
}


template <class T>
inline
void
BoolExpr<T>::setLeft(BoolExpr *subtree)
{
    left = subtree;
}


template <class T>
inline
void
BoolExpr<T>::setRight(BoolExpr *subtree)
{
    right = subtree;
}


template <class T>
inline
void
BoolExpr<T>::setValue(const T &v)
{
    value = v;
}


template <class T>
inline
T &
BoolExpr<T>::getValue()
{
    return value;
}


template <class T>
bool
BoolExpr<T>::isDisjunctiveNormalForm() const
{
    if (type == VALUE)
	return (left == NULL && right == NULL);

    if (type == NOT)
    {
	if (left != NULL || right == NULL || !right->isDisjunctiveNormalForm())
	    return false;

	return (right->type == VALUE || right->type == NOT);
    }

    if (type == AND)
    {
	if (left == NULL || right == NULL
			|| !left->isDisjunctiveNormalForm()
			|| !right->isDisjunctiveNormalForm())
	    return false;

	return (left->type != OR && right->type != OR);
    }

    if (type == OR)
    {
	if (left == NULL || right == NULL
			|| !left->isDisjunctiveNormalForm()
			|| !right->isDisjunctiveNormalForm())
	    return false;

	return true;
    }

    return false;
}


template <class T>
template <class OutputIter>
OutputIter
BoolExpr<T>::getDNFTermRoots(OutputIter dest) const
{
    switch (type)
    {
	case OR:
	    assert(left != NULL);
	    assert(right != NULL);
	    dest = left->getDNFTermRoots(dest);
	    return right->getDNFTermRoots(dest);

	case AND:
	case NOT:
	    assert(right != NULL);
	    assert((left == NULL) == (type == NOT));
	case VALUE:
	    *dest++ = this;
	    return dest;

	default:
	    assert(false);
	    return dest;
    }
}


template <class T>
void
BoolExpr<T>::getTreeVariables(
			std::set<T> &positives, std::set<T> &negatives) const
{
    if (type == VALUE)
    {
	positives.insert(value);
	return;
    }

    if (type == NOT)
    {
	assert(right != NULL);
	negatives.insert(right->value);
	return;
    }

    assert(type == OR || type == AND);
    assert(left != NULL && right != NULL);
    left->getTreeVariables(positives, negatives);
    right->getTreeVariables(positives, negatives);
}


template <class T>
bool
BoolExpr<T>::isDNFTermUseful() const
{
    std::set<T> positives, negatives;
    getTreeVariables(positives, negatives);
    
    std::set<T> intersection;
    set_intersection(positives.begin(), positives.end(),
			negatives.begin(), negatives.end(),
			inserter(intersection, intersection.end()));

    return intersection.empty();
}


template <class T>
void
BoolExpr<T>::print(std::ostream &out) const
{
    switch (type)
    {
	case VALUE:
	    {
		bool q = needsQuotes(value);
		if (q)
		    out << "\"";
		out << value;
		if (q)
		    out << "\"";
	    }
	    break;

	case NOT:
	    {
		bool paren = (right->type == AND || right->type == OR);
		out << '!';
		if (paren)
		    out << '(';
		right->print(out);
		if (paren)
		    out << ')';
	    }
	    break;

	case OR:
	    {
		left->print(out);
		out << "|";
		right->print(out);
	    }
	    break;

	case AND:
	    {
		bool paren = (left->type == OR);
		if (paren)
		    out << '(';
		left->print(out);
		if (paren)
		    out << ')';

		out << '&';

		paren = (right->type == OR);
		if (paren)
		    out << '(';
		right->print(out);
		if (paren)
		    out << ')';
	    }
	    break;

	default:
	    assert(false);
    }
}


template <class T>
std::string
BoolExpr<T>::print() const
{
    std::ostringstream buffer;
    print(buffer);
    return buffer.str();
}


template <class T>
/*static*/
BoolExpr<T> *
BoolExpr<T>::cloneTree(const BoolExpr<T> *root)
{
    if (root == NULL)
        return NULL;
    
    BoolExpr<T> *leftClone = cloneTree(root->left);
    BoolExpr<T> *rightClone = cloneTree(root->right);
    BoolExpr<T> *cloneRoot = new BoolExpr<T>(root->value);
    cloneRoot->type = root->type;
    cloneRoot->left = leftClone;
    cloneRoot->right = rightClone;

    return cloneRoot;
}


template <class T>
/*static*/
BoolExpr<T> *
BoolExpr<T>::getDisjunctiveNormalForm(BoolExpr<T> *root)
{
    BoolExpr<T> *dnfRoot = getRawDNF(root);
    if (dnfRoot == NULL)
	return dnfRoot;

    std::vector<const BoolExpr<T> *> termRoots;
    dnfRoot->getDNFTermRoots(inserter(termRoots, termRoots.end()));

    // Determine which terms are useful:
    std::vector<BoolExpr<T> *> usefulTerms;
    typename std::vector<const BoolExpr<T> *>::const_iterator it;
    for (it = termRoots.begin(); it != termRoots.end(); it++)
    {
	assert(*it != NULL);
	if ((*it)->isDNFTermUseful())
	    usefulTerms.push_back(const_cast<BoolExpr<T> *>(*it));
    }

    // Nothing to do if all the terms are useful:
    if (usefulTerms.size() == termRoots.size())
	return dnfRoot;

    destroyDNFOrNodes(dnfRoot);
    // dnfRoot now invalid

    return joinTreesWithOrNodes(usefulTerms);
}


template <class T>
/*static*/
void
BoolExpr<T>::destroyDNFOrNodes(BoolExpr<T> *root)
{
    if (root == NULL || root->type != OR)
	return;

    // Detach the OR node's subtrees, so that 'delete' does not affect them:
    BoolExpr<T> *left = root->left, *right = root->right;
    assert(left != NULL && right != NULL);
    root->left = root->right = NULL;
    delete root;

    destroyDNFOrNodes(left);
    destroyDNFOrNodes(right);
}


template <class T>
/*static*/
BoolExpr<T> *
BoolExpr<T>::joinTreesWithOrNodes(const std::vector<BoolExpr<T> *> &trees)
{
    if (trees.size() == 0)
	return NULL;
    if (trees.size() == 1)
	return trees[0];

    typename std::vector<BoolExpr<T> *>::const_iterator it = trees.begin();

    BoolExpr<T> *result = new BoolExpr<T>(OR, *it++, NULL);
    result->right = *it++;
	// *it++ not used twice in same statement because eval order is undef

    for ( ; it != trees.end(); it++)
	result = new BoolExpr<T>(OR, result, *it);

    return result;
}


template <class T>
/*static*/
BoolExpr<T> *
BoolExpr<T>::getRawDNF(BoolExpr<T> *root)
{
    if (root == NULL)
        return NULL;
    
    /*  One-level trees:
    */
    if (root->type == VALUE)
        return root;
    
    /*  Two levels trees:
    */
    assert(root->right != NULL);
    switch (root->type)
    {
        case NOT:
            assert(root->left == NULL);
            if (root->right->type == VALUE)
                return root;
            break;

        case OR:
        case AND:
            assert(root->left != NULL);
            if (root->left->type == VALUE && root->right->type == VALUE)
                return root;
            break;
            
        default:
            assert(false);
    }


    /*  Three or more levels:
    */
    root->left = getRawDNF(root->left);
    root->right = getRawDNF(root->right);


    assert(root->right != NULL);
    if (root->type == NOT)
    {
        assert(root->left == NULL);

        if (root->right->type == NOT)
        {
            /*  Two NOTs make a positive:
            */
            assert(root->right->left == NULL);
            assert(root->right->right != NULL);

            BoolExpr<T> *newRoot = root->right->right;
            root->right->right = NULL;
            delete root;  // deletes two nodes
            return getRawDNF(newRoot);
        }

        if (root->right->type == OR)
        {
            BoolExpr<T> *a = root->right->left;
            BoolExpr<T> *b = root->right->right;
            // We have !(a | b), which becomes !a && !b:
            root->right->left = root->right->right = NULL;
            delete root->right;  // destroy the OR node
            BoolExpr<T> *notA = root;
            notA->right = a;
            BoolExpr<T> *notB = new BoolExpr<T>(NOT, NULL, b);
            BoolExpr<T> *newRoot = new BoolExpr<T>(AND, notA, notB);
            return getRawDNF(newRoot);
        }

        if (root->right->type == AND)
        {
            BoolExpr<T> *a = root->right->left;
            BoolExpr<T> *b = root->right->right;
            // We have !(a & b), which becomes !a | !b:
            root->right->left = root->right->right = NULL;
            delete root->right;  // destroy the AND node
            BoolExpr<T> *notA = root;
            notA->right = a;
            BoolExpr<T> *notB = new BoolExpr<T>(NOT, NULL, b);
            BoolExpr<T> *newRoot = new BoolExpr<T>(OR, notA, notB);
            return getRawDNF(newRoot);
        }

        assert(false);
        return NULL;
    }


    assert(root->type != NOT);
    assert(root->left != NULL);


    /*  If one side is a value, make sure that this value is at the left:
    */
    if (root->right->type == VALUE)
        swap(root->left, root->right);

    assert(root->right->type != VALUE);


    /*  Permutate the left and right subtrees if they are
            not in our "conventional order":
    */
    if (root->left->type == NOT && root->right->type == OR)
        swap(root->left, root->right);
    else if (root->left->type == NOT && root->right->type == AND)
        swap(root->left, root->right);
    else if (root->left->type == OR && root->right->type == AND)
        swap(root->left, root->right);


    /*  Conventional order:
            root->left->type and root->right->type are expected to be
            equal or to be one of (OR, NOT), (AND, NOT), (AND, OR).
    */

    if (root->type == OR)
    {
        if (root->left->type == VALUE && root->right->type == NOT)
        {
            // Expected because of recursion step:
            assert(root->right->right->type == VALUE);

            return root;
        }

        if (root->left->type == VALUE)
            return root;

        if (root->left->type != NOT && root->right->type != NOT)
            return root;

        if (root->left->type == NOT)
        {
            assert(root->left->left == NULL);
            assert(root->right->type == NOT);  // expected re: conv. order
            assert(root->right->left == NULL);

            // Expected because of recursion step:
            assert(root->left->right->type == VALUE);
            assert(root->right->right->type == VALUE);

            return root;
        }

        if (root->right->type != NOT)
            return root;

        // Expected because of recursion step:
        assert(root->right->right->type == VALUE);
        return root;
    }


    if (root->type == AND)
    {
        if (root->left->type == VALUE && root->right->type == NOT)
        {
            // Expected because of recursion step:
            assert(root->right->right->type == VALUE);

            return root;
        }

        if (root->left->type == VALUE && root->right->type == AND)
            return root;
        
        if (root->left->type == VALUE && root->right->type == OR)
        {
            BoolExpr<T> *andNode = root;
            BoolExpr<T> *x = root->left;
            BoolExpr<T> *orNode = root->right;
            BoolExpr<T> *a = root->right->left;
            BoolExpr<T> *b = root->right->right;

            andNode->right = a;

            BoolExpr<T> *xClone = new BoolExpr<T>(x->getValue());
            BoolExpr<T> *newAndNode = new BoolExpr<T>(AND, xClone, b);

            orNode->left = andNode;
            orNode->right = newAndNode;

            return getRawDNF(orNode);
        }

        if (root->left->type == AND && root->right->type == AND)
            return root;

        if (root->left->type == NOT)
        {
            assert(root->left->left == NULL);
            assert(root->right->type == NOT);  // expected re: conv. order
            assert(root->right->left == NULL);

            // Expected because of recursion step:
            assert(root->left->right->type == VALUE);
            assert(root->right->right->type == VALUE);

            return root;
        }

        if (root->right->type == NOT)
        {
            assert(root->right->left == NULL);
            assert(root->right->right != NULL);
            assert(root->right->right->type == VALUE);

            if (root->left->type == AND)
                return root;
            
            BoolExpr<T> *a = root->left->left;
            BoolExpr<T> *b = root->left->right;
            BoolExpr<T> *c = root->right->right;
            BoolExpr<T> *andNode = root;
            BoolExpr<T> *orNode = root->left;
            BoolExpr<T> *notNode = root->right;

            // We have (a|b) & !c, which becomes (a&!c) | (b&!c):

            BoolExpr<T> *newCNode = new BoolExpr<T>(c->getValue());
            BoolExpr<T> *newNotNode = new BoolExpr<T>(NOT, NULL, newCNode);
            BoolExpr<T> *newAndNode = new BoolExpr<T>(AND, b, newNotNode);

            orNode->left = andNode;
            orNode->right = newAndNode;
            andNode->left = a;
            andNode->right = notNode;
            notNode->right = c;

            return getRawDNF(orNode);
        }

        assert(root->right->type == OR);

        if (root->left->type == OR)
        {
            BoolExpr<T> *a = root->left->left;
            BoolExpr<T> *b = root->left->right;
            BoolExpr<T> *c = root->right->left;
            BoolExpr<T> *d = root->right->right;
            BoolExpr<T> *andNode = root;
            BoolExpr<T> *leftOrNode = root->left;
            BoolExpr<T> *rightOrNode = root->right;

            // We have (a|b) & (c|d), which becomes a&b | a&c | b&c | b&d:

            andNode->left = a;
            andNode->right = c;

            BoolExpr<T> *aClone = cloneTree(a);
            BoolExpr<T> *firstNewAndNode = new BoolExpr<T>(AND, aClone, d);

            BoolExpr<T> *cClone = cloneTree(c);
            BoolExpr<T> *secondNewAndNode = new BoolExpr<T>(AND, b, cClone);

            BoolExpr<T> *bClone = cloneTree(b);
            BoolExpr<T> *dClone = cloneTree(d);
            BoolExpr<T> *thirdNewAndNode = new BoolExpr<T>(AND, bClone, dClone);

            leftOrNode->left = andNode;
            leftOrNode->right = firstNewAndNode;

            rightOrNode->left = secondNewAndNode;
            rightOrNode->right = thirdNewAndNode;

            BoolExpr<T> *newRoot = new BoolExpr<T>(OR, leftOrNode, rightOrNode);
            return getRawDNF(newRoot);
        }

        if (root->left->type == AND)
        {
            BoolExpr<T> *a = root->left->left;
            BoolExpr<T> *b = root->left->right;
            BoolExpr<T> *c = root->right->left;
            BoolExpr<T> *d = root->right->right;
            BoolExpr<T> *topAndNode = root;
            BoolExpr<T> *rightOrNode = root->right;

            BoolExpr<T> *aClone = cloneTree(a);
            BoolExpr<T> *bClone = cloneTree(b);
            BoolExpr<T> *newLowAndNode = new BoolExpr<T>(AND, aClone, bClone);
            BoolExpr<T> *newHighAndNode = new BoolExpr<T>(AND, newLowAndNode, d);

            topAndNode->right = c;

            rightOrNode->left = topAndNode;
            rightOrNode->right = newHighAndNode;
            return getRawDNF(rightOrNode);
        }

        assert(false);
        return NULL;
    }

    return NULL;
}
