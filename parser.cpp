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
};

struct Command
{
  const SynTree* definition;
  const std::vector<std::string> arguments;
  const std::vector<unsigned char> values;
  const std::vector<std::vector<unsigned char>> table;
  const size_t nameSpaceIdx;
};

size_t nameSpaceIdx = 0;
std::vector<SynTree*> programNameSpace;

SynTree*
parseExpression(const std::vector<Token>& tokens, int& idx);

// Forward declaration

SynTree*
parseFactor(const std::vector<Token>& tokens, int& idx)
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
parseTerm(const std::vector<Token>& tokens, int& idx)
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
parseExpression(const std::vector<Token>& tokens, int& idx)
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
parser(std::vector<Token>* tokens)
{
  CommandType commandType;
  auto values = new std::vector<unsigned char>;
  auto arguments = new std::vector<std::string>;
  auto table = new std::vector<std::vector<unsigned char>>;
  std::string definitionName;
  int idx = 0;
  SynTree* definition;

  if (idx >= tokens->size()) {
    std::cerr << "SYNTAX ERROR: No command found\n";
    return Command{ nullptr };
  }

  TokenType commandTypeRaw = tokens->at(idx++).type;

  switch (commandTypeRaw) {
    case TokenType::DEFINE:
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
      return Command{ definition, *arguments, *values, *table, nameSpaceIdx++ };

      break; // end case: TokenType::DEFINE

    case TokenType::RUN: commandType = CommandType::RUN; break;
    case TokenType::ALL: commandType = CommandType::ALL; break;
    case TokenType::FIND: commandType = CommandType::FIND; break;
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
  std::string fileName = "./examples/ic2.txt";
  FILE* infile;
  infile = fopen(fileName.c_str(), "r");
  if (infile == NULL) {
    std::cerr << "ERROR: Could not open file\n";
    exit(1);
  } else {
    std::cout << "INFO: File successfully loaded\n";
  }

  std::vector<Token>* tokens = tokenizer(infile);
  printTokens(*tokens);
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
