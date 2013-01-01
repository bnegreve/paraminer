AM_CPPFLAGS = -I../melinda/
COMMON = clogen.cpp element.cpp database.cpp pattern.cpp utils.cpp

bin_PROGRAMS = \
	paraminer_itemsets \
	paraminer_cgraphs \
	paraminer_graduals 

LDADD = ../melinda/libmelinda.a -lpthread

AM_DEFAULT_SOURCE_EXT = .cpp

paraminer_itemsets_SOURCES = clogen_local_itemsets.cpp $(COMMON)
paraminer_cgraphs_SOURCES = clogen_local_cgraphs.cpp $(COMMON)
paraminer_graduals_SOURCES = clogen_local_graduals.cpp BinaryMatrix.cpp \
	 CException.cpp TransactionSequence.cpp $(COMMON) 
