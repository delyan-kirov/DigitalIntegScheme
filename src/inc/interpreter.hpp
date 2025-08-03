#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <optional>

// Define the FunctionDefinition struct
struct FunctionDefinition
{
  std::string name;
  std::vector<std::string> argNames;
  SynTree *definition;
};

// Declare the global program namespace
extern std::vector<FunctionDefinition> programNameSpace;
// Function to interpret commands
extern void interpreter (Command command);

#endif // INTERPRETER_H
