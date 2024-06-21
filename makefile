CXX = g++
CXXFLAGS = -g
TARGET = main
SRCS = main.cpp tokenizer.cpp parser.cpp interpreter.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all build test clean

# Default target
all: build clean test

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test:
	@./main ./examples/ic1.txt > /dev/null 2>&1
	@./main ./examples/ic3.txt > /dev/null 2>&1
	@./main ./examples/ic2.txt > /dev/null 2>&1
	@./main ./examples/findWithFile.txt > /dev/null 2>&1
	@./main ./examples/find.txt > /dev/null 2>&1
	@echo "INFO: All tests passed"

clean:
	rm -f $(OBJS)
