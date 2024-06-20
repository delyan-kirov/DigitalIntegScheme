#include "tokenizer.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#if 0
  g++ -g -o parser parser.cpp tokenizer.cpp && ./parser && rm ./parser && exit;
#endif // 0

struct SynTree
{
  std::string val; // Changed to string to handle different types of nodes
  SynTree* left;
  SynTree* right;

  SynTree(const std::string& value,
          SynTree* leftNode = nullptr,
          SynTree* rightNode = nullptr)
    : val(value)
    , left(leftNode)
    , right(rightNode)
  {
  }
};

enum class CommandType
{
  DEFINE,
  RUN,
  ALL,
  FIND,
  CLEAR,
  TRIVIAL,
};

struct Table
{
  size_t N = 0;
  size_t M = 0;
  std::vector<unsigned char> input = std::vector<unsigned char>();
  std::vector<unsigned char> output = std::vector<unsigned char>();
};

void
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
  size_t nameSpaceIdx = 0;
  std::string name = nullptr;
};

size_t nameSpaceIdx = 0;
std::vector<SynTree*> programNameSpace;

SynTree*
parseExpression(const std::vector<Token>& tokens, size_t& idx);

// Forward declaration

SynTree*
parseFactor(const std::vector<Token>& tokens, size_t& idx)
{
  if (idx >= tokens.size()) { return nullptr; }

  Token token = tokens[idx];
  if (token.type == TokenType::VAR_NAME) {
    idx++;
    return new SynTree(token.name);
  } else if (token.type == TokenType::VAL) {
    idx++; // Skip '('
    return new SynTree(std::to_string(token.val));
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
    return new SynTree("!", nullptr, factor);
  } else {
    std::cerr << "SYNTAX ERROR: Unexpected token in parseFactor: "
              << printTokenType(token.type) << '\n';
    return nullptr;
  }
}

SynTree*
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
    node = new SynTree("&", node, right);
  }
  return node;
}

SynTree*
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
    node = new SynTree("|", node, right);
  }
  return node;
}

void
printSyntaxTree(const SynTree* node, int depth = 0)
{
  if (!node) return;
  for (int i = 0; i < depth; ++i)
    std::cout << "  ";
  std::cout << node->val << "\n";
  printSyntaxTree(node->left, depth + 1);
  printSyntaxTree(node->right, depth + 1);
}

Command
parseFindCommand(const std::vector<Token>& tokens, size_t& idx)
{
  CommandType commandType = CommandType::FIND;
  std::vector<unsigned char>* values = new std::vector<unsigned char>();
  std::vector<std::string>* arguments = new std::vector<std::string>();
  Table* table = new Table();
  std::string definitionName;
  SynTree* definition;

  size_t canEnd = 0;
  std::vector<unsigned char>* output = new std::vector<unsigned char>();

  while (idx < tokens.size()) {
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
  printTable(*table); // DEBUG

  return Command{ definition, CommandType::FIND, *arguments,    *values,
                  *table,     nameSpaceIdx++,    definitionName };
}

Command
parser(std::vector<Token>* tokens)
{
  CommandType commandType;
  auto values = new std::vector<unsigned char>;
  auto arguments = new std::vector<std::string>;
  auto table = new Table;
  std::string definitionName;
  size_t idx = 0;
  SynTree* definition;

  if (idx >= tokens->size()) {
    std::cerr << "SYNTAX ERROR: No command found\n";
    return Command{ nullptr };
  }

  TokenType commandTypeRaw = tokens->at(idx++).type;

  switch (commandTypeRaw) {
    case TokenType::DEFINE: {

      commandType = CommandType::DEFINE;

      if (tokens->at(idx++).type != TokenType::VAR_NAME) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: definition name expected, found: "
                  << printTokenType(currTokenType) << '\n';
        return Command{ nullptr };
      } else {
        definitionName = tokens->at(idx - 1).name;
      }

      if (tokens->at(idx++).type != TokenType::PAREN_L) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: left parenthesis expected, found: "
                  << printTokenType(currTokenType) << '\n';
        return Command{ nullptr };
      }

      for (;;) {
        if (idx >= tokens->size()) {
          std::cerr
            << "SYNTAX ERROR: Unexpected end of tokens in DEFINE arguments\n";
          return Command{ nullptr };
        }
        auto tokenType = tokens->at(idx).type;
        if (tokenType == TokenType::VAR_NAME) {
          const std::string arg = tokens->at(idx).name;
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

      if (tokens->at(idx++).type != TokenType::COLS) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: columns expected, found: "
                  << printTokenType(currTokenType) << ' '
                  << tokens->at(idx).name << '\n';
        return Command{ nullptr };
      }

      if (tokens->at(idx++).type != TokenType::QMARK) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: Expected \" , found: "
                  << printTokenType(currTokenType) << ' '
                  << tokens->at(idx).name << '\n';
        return Command{ nullptr };
      }

      // Parse the syntax tree definition
      definition = parseExpression(*tokens, idx);

      if (tokens->at(idx++).type != TokenType::QMARK) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: Expected \" , found: "
                  << printTokenType(currTokenType) << ' '
                  << tokens->at(idx).name << '\n';
        return Command{ nullptr };
      }

      if (!definition) {
        std::cerr << "SYNTAX ERROR: failed to parse syntax tree\n";
        return Command{
          nullptr,
        };
      }

      printSyntaxTree(definition);
      programNameSpace.push_back(definition);
      return Command{ definition, CommandType::DEFINE, *arguments,    *values,
                      *table,     nameSpaceIdx++,      definitionName };
    }

    break; // end case: TokenType::DEFINE

    case TokenType::RUN: {
      commandType = CommandType::RUN;

      if (tokens->at(idx++).type != TokenType::VAR_NAME) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: definition name expected, found: "
                  << printTokenType(currTokenType) << '\n';
        return Command{ nullptr };
      } else {
        definitionName = tokens->at(idx - 1).name;
      }

      if (tokens->at(idx++).type != TokenType::PAREN_L) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: left parenthesis expected, found: "
                  << printTokenType(currTokenType) << '\n';
        return Command{ nullptr };
      }

      for (;;) { // TODO chech if commas are in the right place
        if (idx >= tokens->size()) {
          std::cerr
            << "SYNTAX ERROR: Unexpected end of tokens in RUN arguments\n";
          return Command{ nullptr };
        }
        auto tokenType = tokens->at(idx).type;
        if (tokenType == TokenType::VAL) {
          const unsigned char arg = tokens->at(idx).val;
          values->push_back(arg);
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

      if (tokens->at(idx++).type != TokenType::NEWLINE) {
        std::cerr << "SYNTAX ERROR: Expected end of line, found: "
                  << printTokenType(tokens->at(idx).type) << ' '
                  << tokens->at(idx).name << '\n';
        return Command{ nullptr };
      }

      return Command{ definition, CommandType::RUN, *arguments,    *values,
                      *table,     nameSpaceIdx++,   definitionName };
    }

    break; // end RUN

    case TokenType::ALL: {
      commandType = CommandType::ALL;

      if (tokens->at(idx++).type != TokenType::VAR_NAME) {
        std::cerr << "SYNTAX ERROR: definition name expected, found: "
                  << printTokenType(tokens->at(idx).type) << '\n';
        return Command{ nullptr };
      } else {
        definitionName = tokens->at(idx - 1).name;
      }

      return Command{ definition, CommandType::RUN, *arguments,    *values,
                      *table,     nameSpaceIdx++,   definitionName };
    } break; // end ALL

    case TokenType::FIND: {
      return parseFindCommand(*tokens, idx);
    } break; // END FIND

    case TokenType::NEWLINE: commandType = CommandType::FIND; break;

    default:
      std::cerr << "SYNTAX ERROR: Command must start with DEFINE, RUN or ALL\n";
      return Command{ nullptr };
  }
  return Command{ nullptr };
}

int
main()
{
  std::string fileName = "./examples/find.txt";
  FILE* infile;
  infile = fopen(fileName.c_str(), "r");
  if (infile == NULL) {
    std::cerr << "ERROR: Could not open file\n";
    exit(1);
  } else {
    std::cout << "INFO: File successfully loaded\n";
  }

  std::vector<Token>* tokens = tokenizer(infile);
  // printTokens(*tokens);
  parser(tokens);
  fclose(infile);
  delete tokens;
  return 0;
}

// int
// main()
// {
//   // Example tokens for "a & (b | c) & !d"
//   std::vector<Token> tokens = {
//     { TokenType::QMARK, 0, "" },     { TokenType::VAR_NAME, 0, "a" },
//     { TokenType::AND, 0, "" },       { TokenType::PAREN_L, 0, "" },
//     { TokenType::VAR_NAME, 0, "b" }, { TokenType::OR, 0, "" },
//     { TokenType::VAR_NAME, 0, "c" }, { TokenType::PAREN_R, 0, "" },
//     { TokenType::AND, 0, "" },       { TokenType::NOT, 0, "" },
//     { TokenType::VAR_NAME, 0, "d" }, { TokenType::QMARK, 0, "" }
//   };
//
//   int idx = 0;
//   SynTree* syntaxTree = parseExpression(tokens, idx);
//
//   if (syntaxTree) {
//     std::cout << "Parsed syntax tree:\n";
//     printSyntaxTree(syntaxTree);
//   } else {
//     std::cerr << "SYNTAX ERROR: Failed to parse the expression\n";
//   }
//
//   return 0;
// }
