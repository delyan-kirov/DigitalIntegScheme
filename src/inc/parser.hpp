/*-----------------------------MODULE INFO-----------------------------/
 * \file parser.hpp
 * \author Delyan Kirov
 * \brief Interface for the parser
 *---------------------------------------------------------------------*/

#ifndef PARSER_H
#define PARSER_H

/*----------------------------------------------------------------------/
 *--------------------------MODULE INCLUDES-----------------------------/
 *---------------------------------------------------------------------*/
#include "tokenizer.hpp"

/*----------------------------------------------------------------------/
 *---------------------------MODULE TYPES-------------------------------/
 *---------------------------------------------------------------------*/

//! \brief Enum for operation types
enum class OperationType
{
  AND,
  OR,
  NOT,
};

//! \brief Enum for algebraic types
enum class AlgebraType
{
  VALUE,
  OPERATION,
  VARIABLE,
  NONE // Default state
};

//! \brief for algebraic expressions
struct Algebra
{
  AlgebraType type;
  unsigned char value;
  OperationType operation;
  std::string variable;
};

namespace std
{
inline std::string
to_string (const Algebra &val)
{
  switch (val.type)
    {
    case AlgebraType::VALUE: return std::to_string (val.value);
    case AlgebraType::OPERATION:
      switch (val.operation)
        {
        case OperationType::AND: return "&";
        case OperationType::OR : return "|";
        case OperationType::NOT: return "!";
        default                : return "UNKNOWN ALGEBRA OPERATION TYPE";
        }
    case AlgebraType::VARIABLE: return val.variable;
    default                   : return "UNKNOWN ALGEBRATYPE";
    }
}
}

//! \brief Enum for the types of commands
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

//! \brief Structure for table representation
struct Table
{
  size_t N = 0;
  size_t M = 0;
  std::vector<unsigned char> input{};
  std::vector<unsigned char> output{};
};

namespace std
{
inline std::string
to_string (const Table &table)
{
  // Ensure that input size is a multiple of N and output size is M
  if (table.input.size () % table.N != 0 || table.output.size () != table.M)
    {
      return "Invalid table data: input size must be a multiple of N and "
             "output size must be M.\n";
    }

  // Calculate number of rows
  size_t rows = table.input.size () / table.N;

  // Build the table string
  std::string result;

  result += "Table (N=" + std::to_string (table.N)
            + ", M=" + std::to_string (table.M) + "):\n";

  // Add header
  for (size_t i = 0; i < table.N; ++i)
    {
      result += 'i' + std::to_string (i + 1) + "\t";
    }
  result += "out\n";

  // Add rows
  for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < table.N; ++j)
        {
          result += std::to_string (
                        static_cast<int> (table.input[i * table.N + j]))
                    + "\t";
        }
      if (i < table.M)
        {
          result += std::to_string (static_cast<int> (table.output[i]));
        }
      result += "\n";
    }

  return result;
}
}

//! \brief Structure for syntax tree nodes
struct SynTree
{
  Algebra val;
  SynTree *left;
  SynTree *right;

  SynTree (const Algebra &value, SynTree *leftNode = nullptr,
           SynTree *rightNode = nullptr)
      : val (value), left (leftNode), right (rightNode)
  {
  }
  void
  destroy ()
  {
    // Recursively delete left and right subtrees
    if (this->left != nullptr)
      {
        delete this->left;
        left = nullptr;
      }
    if (this->right != nullptr)
      {
        delete this->right;
        right = nullptr;
      }
  }
};

//! \brief Structure for handling commands
struct Command
{
  SynTree *definition = nullptr;
  CommandType type = CommandType::TRIVIAL;
  std::vector<std::string> arguments{};
  std::vector<unsigned char> values{};
  Table table{};
  std::string name = "";
};

/*----------------------------------------------------------------------/
 *--------------------------MODULE FUNCTIONS----------------------------/
 *---------------------------------------------------------------------*/

//! \brief Parser
extern std::pair<size_t, Command> parser (size_t idx,
                                          std::vector<Token> *tokens);

#endif // PARSER_H

/*----------------------------------------------------------------------/
 *-----------------------------------EOF--------------------------------/
 *---------------------------------------------------------------------*/