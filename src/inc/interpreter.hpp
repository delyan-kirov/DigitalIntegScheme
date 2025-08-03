/*-----------------------------MODULE INFO-----------------------------/
 * \file interpreter.cpp
 * \author Delyan Kirov
 * \brief Interface for the interpreter
 *---------------------------------------------------------------------*/

#ifndef INTERPRETER_H
#define INTERPRETER_H

/*----------------------------------------------------------------------/
 *--------------------------MODULE INCLUDES-----------------------------/
 *---------------------------------------------------------------------*/
#include "parser.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <optional>

/*----------------------------------------------------------------------/
 *---------------------------MODULE TYPES-------------------------------/
 *---------------------------------------------------------------------*/

//! \brief Define the FunctionDefinition struct
struct FunctionDefinition
{
  std::string name;
  std::vector<std::string> argNames;
  SynTree *definition;
};

//! \brief Declare the global program namespace
extern std::vector<FunctionDefinition> programNameSpace;
//! \brief Function to interpret commands
extern void interpreter (Command command);

#endif // INTERPRETER_H

/*----------------------------------------------------------------------/
 *-----------------------------------EOF--------------------------------/
 *---------------------------------------------------------------------*/