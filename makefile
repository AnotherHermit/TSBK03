###########################################################
###														###
###				          MAKEFILE			  			###
###											  			###
### 		         Project in TSBK03	      			###
###			   by Conrad Wahlen (conwa099)	  			###
###											  			###
###########################################################

CXX=g++
RM=rm -f

CPPFLAGS = -Wall -pedantic -std=c++0x
FLAGS= -lm -DGL_GLEXT_PROTOTYPES

LFLAG= -lGL
WFLAG= -lfreeglut -lglew32 -lopengl32 -L$(COM)/Windows/lib

# Utilities
COM=common
LINC= -I$(COM) -I$(COM)/Linux
WINC= -I$(COM) -I$(COM)/Windows

UTILS=$(COM)/*.c
LUTILS=$(COM)/Linux/MicroGlut.c $(UTILS)

# Input files
LAB0=Lab0/lab0.cc
LAB11=Lab1-1/lab1-1.cc
LAB12=Lab1-2/lab1-2.cc
LAB2=Lab2/skinning.c

#Output location
BIN=bin

all: wlab2

lab0: $(LAB0)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab0 $(LAB0) $(LUTILS) $(FLAGS) $(LFLAG) $(LINC)

wlab0: $(LAB0)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab0 $(LAB0) $(UTILS) $(FLAGS) $(WFLAG) $(WINC)

lab11: $(LAB11)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab11 $(LAB11) $(LUTILS) $(FLAGS) $(LFLAG) $(LINC)

wlab11: $(LAB11)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab11 $(LAB11) $(UTILS) $(FLAGS) $(WFLAG) $(WINC)
	
lab12: $(LAB11)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab12 $(LAB12) $(LUTILS) $(FLAGS) $(LFLAG) $(LINC)

wlab12: $(LAB12)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab12 $(LAB12) $(UTILS) $(FLAGS) $(WFLAG) $(WINC)
	
lab2: $(LAB2)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab2 $(LAB2) $(LUTILS) $(FLAGS) $(LFLAG) $(LINC)

wlab2: $(LAB2)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab2 $(LAB2) $(UTILS) $(FLAGS) $(WFLAG) $(WINC)
	
clean :
	@ $(RM) $(BIN)/lab*

