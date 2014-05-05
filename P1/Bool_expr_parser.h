/*  $Id: BoolExprParser.h,v 1.9 2003/03/15 08:37:17 sarrazip Exp $
    BoolExprParser.h - Boolean expression parser and syntax tree builder

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

#ifndef BOOLEXPRPARSER_H
#define BOOLEXPRPARSER_H

#include "Bool_expr.h"

#include <string>


class BoolExprParser
{
public:

    class Error
    {
    public:
	enum Code
	{
	    GARBAGE_AT_END,
	    RUNAWAY_PARENTHESIS,
	    RUNAWAY_DOUBLE_QUOTES,
	    STRING_EXPECTED,
	    EMPTY_QUOTED_STRING
	};

	size_t index;  // index (>=0) in input string
	Code code;

	Error(size_t i, Code c) : index(i), code(c) {}
    };


    BoolExprParser();

    ~BoolExprParser();

    BoolExpr<std::string> *parse(const std::string &expr) throw(Error);
    /*
	Dynamically allocates a tree of nodes that represents the
	syntactic structure of 'expr'.
	The returned pointer should be passed to the deleteTree() method
	eventually to destroy the tree's nodes.
    */

private:

    std::string curInput;
    size_t curIndex;

    // Implementation methods:
    BoolExpr<std::string> *parseExpr() throw(Error);
    BoolExpr<std::string> *parseTerm() throw(Error);
    BoolExpr<std::string> *parseFactor() throw(Error);
    BoolExpr<std::string> *parseAtom() throw(Error);
    BoolExpr<std::string> *parseString() throw(Error);

    bool atEnd();
    bool tokenSeen(const char *s);
    void skipToken(const char *s);
    void skipSpaces();
    bool isStringChar(char c) const;

    // Forbidden operations:
    BoolExprParser(const BoolExprParser &);
    BoolExprParser &operator = (const BoolExprParser &);
};


#endif  /* _H_BoolExprParser */
