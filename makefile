#-----------------------------MODULE INFO-----------------------------/
# \file makefile
# \author Delyan Kirov
# \brief Make file that builds executable main.exe
#---------------------------------------------------------------------*/

#-----------------------------CONFIG FLAGS-----------------------------/
CXX = g++
CXXFLAGS = -std=c++20 -g -Wall -Wextra -Wpedantic
#---------------------------------------------------------------------*/

#-----------------------------SOURCE FILES-----------------------------/
SRC_DIR = ./src/bin/
INC_DIR = ./src/inc/
BLD_DIR = ./bld/

all: build test
#---------------------------------------------------------------------*/

#---------------------------------TARGETS------------------------------/
TARGETS := main.exe
main.exe_SRCS := main.cpp \
				 interpreter.cpp \
				 parser.cpp \
				 tokenizer.cpp

# Pattern rules for objects and dependencies
define MAKE_TARGET_RULES
$1_OBJS := $$($1_SRCS:%.cpp=$(BLD_DIR)%.o)
$1_DEPS := $$($1_OBJS:.o=.d)

$1: $$($1_OBJS)
	$$(CXX) $$(CXXFLAGS) $$^ -o $$@

$(BLD_DIR)%.o: $(SRC_DIR)%.cpp
	$$(CXX) $$(CXXFLAGS) -I$$(INC_DIR) -I$$(BLD_DIR) -MMD -c $$< -o $$@

-include $$($1_DEPS)
endef

$(foreach tgt,$(TARGETS),$(eval $(call MAKE_TARGET_RULES,$(tgt))))
#---------------------------------------------------------------------*/

#--------------------------------TESTS---------------------------------/
TST_DIR = ./src/tst/
tst.SRC = ic1.txt ic3.txt ic2.txt findWithFile.txt find.txt
tst.SRC.DEP = $(addprefix $(TST_DIR), $(tst.SRC))

test: $(TARGETS)
	@for test_case in $(tst.SRC.DEP); do \
		./$(TARGETS) $$test_case; \
	done
	@echo "INFO: All tests passed"
#---------------------------------------------------------------------*/

#-----------------------------BUILD DIRECTORY--------------------------/
$(BLD_DIR):
	mkdir -p $(BLD_DIR)

build: $(BLD_DIR) $(TARGETS)
#---------------------------------------------------------------------*/

#-----------------------------PHONY TARGETS----------------------------/
.PHONY: all build test clean bear 

bear:
	bear -- make clean all

clean:
	rm -f $(TARGETS)
	rm -rf $(BLD_DIR)
	rm -f compile_commands.json
#---------------------------------------------------------------------*/
