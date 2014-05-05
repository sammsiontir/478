# usage: if this file is named "Makefile", then the commands are:
#	"make" will build the specified executable (PROG)
#	"make clean" will delete all of the .o files and the executable
#
# if this file is named something else, then use the -f option for make:
#	"make -f makefilename real_clean"

# set the variables below depending on compiler and options

CC = g++
LD = g++

# add -g to the CFLAGS for debugging (or -ggdb if you use gdb). 
# -ansi and -pedantic attempt to enforce Standard
# -Wmissing-prototypes helps with "safe C"
# -Wall asks for certain warnings of possible errors
# -c is required to specify compile-only (no linking)

CFLAGS = -ansi -pedantic -Wall -ggdb -O3 -c

#no load flags defined, but -l would be used to include a special library

LFLAGS =
OBJS = project1.o main.o bdd_node.o operation.o bdd_tables.o Bool_expr_parser.o 
PROG = project1

default: $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

project1.o: project1.cpp bdd_node.h operation.h bdd_tables.h
	$(CC) $(CFLAGS) project1.cpp

main.o: main.cpp bdd_node.h operation.h bdd_tables.h Bool_expr_parser.h Bool_expr.h Bool_expr.cpp
	$(CC) $(CFLAGS) main.cpp

bdd_node.o: bdd_node.cpp bdd_node.h smart_pointer.h
	$(CC) $(CFLAGS) bdd_node.cpp

operation.o: operation.cpp operation.h bdd_node.h
	$(CC) $(CFLAGS) operation.cpp
        
bdd_tables.o: bdd_tables.cpp bdd_tables.h bdd_node.h
	$(CC) $(CFLAGS) bdd_tables.cpp

Bool_expr_parser.o: Bool_expr_parser.cpp Bool_expr_parser.h Bool_expr.cpp Bool_expr.h
	$(CC) $(CFLAGS) Bool_expr_parser.cpp

clean:
	touch $(PROG) 
	rm $(PROG) $(PROG_LINUX) $(PROG_SOL)
	rm $(OBJS)
