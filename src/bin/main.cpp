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

  std::vector<Token> *tokens;
  try
    {
      tokens = tokenizer (infile);
    }
  catch (...)
    {
      std::cerr << "TOKENIZER ERROR: incorrect syntax\n";
      exit (2);
    }

  try
    {
      auto command = parser (0, tokens);
      interpreter (command.second);

      for (;;)
        {
          command = parser (command.first, tokens);
          if (command.second.type == CommandType::TRIVIAL) continue;
          interpreter (command.second);
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