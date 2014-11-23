CC   = g++
SRC  = $(wildcard *.cpp) $(wildcard src/*/*.cpp)
OBJ  = $(addprefix obj/,$(notdir $(SRC:.cpp=.o)))  
DEPS = $(SRC:%.cpp=obj/%.d)

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
CFLAGS  = -pedantic -ansi -std=c++11 -Wall -fPIC -g -I/usr/local/include

EXE = sbm

UNAME := $(shell uname)

.PHONY: all clean run

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

obj/%.o: %.cpp | obj
	$(CC) -MMD -MP -c $< $(CFLAGS) -o $@ 

obj:
	mkdir obj

run:
	./$(EXE)

clean:
	rm $(OBJ) $(DEPS)

-include $(DEPS)
