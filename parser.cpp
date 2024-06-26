#include "tokenizer.h"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <utility>

enum class OperationType
{
  AND,
  OR,
  NOT,
};

enum class AlgebraType
{
  VALUE,
  OPERATION,
  VARIABLE,
  NONE
};

struct Algebra
{
  AlgebraType type;
  unsigned char value;
  OperationType operation;
  std::string variable;
};

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

struct SynTree
{
  Algebra val;
  SynTree* left;
  SynTree* right;

  SynTree(const Algebra& value,
          SynTree* leftNode = nullptr,
          SynTree* rightNode = nullptr)
    : val(value)
    , left(leftNode)
    , right(rightNode)
  {
  }
  void destroy()
  {
    // Recursively delete left and right subtrees
    if (this->left != nullptr) {
      delete this->left;
      left = nullptr; // Optional: Set to nullptr after deletion
    }
    if (this->right != nullptr) {
      delete this->right;
      right = nullptr; // Optional: Set to nullptr after deletion
    }
  }
};

static size_t
powerOf(size_t a, size_t b)
{
  size_t result = 1;
  while (b > 0) {
    result *= a;
    b--;
  }
  return result;
}

static OperationType
tokenToOpType(const TokenType& tokenType)
{
  switch (tokenType) {
    case TokenType::OR: return OperationType::OR;
    case TokenType::AND: return OperationType::AND;
    case TokenType::NOT: return OperationType::NOT;
    default: {
      std::cerr << "PARSE ERROR: expected operation | & or !, found token "
                << printTokenType(tokenType);
      return OperationType::NOT;
    }
  }
}

static std::string
printAlgebraType(const Algebra& val)
{
  switch (val.type) {
    case AlgebraType::VALUE: return std::to_string(val.value);
    case AlgebraType::OPERATION:
      switch (val.operation) {
        case OperationType::AND: return "&";
        case OperationType::OR: return "|";
        case OperationType::NOT: return "!";
      }
    case AlgebraType::VARIABLE: return val.variable;
    default: return "Unknown AlgebraType";
  }
}

struct Table
{
  size_t N = 0;
  size_t M = 0;
  std::vector<unsigned char> input = std::vector<unsigned char>();
  std::vector<unsigned char> output = std::vector<unsigned char>();
};

static void
printSyntaxTree(const SynTree* node, int depth = 0)
{
  if (!node) return;
  for (int i = 0; i < depth; ++i)
    std::cout << "  ";
  std::cout << printAlgebraType(node->val) << "\n";
  printSyntaxTree(node->left, depth + 1);
  printSyntaxTree(node->right, depth + 1);
}

static void
printTable(const Table& table)
{
  // Ensure that input size is a multiple of N and output size is M
  if (table.input.size() % table.N != 0 || table.output.size() != table.M) {
    std::cerr << "Invalid table data: input size must be a multiple of N and "
                 "output size must be M.\n";
    return;
  }

  // Calculate number of rows
  size_t rows = table.input.size() / table.N;

  // Print the table
  std::cout << "Table (N=" << table.N << ", M=" << table.M << "):\n";

  // Print header
  for (size_t i = 0; i < table.N; ++i) {
    std::cout << 'i' << i + 1 << "\t";
  }
  std::cout << "out\n";

  // Print rows
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < table.N; ++j) {
      std::cout << static_cast<int>(table.input[i * table.N + j]) << "\t";
    }
    if (i < table.M) { std::cout << static_cast<int>(table.output[i]); }
    std::cout << "\n";
  }
}

struct Command
{
  SynTree* definition = nullptr;
  CommandType type = CommandType::TRIVIAL;
  std::vector<std::string> arguments;
  std::vector<unsigned char> values;
  Table table;
  std::string name = "";
};

// Forward declaration
static SynTree*
parseExpression(const std::vector<Token>& tokens, size_t& idx);

inline static SynTree*
parseFactor(const std::vector<Token>& tokens, size_t& idx)
{
  if (idx >= tokens.size()) { return nullptr; }

  Token token = tokens[idx];

  if (token.type == TokenType::VAR_NAME) {
    idx++;
    Algebra varName;
    varName.type = AlgebraType::VARIABLE;
    varName.variable = token.name;
    return new SynTree(varName);

  } else if (token.type == TokenType::VAL) {
    idx++; // Skip '('
    Algebra value;
    value.type = AlgebraType::VALUE;
    value.value = token.val;
    return new SynTree(value);

  } else if (token.type == TokenType::PAREN_L) {
    idx++; // Skip '('
    SynTree* expr = parseExpression(tokens, idx);
    if (idx >= tokens.size() || tokens[idx].type != TokenType::PAREN_R) {
      std::cerr << "SYNTAX ERROR: Mismatched parentheses\n";
      return nullptr;
    }
    idx++; // Skip ')'
    return expr;

  } else if (token.type == TokenType::NOT) {
    idx++; // Skip '!'
    SynTree* factor = parseFactor(tokens, idx);
    Algebra operation;
    operation.type = AlgebraType::OPERATION;
    operation.operation = OperationType::NOT;
    return new SynTree(operation, nullptr, factor);

  } else {
    std::cerr << "SYNTAX ERROR: Unexpected token in parseFactor: "
              << printTokenType(token.type) << '\n';
    return nullptr;
  }
}

inline static SynTree*
parseTerm(const std::vector<Token>& tokens, size_t& idx)
{
  SynTree* node = parseFactor(tokens, idx);
  if (!node) { return nullptr; }

  while (idx < tokens.size() && tokens[idx].type != TokenType::QMARK &&
         tokens[idx].type == TokenType::AND) {
    idx++; // Skip '&'
    SynTree* right = parseFactor(tokens, idx);
    if (!right) {
      std::cerr << "SYNTAX ERROR: Failed to parse right factor in parseTerm\n";
      return nullptr;
    }
    Algebra operation;
    operation.type = AlgebraType::OPERATION;
    operation.operation = OperationType::AND;
    node = new SynTree(operation, node, right);
  }
  return node;
}

inline static SynTree*
parseExpression(const std::vector<Token>& tokens, size_t& idx)
{
  if (tokens.at(idx).type == TokenType::QMARK &&
      tokens.at(idx).type == TokenType::NEWLINE)
    ++idx;

  SynTree* node = parseTerm(tokens, idx);
  if (!node) { return nullptr; }

  while (idx < tokens.size() && tokens[idx].type != TokenType::QMARK &&
         tokens[idx].type == TokenType::OR) {
    idx++; // Skip '|'
    SynTree* right = parseTerm(tokens, idx);
    if (!right) {
      std::cerr
        << "SYNTAX ERROR: Failed to parse right term in parseExpression\n";
      return nullptr;
    }
    Algebra operation;
    operation.type = AlgebraType::OPERATION;
    operation.operation = OperationType::OR;
    node = new SynTree(operation, node, right);
  }
  return node;
}

inline static Command
parseFindCommand(const std::vector<Token>& tokens, size_t& idx)
{
  CommandType commandType = CommandType::FIND;
  Table* table = new Table;

  size_t canEnd = 0;
  std::vector<unsigned char>* output = new std::vector<unsigned char>();

  // Check for file first
  if (idx < tokens.size() && tokens.at(idx).type == TokenType::QMARK) {
    if (tokens.at(++idx).type != TokenType::VAR_NAME) {
      std::cerr << "SYNTAX ERROR: expected file name. Found: "
                << printTokenType(tokens.at(idx).type) << '\n';
      return Command{ nullptr };
    }
    if (tokens.at(idx + 1).type != TokenType::QMARK) {
      std::cerr << "SYNTAX ERROR: expected closing of \". Found: "
                << printTokenType(tokens.at(idx).type) << '\n';
      return Command{ nullptr };
    }
    std::string fileName = "./csvFiles/" + tokens.at(idx).name;
    FILE* file = fopen(fileName.c_str(), "r");
    if (file == nullptr) {
      std::cerr << "ERROR: could not open file: " << fileName << '\n';
      return Command{ nullptr };
    }
    std::vector<Token>* tokens = tokenizer(file);
    idx += 2; // must move the index forward twice
    size_t newIdx = 0;
    return parseFindCommand(*tokens, newIdx);
  }

  while (idx < tokens.size()) {
    // Make special chech when i is zero initially
    if (idx == 0) {
      if (tokens.at(idx).type != TokenType::VAL ||
          (tokens.at(idx).val != 0 && tokens.at(idx).val != 1)) {
        std::cerr << "SYNTAX ERROR: value 0 or 1 expected, found: "
                  << printTokenType(tokens.at(idx).type) << '\n';
        return Command{ nullptr };
      }
      table->input.push_back(tokens.at(idx).val);
      idx += 2;
    }
    // Ensure the token is a VAL and it is either 0 or 1
    if (tokens.at(idx).type != TokenType::VAL ||
        (tokens.at(idx).val != 0 && tokens.at(idx).val != 1)) {
      std::cerr << "SYNTAX ERROR: value 0 or 1 expected, found: "
                << printTokenType(tokens.at(idx).type) << '\n';
      return Command{ nullptr };
    }
    if (tokens.at(idx - 1).type != TokenType::COLS) {
      table->input.push_back(tokens.at(idx).val);
    }
    ++idx;

    // Ensure the token is a COMMA, COLS, SEMICOLS, or NEWLINE
    if (idx < tokens.size()) {
      TokenType nextType = tokens.at(idx).type;
      if (nextType == TokenType::COMMA) {
        ++idx;
      } else if (nextType == TokenType::COLS) {
        ++idx;
        // Ensure the token is a VAL and it is either 0 or 1
        if (tokens.at(idx).type != TokenType::VAL ||
            (tokens.at(idx).val != 0 && tokens.at(idx).val != 1)) {
          std::cerr << "SYNTAX ERROR: value 0 or 1 expected, found: "
                    << printTokenType(tokens.at(idx).type) << '\n';
          return Command{ nullptr };
        }
        table->output.push_back(tokens.at(idx).val);
        table->N++;
      } else if (nextType == TokenType::SEMICOLS) {
        ++idx;
        if (idx < tokens.size() && tokens.at(idx).type == TokenType::NEWLINE) {
          ++idx;
          if (idx < tokens.size() && tokens.at(idx).type != TokenType::VAL) {
            break; // End the loop if the next token is not a VAL
          }
        }
      } else if (nextType == TokenType::NEWLINE) {
        ++idx;
        if (idx < tokens.size() && tokens.at(idx).type != TokenType::VAL) {
          break; // End the loop if the next token is not a VAL
        }
      } else {
        std::cerr
          << "SYNTAX ERROR: expected comma, colons or semicolons. Found: "
          << printTokenType(nextType) << '\n';
        return Command{ nullptr };
      }
    } else {
      std::cerr << "SYNTAX ERROR: Unexpected end of tokens\n";
      return Command{ nullptr };
    }
  }

  table->N = table->input.size() / table->output.size();
  table->M = table->output.size();

  // printTable(*table); // DEBUG
  if (powerOf(2, table->N) != table->M) {
    std::cerr
      << "PARSE ERROR: the table defined with FIND command is invalid\n";
    return Command{ nullptr };
  };

  return Command{ .type = CommandType::FIND, .table = *table, .name = "" };
}

static Command
parseDefCommand(const std::vector<Token>& tokens, size_t& idx)
{
  CommandType commandType = CommandType::DEFINE;
  std::vector<std::string>* arguments = new std::vector<std::string>;
  std::string definitionName;
  SynTree* definition;

  if (tokens.at(idx++).type != TokenType::VAR_NAME) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: definition name expected, found: "
              << printTokenType(currTokenType) << '\n';
    return Command{ nullptr };
  } else {
    definitionName = tokens.at(idx - 1).name;
  }

  if (tokens.at(idx++).type != TokenType::PAREN_L) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: left parenthesis expected, found: "
              << printTokenType(currTokenType) << '\n';
    return Command{ nullptr };
  }

  for (;;) {
    if (idx >= tokens.size()) {
      std::cerr
        << "SYNTAX ERROR: Unexpected end of tokens in DEFINE arguments\n";
      return Command{ nullptr };
    }
    auto tokenType = tokens.at(idx).type;
    if (tokenType == TokenType::VAR_NAME) {
      const std::string arg = tokens.at(idx).name;
      arguments->push_back(arg);
      idx++; // Move to the next token after pushing into arguments
    } else if (tokenType == TokenType::COMMA) {
      idx++; // Move past the comma token
      continue;
    } else if (tokenType == TokenType::PAREN_R) {
      idx++; // Move past the closing parenthesis token
      break;
    } else {
      std::cerr << "SYNTAX ERROR: unexpected token found: "
                << printTokenType(tokenType) << '\n';
      return Command{ nullptr };
    }
  }

  if (tokens.at(idx++).type != TokenType::COLS) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: columns expected, found: "
              << printTokenType(currTokenType) << ' ' << tokens.at(idx).name
              << '\n';
    return Command{ nullptr };
  }

  if (tokens.at(idx++).type != TokenType::QMARK) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: Expected \" , found: "
              << printTokenType(currTokenType) << ' ' << tokens.at(idx).name
              << '\n';
    return Command{ nullptr };
  }

  // Parse the syntax tree definition
  definition = parseExpression(tokens, idx);

  if (tokens.at(idx++).type != TokenType::QMARK) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: Expected \" , found: "
              << printTokenType(currTokenType) << ' ' << tokens.at(idx).name
              << '\n';
    return Command{ nullptr };
  }

  if (!definition) {
    std::cerr << "SYNTAX ERROR: failed to parse syntax tree\n";
    return Command{
      nullptr,
    };
  }

  // printSyntaxTree(definition);
  return Command{ .definition = definition,
                  .type = CommandType::DEFINE,
                  .arguments = *arguments,
                  .name = definitionName };
}

static Command
parseRunCommand(const std::vector<Token>& tokens, size_t& idx)
{
  if (tokens.at(idx).type != TokenType::VAR_NAME) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: definition name expected, found: "
              << printTokenType(currTokenType) << '\n';
    return Command{ nullptr };
  }

  std::string definitionName = tokens.at(idx++).name;
  std::vector<unsigned char> values;

  // Check for left parenthesis
  if (tokens.at(idx++).type != TokenType::PAREN_L) {
    const TokenType currTokenType = tokens.at(idx).type;
    std::cerr << "SYNTAX ERROR: left parenthesis expected, found: "
              << printTokenType(currTokenType) << '\n';
    return Command{ nullptr };
  }

  // Parse arguments
  while (tokens.at(idx).type != TokenType::PAREN_R) {
    if (idx >= tokens.size()) {
      std::cerr << "SYNTAX ERROR: Unexpected end of tokens in RUN arguments\n";
      return Command{ nullptr };
    }

    if (tokens.at(idx).type == TokenType::VAL) {
      values.push_back(tokens.at(idx).val);
      idx++;
    } else if (tokens.at(idx).type == TokenType::COMMA) {
      idx++; // Skip comma
    } else {
      std::cerr << "SYNTAX ERROR: unexpected token found: "
                << printTokenType(tokens.at(idx).type) << '\n';
      return Command{ nullptr };
    }
  }

  // Move past the closing parenthesis
  idx++;

  // Check for end of line
  if (tokens.at(idx++).type != TokenType::NEWLINE) {
    std::cerr << "SYNTAX ERROR: Expected end of line, found: "
              << printTokenType(tokens.at(idx - 1).type) << ' '
              << tokens.at(idx - 1).name << '\n';
    return Command{ nullptr };
  }

  return Command{ .type = CommandType::RUN,
                  .values = values,
                  .name = definitionName };
}

inline static Command
parseAllCommand(const std::vector<Token>& tokens, size_t& idx)
{
  if (tokens.at(idx).type != TokenType::VAR_NAME) {
    std::cerr << "SYNTAX ERROR: definition name expected, found: "
              << printTokenType(tokens.at(idx).type) << '\n';
    return Command{ nullptr };
  }

  std::string name = tokens.at(idx++).name;
  SynTree* definition = nullptr;

  return Command{ .definition = definition,
                  .type = CommandType::ALL,
                  .name = name };
}

std::pair<size_t, Command>
parser(size_t idx, std::vector<Token>* tokens)
{
  if (idx >= tokens->size()) {
    // std::cerr << "SYNTAX ERROR: No command found\n";
    return std::pair(idx, Command{ .type = CommandType::EXIT });
  }

  TokenType commandTypeRaw = tokens->at(idx++).type;

  switch (commandTypeRaw) {
    case TokenType::DEFINE: {
      return std::pair(idx, parseDefCommand(*tokens, idx));
    } break; // END DEFINE

    case TokenType::RUN: {
      return std::pair(idx, parseRunCommand(*tokens, idx));
    } break; // END RUN

    case TokenType::ALL: {
      return std::pair(idx, parseAllCommand(*tokens, idx));
    } break; // end ALL

    case TokenType::FIND: {
      return std::pair(idx, parseFindCommand(*tokens, idx));
    } break; // END FIND

    case TokenType::NEWLINE: {
      return std::pair(idx, Command{ .type = CommandType::TRIVIAL });
    } break; // END NEWLINE

    case TokenType::CLEAR: {
      return std::pair(idx, Command{ .type = CommandType::CLEAR });
    } break; // CLEAR

    default:
      std::cerr << "SYNTAX ERROR: Command must start with DEFINE, RUN, CLEAR "
                   "or ALL\n";
      return std::pair(idx, Command{ .type = CommandType::TRIVIAL });
  }
}
