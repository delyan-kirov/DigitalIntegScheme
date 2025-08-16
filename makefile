#-----------------------------MODULE INFO-----------------------------/
# \file makefile
# \author Delyan Kirov
# \brief Make file that build executable main.exe
#---------------------------------------------------------------------*/

#---------------------------------------------------------------------*/
#-----------------------------CONFIG FLAGS-----------------------------/
#---------------------------------------------------------------------*/
CXX = g++
CXXFLAGS = -std=c++20 -g -Wall -Wextra -Wpedantic


#---------------------------------------------------------------------*/
#-----------------------------SOURCE FILES-----------------------------/
#---------------------------------------------------------------------*/

TARGET = main.exe
SRC_DIR = ./src/bin/
INC_DIR = ./src/inc/
BLD_DIR = ./bld/

all: build test

#---------------------------------------------------------------------*/
#----------------------------------TARGETS-----------------------------/
#---------------------------------------------------------------------*/

$(TARGET): $(BLD_DIR)main.o $(BLD_DIR)interpreter.o $(BLD_DIR)parser.o $(BLD_DIR)tokenizer.o $(BLD_DIR)
	$(CXX) $(CXXFLAGS) $(BLD_DIR)main.o $(BLD_DIR)interpreter.o $(BLD_DIR)parser.o $(BLD_DIR)tokenizer.o -o $(TARGET)

$(BLD_DIR)main.o: $(SRC_DIR)main.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I$(BLD_DIR) -c $(SRC_DIR)main.cpp -o $(BLD_DIR)main.o

$(BLD_DIR)interpreter.o: $(SRC_DIR)interpreter.cpp $(INC_DIR)interpreter.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I$(BLD_DIR) -c $(SRC_DIR)interpreter.cpp -o $(BLD_DIR)interpreter.o

$(BLD_DIR)parser.o: $(SRC_DIR)parser.cpp $(INC_DIR)parser.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I$(BLD_DIR) -c $(SRC_DIR)parser.cpp -o $(BLD_DIR)parser.o

$(BLD_DIR)tokenizer.o: $(SRC_DIR)tokenizer.cpp $(INC_DIR)tokenizer.hpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -I$(BLD_DIR) -c $(SRC_DIR)tokenizer.cpp -o $(BLD_DIR)tokenizer.o

# TODO: write the tests in a python script
TST_DIR = ./src/tst/
tst.SRC = ic1.txt \
	      ic3.txt \
	      ic2.txt \
	      findWithFile.txt \
	      find.txt
tst.SRC.DEP = $(addprefix $(TST_DIR), $(tst.SRC))
test: $(TARGET)
	$(foreach test_case, $(tst.SRC.DEP), ./$(TARGET) $(test_case)) 
	@echo "INFO: All tests passed"

$(BLD_DIR):
	mkdir $(BLD_DIR)

.PHONY: all build test clean bear 

build: $(BLD_DIR) $(TARGET)

bear:
	bear -- make clean all

clean:
	rm -f $(TARGET)
	rm -rf $(BLD_DIR)
	rm -f compile_commands.json
