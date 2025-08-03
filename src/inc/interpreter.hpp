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

namespace Interpreter
{
/*----------------------------------------------------------------------/
 *---------------------------MODULE TYPES-------------------------------/
 *---------------------------------------------------------------------*/

//! \brief Define the FunctionDefinition struct
struct Func
{
  std::string name;
  std::vector<std::string> argNames;
  Parser::SynTree *definition;
};

/*----------------------------------------------------------------------/
 *---------------------------MODULE FUNCTIONS---------------------------/
 *---------------------------------------------------------------------*/

//! \brief Function that interprets parser commands
extern void interpret (Parser::Command command);
}

/*----------------------------------------------------------------------/
 *------------------------------FOREIGN DATA----------------------------/
 *---------------------------------------------------------------------*/

//! \brief Declare the global program namespace
extern std::vector<Interpreter::Func> programNameSpace;

#endif // INTERPRETER_H

/*----------------------------------------------------------------------/
 *-----------------------------------EOF--------------------------------/
 *---------------------------------------------------------------------*/