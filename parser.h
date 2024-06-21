#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include <string>
#include <vector>

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
  SynTree* definition = nullptr;
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
  SynTree* left;
  SynTree* right;

  SynTree(const Algebra& value,
          SynTree* leftNode = nullptr,
          SynTree* rightNode = nullptr);
};

// Function declarations
size_t
powerOf(size_t a, size_t b);

OperationType
tokenToOpType(const TokenType& tokenType);

std::string
printAlgebraType(const Algebra& val);

void
printSyntaxTree(const SynTree* node, int depth = 0);

void
printTable(const Table& table);

SynTree*
parseExpression(const std::vector<Token>& tokens, size_t& idx);

Command
parseFindCommand(const std::vector<Token>& tokens, size_t& idx);

Command
parseDefCommand(const std::vector<Token>& tokens, size_t& idx);

Command
parseRunCommand(const std::vector<Token>& tokens, size_t& idx);

Command
parseAllCommand(const std::vector<Token>& tokens, size_t& idx);

std::pair<size_t, Command>
parser(size_t idx, std::vector<Token>* tokens);

#endif // PARSER_H
