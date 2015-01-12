
SOURCE_MAIN = Behaviors.cpp main.cpp

INCLUDES = $(SOURCES_MAIN:.cpp=.h)

CC = g++ 
LINK = g++

CFLAGS =  -fPIC  -Wall -O3 $(shell pkg-config --cflags playerc++)
#CFLAGS =  -fPIC  -O3 $(shell pkg-config --cflags playerc++)
CFLAGS += $(shell pkg-config --cflags playercore)
CFLAGS += -I.


LIBS   = $(shell pkg-config --libs   playerc++)
LIBS   += $(shell pkg-config --libs  playerinterface)
LIBS   += $(shell pkg-config --libs  playercore)


INCLUDE_DIR =  -I.

OBJECT_PATH = objects
vpath .. %.o $(OBJECT_PATH)

OBJECTS_MAIN = $(patsubst %.cpp,objects/%.o,$(SOURCE_MAIN))

TARGET =   robot

$(TARGET): $(OBJECTS_MAIN)
	$(LINK) $(LDFLAGS)  -o $(TARGET) $(LIBS)  $(OBJECTS_MAIN)

all: $(TARGET) 

.SUFFIXES: .h .cpp 

objects/%.o:  %.cpp
	$(CC)  $(INCLUDE_DIR)  $(CFLAGS) -g -c $(<) -o $@

clean:
	rm -f objects/*.o $(TARGET)

backup:
	ci -l *.cpp *.h makefile


depend:	
	makedepend -I. -pobjects/ -fmakefile $(INCLUDES) $(SOURCE_MAIN)


# DO NOT DELETE

objects/Behaviors.o: Behaviors.h
objects/main.o: Behaviors.h
