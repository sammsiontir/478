#ifndef PROJECT1_H
#define PROJECT1_H

#include "bdd_node.h"
#include "operation.h"
#include "bdd_tables.h"
#include <iostream>
#include <assert.h>
#include <stdlib.h>

// prototypes
bdd_ptr apply(bdd_ptr bdd1, bdd_ptr bdd2, std::string o);
bdd_ptr apply(bdd_ptr bdd1, bdd_ptr bdd2, operation &op);

bdd_ptr negative_cofactor(bdd_ptr np, char var);
bdd_ptr positive_cofactor(bdd_ptr np, char var);
bdd_ptr boolean_difference(bdd_ptr np, char var);
bdd_ptr sort_by_influence(bdd_ptr np);


#endif
