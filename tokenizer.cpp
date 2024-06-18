#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>
#if 0
  g++ -g -o tokenizer tokenizer.cpp && ./tokenizer && exit;
#endif // 0

#include <fstream>
#include <iostream>

enum class TokenType
{
  PAREN_L,
  PAREN_R,
  // key words
  DEFINE,
  RUN,
  ALL,
  FIND,
  // variables
  VAR_NAME,
  VAL,
  // syntax
  NEWLINE,
  COMMA,
  COLS,
  SEMICOLS,
  QMARK,
  // boolean algebra
  AND,
  OR,
  NOT,
};

std::string
printTokenType(const TokenType& type)
{
  switch (type) {
    case TokenType::PAREN_L: return "PAREN_L";
    case TokenType::PAREN_R: return "PAREN_R";
    case TokenType::DEFINE: return "DEFINE";
    case TokenType::RUN: return "RUN";
    case TokenType::FIND: return "FIND";
    case TokenType::ALL: return "ALL";
    case TokenType::VAR_NAME: return "VAR_NAME";
    case TokenType::VAL: return "VAL";
    case TokenType::NEWLINE: return "NEWLINE";
    case TokenType::COMMA: return "COMMA";
    case TokenType::SEMICOLS: return "SEMICOLS";
    case TokenType::COLS: return "COLS";
    case TokenType::QMARK: return "QMARK";
    case TokenType::AND: return "AND";
    case TokenType::OR: return "OR";
    case TokenType::NOT: return "NOT";
    default: return "UNKNOWN";
  }
}

struct Token
{
  TokenType type;
  unsigned char val;
  std::string name;
};

void
printTokens(const std::vector<Token>& tokens)
{
  for (size_t i = 0; i < tokens.size(); ++i) {
    Token token = tokens[i];
    std::cout << "TOKEN: " << printTokenType(token.type) << ' ' << token.name
              << ' ';
    if ((unsigned int)token.val != 2) std::cout << (unsigned int)token.val;
    std::cout << std::endl;
  }
}

// FIX
std::vector<Token>*
tokenizer(std::fstream& file)
{
  auto tokens = new std::vector<Token>;
  Token newToken;
  TokenType tokenType;
  std::string tokenName;
  unsigned char value;

  char c;
  while (file.get(c)) {
    std::cout << c;

    if (isalnum(c)) {
      tokenName += c;
    } else if (c == '&') {
      newToken = { .type = TokenType::AND, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == '|') {
      newToken = { .type = TokenType::OR, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == '!') {
      newToken = { .type = TokenType::NOT, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == ' ' || c == '(' || c == ')' || c == ',' || c == '"' ||
               c == ':' || c == ';' || c == '\n') {
      if (tokenName == "DEFINE") {
        newToken = { .type = TokenType::DEFINE, .val = 2, .name = "" };
        tokens->push_back(newToken);
      } else if (tokenName == "RUN") {
        newToken = { .type = TokenType::RUN, .val = 2, .name = "" };
        tokens->push_back(newToken);
      } else if (tokenName == "FIND") {
        newToken = { .type = TokenType::FIND, .val = 2, .name = "" };
        tokens->push_back(newToken);
      } else if (tokenName == "ALL") {
        newToken = { .type = TokenType::ALL, .val = 2, .name = "" };
        tokens->push_back(newToken);
      } else if (tokenName != "" && tokenName != "1" && tokenName != "0") {
        newToken = { .type = TokenType::VAR_NAME, .val = 2, .name = tokenName };
        tokens->push_back(newToken);
      } else if (tokenName == "1") {
        value = 1;
        newToken = { .type = TokenType::VAL, .val = 1, .name = "" };
        tokens->push_back(newToken);
      } else if (tokenName == "0") {
        value = 0;
        newToken = { .type = TokenType::VAL, .val = 0, .name = "" };
        tokens->push_back(newToken);
      }
      tokenName = "";
    } else {
      std::cerr << "ERROR: Unrecognized token: " << c << '\n';
      exit(1);
    }
    if (c == '(') {
      newToken = { .type = TokenType::PAREN_L, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == ')') {
      newToken = { .type = TokenType::PAREN_R, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == ':') {
      newToken = { .type = TokenType::COLS, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == ';') {
      newToken = { .type = TokenType::SEMICOLS, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == '"') {
      newToken = { .type = TokenType::QMARK, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == ',') {
      newToken = { .type = TokenType::COMMA, .val = 2, .name = "" };
      tokens->push_back(newToken);
    } else if (c == '\n') {
      newToken = { .type = TokenType::NEWLINE, .val = 2, .name = "" };
      tokens->push_back(newToken);
    }
  }
  return tokens;
}

void
test_tokenizer(const std::string& fileName)
{
  std::cout << "TEST: tokenizer\n";
  std::fstream infile(fileName);
  if (!infile.is_open()) {
    std::cerr << "ERROR: Could not open file\n";
    exit(1);
  } else {
    std::cout << "INFO: File successfully loaded\n";
  }

  auto tokens = tokenizer(infile);
  printTokens(*tokens);
  infile.close();
  delete tokens;
}

int
main()
{
  test_tokenizer("./examples/ic1.txt");
  test_tokenizer("./examples/ic2.txt");
  test_tokenizer("./examples/ic3.txt");
  test_tokenizer("./examples/find.txt");
  return 0;
}
