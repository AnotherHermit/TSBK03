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
COMFLAGS = -lm -DGL_GLEXT_PROTOTYPES

COM=common
COMUTILS = $(COM)/*.c
COMINC = -I$(COM) 

ifeq ($(OS),Windows_NT)
INC= $(COMINC) -I$(COM)/Windows
FLAGS= $(COMFLAGS) -lfreeglut -lglew32 -lopengl32 -L$(COM)/Windows/lib $(INC)
UTILS= $(COMUTILS)
else
INC= $(COMINC) -I$(COM)/Linux
FLAGS= $(COMFLAGS) -lGL
UTILS= $(COMUTILS) $(COM)/Linux/MicroGlut.c
endif

# Input files
LAB0=Lab0/lab0.cc
LAB11=Lab1-1/lab1-1.cc
LAB12=Lab1-2/lab1-2.cc
LAB2=Lab2/skinning.c

OBJECTS = *.o

#Output location
BIN=bin


all: alab2

alab0: $(LAB0) $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab0 $(LAB0) $(OBJECTS) $(FLAGS)

alab11: $(LAB11) $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab11 $(LAB11) $(OBJECTS) $(FLAGS)
	
alab12: $(LAB11) $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab12 $(LAB12) $(OBJECTS) $(FLAGS)

alab2: $(LAB2) $(OBJECTS)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab2 $(LAB2) $(OBJECTS) $(FLAGS)

$(OBJECTS):
	$(CXX) $(CPPFLAGS) -c $(UTILS) $(FLAGS)

clean :
	@ $(RM) $(BIN)/lab*
	@ $(RM) $(OBJECTS)

