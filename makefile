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

main.SRC = main.cpp \
		   interpreter.cpp \
		   parser.cpp \
		   tokenizer.cpp
main.INC = interpreter.hpp \
		   parser.hpp \
		   tokenizer.hpp

all: build test

#---------------------------------------------------------------------*/
#----------------------------------TARGETS-----------------------------/
#---------------------------------------------------------------------*/

main.SRC.DEP = $(addprefix $(SRC_DIR), $(main.SRC))
main.INC.DEP = $(addprefix $(INC_DIR), $(main.INC))
$(TARGET): $(main.SRC.DEP) $(main.INC.DEP)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) $(main.SRC.DEP) -o $(TARGET)

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

.PHONY: all build test clean

build: $(TARGET)

clean:
	rm $(TARGET)
