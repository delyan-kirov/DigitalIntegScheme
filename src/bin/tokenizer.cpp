/*-----------------------------MODULE INFO-----------------------------/
 * \file tokenizer.cpp
 * \author Delyan Kirov
 * \brief Interface for the tokenizer
 *---------------------------------------------------------------------*/

/*----------------------------------------------------------------------/
 *--------------------------MODULE INCLUDES-----------------------------/
 *---------------------------------------------------------------------*/

#include "tokenizer.hpp"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <vector>

/*----------------------------------------------------------------------/
 *------------------------------MODULE EXPR-----------------------------/
 *---------------------------------------------------------------------*/

namespace Tokenizer
{
extern void
printTokens (const std::vector<Token> &tokens)
{
  for (size_t i = 0; i < tokens.size (); ++i)
    {
      Token token = tokens[i];
      std::cout << std::to_string (token);
      if (i != tokens.size () - 1) std::cout << '\n';
    }
}

//! \brief Function to tokenize the input file
//! \todo Handle comments
extern std::vector<Token> *
tokenize (FILE *file)
{
  auto tokens = new std::vector<Token>;
  Token newToken;
  std::string tokenName;

  int c;
  while ((c = fgetc (file)) != EOF)
    {
      // std::cout << (char)c;
      if (isalnum (c) || c == '.')
        {
          tokenName += (char)c;
        }
      else if (c == '&')
        {
          tokens->push_back ({ TokenType::AND, 2, "" });
        }
      else if (c == '|')
        {
          tokens->push_back ({ TokenType::OR, 2, "" });
        }
      else if (c == '!')
        {
          tokens->push_back ({ TokenType::NOT, 2, "" });
        }
      else if (c == ' ' || c == '(' || c == ')' || c == ',' || c == '"'
               || c == ':' || c == ';' || c == '\n')
        {
          if (tokenName == "DEFINE")
            {
              tokens->push_back ({ TokenType::DEFINE, 2, "" });
            }
          else if (tokenName == "RUN")
            {
              tokens->push_back ({ TokenType::RUN, 2, "" });
            }
          else if (tokenName == "FIND")
            {
              tokens->push_back ({ TokenType::FIND, 2, "" });
            }
          else if (tokenName == "CLEAR")
            {
              tokens->push_back ({ TokenType::CLEAR, 2, "" });
            }
          else if (tokenName == "ALL")
            {
              tokens->push_back ({ TokenType::ALL, 2, "" });
            }
          else if (tokenName != "" && tokenName != "1" && tokenName != "0")
            {
              tokens->push_back ({ TokenType::VAR_NAME, 2, tokenName });
            }
          else if (tokenName == "1")
            {
              tokens->push_back ({ TokenType::VAL, 1, "" });
            }
          else if (tokenName == "0")
            {
              tokens->push_back ({ TokenType::VAL, 0, "" });
            }
          tokenName = "";
        }
      else
        {
          std::cerr << "ERROR: Unrecognized token: " << (char)c << '\n';
          exit (1);
        }
      if (c == '(')
        {
          tokens->push_back ({ TokenType::PAREN_L, 2, "" });
        }
      else if (c == ')')
        {
          tokens->push_back ({ TokenType::PAREN_R, 2, "" });
        }
      else if (c == ':')
        {
          tokens->push_back ({ TokenType::COLS, 2, "" });
        }
      else if (c == ';')
        {
          tokens->push_back ({ TokenType::SEMICOLS, 2, "" });
        }
      else if (c == '"')
        {
          tokens->push_back ({ TokenType::QMARK, 2, "" });
        }
      else if (c == ',')
        {
          tokens->push_back ({ TokenType::COMMA, 2, "" });
        }
      else if (c == '\n')
        {
          tokens->push_back ({ TokenType::NEWLINE, 2, "" });
        }
    }
  return tokens;
}
} // end namespace Tokenizer

/*----------------------------------------------------------------------/
 *-----------------------------------EOF--------------------------------/
 *---------------------------------------------------------------------*/