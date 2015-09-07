CXX=g++
RM=rm -f

CPPFLAGS = -Wall -pedantic -std=c++0x
FLAGS= -lm -DGL_GLEXT_PROTOTYPES

LFLAG= -lGL
WFLAG= -lfreeglut -lglew32 -lopengl32  -L$(COM)/Windows/lib
#-lgdi32

COM=../common
LINC= -I$(COM) -I$(COM)/Linux
WINC= -I$(COM) -I$(COM)/Windows

OBJS=lab0.c

UTILS=$(COM)/GL_utilities.c $(COM)/VectorUtils3.c $(COM)/LoadTGA.c $(COM)/loadobj.c 
LUTILS=$(COM)/Linux/MicroGlut.c $(UTILS)
WUTILS=$(UTILS)

#$(COM)/Windows/MicroGlut.c 

BIN=../bin

all: wlab0

lab0: $(OBJS)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab0 $(OBJS) $(LUTILS) $(LFLAG) $(FLAGS) $(LINC)

wlab0: $(OBJS)
	$(CXX) $(CPPFLAGS) -o $(BIN)/lab0 $(OBJS) $(WUTILS) $(WFLAG) $(FLAGS) $(WINC)

clean :
	@ $(RM) $(BIN)/lab0

