CFLAGS = -std=c++17 -Wall -O2 -s -I./include

BIN := bin
INC := include
SRC := src
OBJ := obj

ARM := arm
X86 := x86

SOURCES = $(shell find $(SRC) -name "*.cpp")
OBJECTS_X86 = $(patsubst $(SRC)/%.cpp, $(OBJ)/$(X86)/%.o, $(SOURCES))
OBJECTS_ARM = $(patsubst $(SRC)/%.cpp, $(OBJ)/$(ARM)/%.o, $(SOURCES))

x86: $(OBJECTS_X86)
	ar rcs $(BIN)/$(X86)/libStar.a $(OBJECTS)

arm:
	echo Not implemented!

clean:
	rm -f $(OBJ)/$(ARM)/*
	rm -f $(OBJ)/$(X86)/*
	rm -f $(BIN)/$(ARM)/*
	rm -f $(BIN)/$(X86)/*

$(OBJ)/$(X86)/%.o : $(SRC)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJ)/$(ARM)/%.o : $(SRC)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@