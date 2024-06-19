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
  unsigned char val; // this type makes no sense
  SynTree* left;
  SynTree* right;
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
  const std::string name;
};

// gobal list of definitions
size_t nameSpaceIdx = 0;
std::vector<std::pair<std::string, SynTree>> programNameSpace;

// static Command parseSynTree (std::vector<Token>* tokens){
//
// }

Command
parser(std::vector<Token>* tokens)
{
  CommandType commandType;
  auto values = new std::vector<unsigned char>;
  auto arguments = new std::vector<std::string>;
  auto table = new std::vector<std::vector<unsigned char>>;
  std::string definitionName;
  int idx = 0;
  TokenType commandTypeRaw = tokens->at(idx++).type;

  switch (commandTypeRaw) {

    case TokenType::DEFINE:
      commandType = CommandType::DEFINE;
      if (tokens->at(idx++).type != TokenType::VAR_NAME) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: definition name expected, found: "
                  << printTokenType(currTokenType) << '\n';
        return Command{ nullptr };
      } else
        definitionName = tokens->at(idx - 1).name;
      if (tokens->at(idx++).type != TokenType::PAREN_L) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: left paranthesis expected, found: "
                  << printTokenType(currTokenType) << '\n';
        return Command{ nullptr };
      }
      for (;;) {
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
      for (auto i : *arguments)
        std::cout << i << ", ";
      std::cout << definitionName << std::endl;
      if (tokens->at(idx++).type != TokenType::COLS) {
        const TokenType currTokenType = tokens->at(idx).type;
        std::cerr << "SYNTAX ERROR: columns expected, found: "
                  << printTokenType(currTokenType) << ' '
                  << tokens->at(idx).name << '\n';
        return Command{ nullptr };
      }
      break; // end case: TokenType::DEFINE

    case TokenType::RUN: commandType = CommandType::RUN; break;
    case TokenType::ALL: commandType = CommandType::ALL; break;
    case TokenType::FIND: commandType = CommandType::FIND; break;
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
