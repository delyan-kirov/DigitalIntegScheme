#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>
#if 0
  g++ -g -o tokenizer tokenizer.cpp && ./tokenizer && exit;
#endif // 0

#include <fstream>
#include <iostream>

enum class TokenType {
  PAREN_L,
  PAREN_R,
  // key words
  DEFINE,
  RUN,
  ALL,
  // variables
  VAR_NAME,
  VAL,
  // syntax
  NEWLINE,
  COMMA,
  SEMICOLS,
  QMARK,
  // boolean algebra
  AND,
  OR,
  NOT,
};

std::string printTokenType(const TokenType &type) {
  switch (type) {
  case TokenType::PAREN_L:
    return "PAREN_L";
  case TokenType::PAREN_R:
    return "PAREN_R";
  case TokenType::DEFINE:
    return "DEFINE";
  case TokenType::RUN:
    return "RUN";
  case TokenType::ALL:
    return "ALL";
  case TokenType::VAR_NAME:
    return "VAR_NAME";
  case TokenType::VAL:
    return "VAL";
  case TokenType::NEWLINE:
    return "NEWLINE";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::SEMICOLS:
    return "SEMICOLS";
  case TokenType::QMARK:
    return "QMARK";
  case TokenType::AND:
    return "AND";
  case TokenType::OR:
    return "OR";
  case TokenType::NOT:
    return "NOT";
  default:
    return "UNKNOWN";
  }
}

struct Token {
  TokenType type;
  unsigned char val;
  std::string name;
};

void printTokens(const std::vector<Token> &tokens) {
  for (size_t i = 0; i < tokens.size(); ++i) {
    Token token = tokens[i];
    std::cout << "TOKEN: " << printTokenType(token.type) << ' ' << token.name
              << ' ' << token.val << '\n';
  }
}

// FIX
std::vector<Token> *tokenizer(std::fstream &file) {
  auto tokens = new std::vector<Token>;
  Token newToken;
  TokenType tokenType;
  std::string tokenName;
  unsigned char value;

  char c;
  while (file.get(c)) {
    std::cout << c;

    if (isalpha(c)) {
      tokenName += c;
    } else if (c == '1') {
      value = 1;
      tokenType = TokenType::VAL;
      newToken = {.type = tokenType, .val = 1, .name = ""};
      tokens->push_back(newToken);
    } else if (c == '0') {
      value = 0;
      tokenType = TokenType::VAL;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
    } else if (c == '&') {
      tokenType = TokenType::AND;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
    } else if (c == '|') {
      tokenType = TokenType::OR;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
    } else if (c == '(') {
      tokenType = TokenType::PAREN_L;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
      tokenName = "";
    } else if (c == ')') {
      tokenType = TokenType::PAREN_R;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
      tokenName = "";
    } else if (c == '\n') {
      tokenType = TokenType::NEWLINE;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
      tokenName = "";
    } else if (c == ',') {
      tokenType = TokenType::COMMA;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
      tokenName = "";
    } else if (c == ':') {
      tokenType = TokenType::SEMICOLS;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
    } else if (c == '"') {
      tokenType = TokenType::QMARK;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
    } else if (c == '!') {
      tokenType = TokenType::NOT;
      newToken = {.type = tokenType, .val = 0, .name = ""};
      tokens->push_back(newToken);
    } else if (c == ' ') {
      if (tokenName == "DEFINE") {
        tokenType = TokenType::DEFINE;
        newToken = {.type = tokenType, .val = 0, .name = ""};
        tokens->push_back(newToken);
        tokenName = "";
      } else if (tokenName == "RUN") {
        tokenType = TokenType::RUN;
        newToken = {.type = tokenType, .val = 0, .name = ""};
        tokens->push_back(newToken);
        tokenName = "";
      } else if (tokenName == "ALL") {
        tokenType = TokenType::ALL;
        newToken = {.type = tokenType, .val = 0, .name = ""};
        tokens->push_back(newToken);
        tokenName = "";
      } else {
        tokenType = TokenType::VAR_NAME;
        newToken = {.type = tokenType, .val = 0, .name = tokenName};
        tokens->push_back(newToken);
        tokenName = "";
      }
    } else {
      std::cerr << "ERROR: Unrecognized token: " << c << '\n';
      exit(1);
    }
  }
  return tokens;
}

int main() {
  std::cout << "TEST: tokenizer\n";

  std::string fileName = "./examples/ic1.txt";
  std::fstream infile(fileName);
  if (!infile.is_open()) {
    std::cerr << "ERROR: Could not open file\n";
    exit(1);
  } else {
    std::cout << "INFO: File successfully loaded\n";
  }

  auto tokens = tokenizer(infile);
  printTokens(*tokens);
  // delete[] tokens;
  // infile.close();
  return 0;
}
