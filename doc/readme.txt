 ParaMiner : a generic, parallel algorithm for closed pattern mining.
 ====================================================================

Author: bengreve <bengreve@confiance.imag.fr>
Date: 2011-10-18 20:55:25 CEST


Table of Contents
=================
1 Quick start 
2 Compilation 
        2.1 Switching build modes 
3 Running an instance of ParaMiner 
    3.1 Closed frequent itemset mining (FIM) with ParaMiner 
        3.1.1 Input file format 
        3.1.2 Running the FIM instance of ParaMiner 
        3.1.3 Output format 
    3.2 Other built-in instances of ParaMiner 
        3.2.1 Closed frequent relational graphs 
        3.2.2 Closed graudal itemsets 
        3.2.3 Closed periodic patterns 
4 Creating your own instance of ParaMiner 
    4.1 A selection criterion 
    4.2 A closure operator 
    4.3 A main function 
        4.3.1 Parsing the command line arguments 
        4.3.2 Loading the dataset 
        4.3.3 Invoking the search space 
5 Bugs 


1 Quick start 
~~~~~~~~~~~~~~

1. Download the complete archive ParaMiner+Melinda here: [http://membres-liglab.imag.fr/negrevergne/paraminer/paraminermelinda.tgz]

2. Extract with 
    tar xzvf paraminermelinda.tgz
    and move into the directory with 
    cd paraminermelinda

3. Compile Melinda
  cd melinda
  cmake . 
  make
  cd ../


4. Compile ParaMiner
  cd paraminer
  cmake . 
  make


5. Run the closed frequent itemset mining (FIM) instance of ParaMiner on the test.dat dataset an absolute frequency threshold of 2 (and 4 threads).
  ./clogen_itemsets test.dat 2 -t 4


It will output the closed frequent itemset occurring in test.dat with an absolute frequency threshold of at least 2. 

2 Compilation 
~~~~~~~~~~~~~~

ParaMiner is a core algorithm, and it uses the Melinda library as a
parallelism engine.

In order to build paraminer you need the Melinda Library which is available at 
[http://membres-liglab.imag.fr/negrevergne/software/melinda]

Edit the src/CMakeLists.txt and change the MELINDA\_DIR file to the correct melinda location. 

  cmake . 
  make


This will create several a binary (executable) files, one for each
clogen\_<problem\_name> file for each clogen\_local\_<problem\_name> in the
directory.

For example clogen\_itemsets is built. See section [instance fim].

2.1 Switching build modes 
--------------------------

You can compile ParaMiner in debug mode (useful if you want to implement your own pattern problem). 
  cmake -DCMAKE_BUILD_TYPE=Debug
  make


To set the build system in debug mode

Use:
  cmake -DCMAKE_BUILD_TYPE=Release
  make

to switch back de Release mode.

3 Running an instance of ParaMiner 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to solve a pattern mining problem, you need the adequate instance of
ParaMiner.  You can see how to use the built-in
instance of ParaMiner to mine closed frequent itemset in [instance fim].
Or you can learn how to create your own instance of ParaMiner in
[creating your own instance].

3.1 Closed frequent itemset mining (FIM) with ParaMiner 
========================================================

If you correctly compiled ParaMiner, you should have a clogen\_itemset
binary file in the src/ directory.

You can use this instance of ParaMiner to mine closed frequent itemset
from a transactional dataset.

3.1.1 Input file format 
------------------------
    
Dataset stored within a single ASCII file.

- Each line is a transaction;
- each transaction contains distincts items;
- transactions must be ordered;
- last line must be empty.

eg.
  --- file test.dat ---
  1 2 4 6
  1 2 3 5 7
  2 3


is a valid dataset.

3.1.2 Running the FIM instance of ParaMiner 
--------------------------------------------

You can mine closed frequent itemsets occurring in this dataset by executing the following command:
./clogen_itemsets test.dat 2 

Alternatively, if you have a multi-core/parallel computer with 8
cores, you can exploit them by executing the following command: 
./clogen_itemsets test.dat 2 -t 8

3.1.3 Output format 
--------------------

The FIM implementation of ParaMiner has the following format: 
- each line is a frequent closed itemset;
- frequency is stored at the end of the line into brackets.

eg.
./clogen_itemsets test.dat 2 
will generate the following output on the standard output:

  2 (3)
  1 2 (2)
  3 2 (2)


The results can be stored by redirecting the standard output into a file:
./clogen_itemsets test.dat 2 -t 1 > results.out

3.2 Other built-in instances of ParaMiner 
==========================================

3.2.1 Closed frequent relational graphs 
----------------------------------------

3.2.2 Closed graudal itemsets 
------------------------------

3.2.3 Closed periodic patterns 
-------------------------------
 

4 Creating your own instance of ParaMiner 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you want to create your own instance of ParaMiner to mine your own
type of patterns (say dark patters), you to write a clogen_local_dark.cpp file which will contain an implementation for the following functions:

4.1 A selection criterion 
==========================
In a function called membership\_oracle(). 
The selection criterion to distingish candidate patterns from patterns.

It takes as an argument a closed pattern P and a possible augmentation
element e.  It must return a non-null value if and only if the
candidate pattern P U {e} is a pattern.

For example for our closed dark pattern mining problem, it can be as
simple as:

  bool membership_oracle(P, e){
    return is_a_dark_pattern(P U {e}); 
  }


4.2 A closure operator 
=======================
In a function called clo()

The closure operator, used to limit the
redundancy in the resulting patterns. Takes a pattern as an argument,
and returns a pattern.  This function takes a pattern as a parameter
and return its corresponding closed pattern (ie. its closure).  If you
don't need such a thing, the identity is a perfectly valid closure
operator. However, it does not reduce the redundancy among the set of
resulting patterns.

So far, we don't have any satisfying definition of closed dark pattern
therefore we are only going to use the identity.

  clo(P){
    return P;
  }


4.3 A main function 
====================

The main function is here to achieves three goals:
1. Parse the command line arguments
2. Load and pre-process the dataset 
3. Invoque the clogen() routine to start the exploration. 

4.3.1 Parsing the command line arguments 
-----------------------------------------
    
You must start your main function by calling the
parse_clogen_arguments(argc, argv) function.  It will capture the
arguments used by ParaMiner remove them from argv and decrease argc.

4.3.2 Loading the dataset 
--------------------------

The dataset must be loaded into a table called tt which is of type TransactionTable. 

If your dataset is stored as described in [fim input file format], you
can use the built-in function read_transaction_table() It takes two
argument, the filename and the transaction table.

So far our clogen_local_dark.cpp file looks like this:

  int main(int argc, char **argv){
  
  load_transaction_table (&tt, argv[1])
  
  ...
  
  }


4.3.3 Invoking the search space 
--------------------------------

Once your dataset is loaded into tt, you must call the clogen() main routine with empty_set
as an argument if you want to start the exploration from the emptyset.

5 Bugs 
~~~~~~~

Repport bugs and/or comments at:
FirstName.LastName@imag.fr

My FirstName is Benjamin
My LastName is Negrevergne
