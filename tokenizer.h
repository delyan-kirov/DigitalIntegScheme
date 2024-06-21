#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <vector>

// Enum for token types
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

// Struct for tokens
struct Token
{
  TokenType type;
  unsigned char val;
  std::string name;
};

// Function to print tokens
std::string
printTokenType(const TokenType& type);

void
printTokens(const std::vector<Token>& tokens);

// Function to tokenize the input file
std::vector<Token>*
tokenizer(FILE* file);

#endif // TOKENIZER_H
