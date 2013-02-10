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

COMMON = paraminer.cpp element.cpp database.cpp pattern.cpp utils.cpp	\
	 paraminer.hpp paraminer_local.hpp database.hpp element.hpp		\
	 melinda_local.hpp pattern.hpp utils.hpp 

bin_PROGRAMS = \
	paraminer_itemsets \
	paraminer_cgraphs \
	paraminer_graduals 

LDADD = ../melinda/libmelinda.a -lpthread

AM_DEFAULT_SOURCE_EXT = .cpp

paraminer_itemsets_SOURCES = paraminer_local_itemsets.cpp $(COMMON)
paraminer_cgraphs_SOURCES = paraminer_local_cgraphs.cpp $(COMMON)
paraminer_graduals_SOURCES = paraminer_local_graduals.cpp bool_matrix.hpp $(COMMON)

