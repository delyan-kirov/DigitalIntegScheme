#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.hpp"

// Forward declarations
struct Token;
struct SynTree;
struct Table;
struct Command;

// Enum for operation types
enum class OperationType
{
  AND,
  OR,
  NOT,
};

// Enum for algebraic types
enum class AlgebraType
{
  VALUE,
  OPERATION,
  VARIABLE,
  NONE // Default state
};

// Structure for algebraic expressions
struct Algebra
{
  AlgebraType type;
  unsigned char value;
  OperationType operation;
  std::string variable;
};

// Structure for commands

enum class CommandType
{
  DEFINE,
  RUN,
  ALL,
  FIND,
  CLEAR,
  TRIVIAL,
  EXIT,
};

// Structure for table representation
struct Table
{
  size_t N;
  size_t M;
  std::vector<unsigned char> input;
  std::vector<unsigned char> output;
};

struct Command
{
  SynTree *definition = nullptr;
  CommandType type = CommandType::TRIVIAL;
  std::vector<std::string> arguments;
  std::vector<unsigned char> values;
  Table table;
  std::string name = nullptr;
};

// Structure for syntax tree nodes
struct SynTree
{
  Algebra val;
  SynTree *left;
  SynTree *right;

  SynTree (const Algebra &value, SynTree *leftNode = nullptr,
           SynTree *rightNode = nullptr);

  void
  destroy ()
  {
    // Recursively delete left and right subtrees
    if (this->left != nullptr)
      {
        delete this->left;
        left = nullptr; // Optional: Set to nullptr after deletion
      }
    if (this->right != nullptr)
      {
        delete this->right;
        right = nullptr; // Optional: Set to nullptr after deletion
      }
  }
};

extern std::pair<size_t, Command> parser (size_t idx, std::vector<Token> *tokens);

#endif // PARSER_H
