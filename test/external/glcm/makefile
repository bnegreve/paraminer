## Makefile
## Made by Alexandre Termier
## Login   <alexandre@belem>
## Started on  yyyy/mm/dd : 2004/04/09 Alexandre Termier
## Last update Thu Jul 17 15:05:13 2008 Alexandre Termier

#
# Makefile for PLCM
#
#

#CC = g++ -m32
#CC = icc -O1
CC = g++  
#CC = g++ -g

#Compilation parameters

#CFLAGS= -g -I/usr/local/include -DTRACE 
CFLAGS += -O3 -I/usr/local/include
#Link parameters
LDFLAGS += -L/usr/local/lib -lpthread

#Default target
all:linux


#All arch independant source files
SRCS= BinaryMatrix.cpp CException.cpp DBMain.cpp ItemSet.cpp TransactionSequence.cpp GLCM.cpp 

#All arch independant object files
OBJS=$(addsuffix .o,$(basename $(SRCS))) 

EXE_NAME = glcm


.PHONY :  dep tags


#How to build PLCM objets files
%.o:%.cpp depends
	$(CC) -c $(CFLAGS) -o $@ $<




linux: $(OBJS)  main.o
	$(CC) $(LDFLAGS) $(OBJS) main.o -o $(EXE_NAME)


#How to construct de dep file which contain all de dependences (included at the very end of this Makefile)
dep: 
	$(CC)  $(CFLAGS) -MM $(SRCS) > depends

clean:
	rm -f *.o ; rm -f *.o ; rm -f *~ ; rm -f *~ ; rm -f $(EXE_NAME) depends

tags:
	etags *.cpp *.hpp *.h */*.cpp */*.hpp */*.h
-include depends
