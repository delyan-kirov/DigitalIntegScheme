/*-------------------------------EXE INFO------------------------------/
 * \file main.cpp
 * \author Delyan Kirov
 * \executable main.exe
 * \extends parser, tokenizer, interpreter
 *---------------------------------------------------------------------*/

/*----------------------------------------------------------------------/
 *-----------------------------EXE INCLUDES------------------------------/
 *----------------------------------------------------------------------*/
#include "interpreter.hpp"
#include "parser.hpp"
#include <cstdlib>
#include <iostream>
#include <utility>

/*----------------------------------------------------------------------/
 *---------------------------------MAIN---------------------------------/
 *---------------------------------------------------------------------*/

//! \brief Main function for main.exe
int
main (int argc, char *argv[])
{
  FILE *infile;

  if (argc == 1)
    {
      infile = fopen ("/dev/stdin", "r"); // Open stdin for reading
    }
  else
    {
      char *fileName = argv[1];
      infile = fopen (fileName, "r");
    }

  if (infile == nullptr)
    {
      std::cerr << "ERROR: Could not open file\n";
      return 1;
    }

  std::vector<Tokenizer::Token> *tokens;
  try
    {
      tokens = Tokenizer::tokenize (infile);
    }
  catch (...)
    {
      std::cerr << "TOKENIZER ERROR: incorrect syntax\n";
      exit (2);
    }

  try
    {
      auto command = Parser::parse (0, tokens);
      Interpreter::interpret (command.second);

      for (;;)
        {
          command = Parser::parse (command.first, tokens);
          if (command.second.type == Parser::CommandType::TRIVIAL) continue;
          Interpreter::interpret (command.second);
        }
    }
  catch (...)
    {
      std::cerr << "PARSER ERROR: incorrect syntax\n";
      exit (2);
    }
  return 0;
}

/*----------------------------------------------------------------------/
 *-----------------------------------EOF--------------------------------/
 *---------------------------------------------------------------------*/