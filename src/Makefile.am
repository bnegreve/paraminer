AM_CPPFLAGS = -I$(srcdir)/../melinda/
AM_CFLAGS = -std=c99
AM_LDFLAGS = 

if DEBUG
AM_CPPFLAGS +=  
else 
AM_CPPFLAGS += -DNDEBUG
endif 

if PROF
AM_CPPFLAGS += -pg --coverage
AM_LDFLAGS += -pg --coverage
endif 

COMMON = clogen.cpp element.cpp database.cpp pattern.cpp utils.cpp	\
	  clogen.hpp clogen_local.hpp database.hpp element.hpp		\
	  itemset_specific.hpp melinda_local.hpp pattern.hpp		\
	  utils.hpp

bin_PROGRAMS = \
	paraminer_itemsets \
	paraminer_cgraphs \
	paraminer_graduals 

LDADD = ../melinda/libmelinda.a -lpthread

AM_DEFAULT_SOURCE_EXT = .cpp

paraminer_itemsets_SOURCES = clogen_local_itemsets.cpp $(COMMON)
paraminer_cgraphs_SOURCES = clogen_local_cgraphs.cpp $(COMMON)
paraminer_graduals_SOURCES = clogen_local_graduals.cpp $(COMMON)

