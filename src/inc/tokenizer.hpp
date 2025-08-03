/*-----------------------------MODULE INFO-----------------------------/
 * \file tokenizer.hpp
 * \author Delyan Kirov
 * \brief Interface for the tokenizer
 *---------------------------------------------------------------------*/

#ifndef TOKENIZER_H
#define TOKENIZER_H

/*----------------------------------------------------------------------/
 *--------------------------MODULE INCLUDES-----------------------------/
 *---------------------------------------------------------------------*/
#include <iostream>
#include <vector>

/*----------------------------------------------------------------------/
 *---------------------------MODULE TYPES-------------------------------/
 *---------------------------------------------------------------------*/

//! \brief Enum for token types
enum class TokenType
{
  PAREN_L,
  PAREN_R,
  DEFINE,
  RUN,
  ALL,
  FIND,
  CLEAR,
  VAR_NAME,
  VAL,
  NEWLINE,
  COMMA,
  COLS,
  SEMICOLS,
  QMARK,
  AND,
  OR,
  NOT,
};

namespace std
{
inline std::string
to_string (const TokenType &type)
{
  switch (type)
    {
    case TokenType::PAREN_L : return "PAREN_L";
    case TokenType::PAREN_R : return "PAREN_R";
    case TokenType::DEFINE  : return "DEFINE";
    case TokenType::RUN     : return "RUN";
    case TokenType::FIND    : return "FIND";
    case TokenType::CLEAR   : return "CLEAR";
    case TokenType::ALL     : return "ALL";
    case TokenType::VAR_NAME: return "VAR_NAME";
    case TokenType::VAL     : return "VAL";
    case TokenType::NEWLINE : return "NEWLINE";
    case TokenType::COMMA   : return "COMMA";
    case TokenType::COLS    : return "COLS";
    case TokenType::SEMICOLS: return "SEMICOLS";
    case TokenType::QMARK   : return "QMARK";
    case TokenType::AND     : return "AND";
    case TokenType::OR      : return "OR";
    case TokenType::NOT     : return "NOT";
    default                 : return "UNKNOWN";
    }
}
}

//! \brief Struct for tokens
struct Token
{
  TokenType type;
  unsigned char val;
  std::string name;
};

namespace std
{
inline string
to_string (const Token &token)
{
  string result = "TOKEN: " + std::to_string (token.type) + " " + token.name;
  int token_value = static_cast<int> (token.val);
  if (token_value != 2)
    {
      result += " " + std::to_string (token_value);
    }
  return result;
}
} // namespace std

/*----------------------------------------------------------------------/
 *--------------------------MODULE FUNCTIONS----------------------------/
 *---------------------------------------------------------------------*/

//! \brief print tokens
extern void printTokens (const std::vector<Token> &tokens);

//! \brief Function to tokenize the input file
extern std::vector<Token> *tokenizer (FILE *file);

#endif // TOKENIZER_H

/*----------------------------------------------------------------------/
 *-----------------------------------EOF--------------------------------/
 *---------------------------------------------------------------------*/