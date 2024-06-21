#if 0
  g++ -g -o tokenizer tokenizer.cpp && ./tokenizer && rm ./tokenizer && exit;
#endif // 0

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
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

// Function to print token type as a string
std::string
printTokenType(const TokenType& type)
{
  switch (type) {
    case TokenType::PAREN_L: return "PAREN_L";
    case TokenType::PAREN_R: return "PAREN_R";
    case TokenType::DEFINE: return "DEFINE";
    case TokenType::RUN: return "RUN";
    case TokenType::FIND: return "FIND";
    case TokenType::CLEAR: return "CLEAR";
    case TokenType::ALL: return "ALL";
    case TokenType::VAR_NAME: return "VAR_NAME";
    case TokenType::VAL: return "VAL";
    case TokenType::NEWLINE: return "NEWLINE";
    case TokenType::COMMA: return "COMMA";
    case TokenType::COLS: return "COLS";
    case TokenType::SEMICOLS: return "SEMICOLS";
    case TokenType::QMARK: return "QMARK";
    case TokenType::AND: return "AND";
    case TokenType::OR: return "OR";
    case TokenType::NOT: return "NOT";
    default: return "UNKNOWN";
  }
}

// Struct for tokens
struct Token
{
  TokenType type;
  unsigned char val;
  std::string name;
};

// Function to print tokens
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

// Function to tokenize the input file
std::vector<Token>*
tokenizer(FILE* file)
{
  auto tokens = new std::vector<Token>;
  Token newToken;
  std::string tokenName;

  int c;
  while ((c = fgetc(file)) != EOF) {
    std::cout << (char)c;
    if (isalnum(c) || c == '.') {
      tokenName += (char)c;
    } else if (c == '&') {
      tokens->push_back({ TokenType::AND, 2, "" });
    } else if (c == '|') {
      tokens->push_back({ TokenType::OR, 2, "" });
    } else if (c == '!') {
      tokens->push_back({ TokenType::NOT, 2, "" });
    } else if (c == ' ' || c == '(' || c == ')' || c == ',' || c == '"' ||
               c == ':' || c == ';' || c == '\n') {
      if (tokenName == "DEFINE") {
        tokens->push_back({ TokenType::DEFINE, 2, "" });
      } else if (tokenName == "RUN") {
        tokens->push_back({ TokenType::RUN, 2, "" });
      } else if (tokenName == "FIND") {
        tokens->push_back({ TokenType::FIND, 2, "" });
      } else if (tokenName == "CLEAR") {
        tokens->push_back({ TokenType::CLEAR, 2, "" });
      } else if (tokenName == "ALL") {
        tokens->push_back({ TokenType::ALL, 2, "" });
      } else if (tokenName != "" && tokenName != "1" && tokenName != "0") {
        tokens->push_back({ TokenType::VAR_NAME, 2, tokenName });
      } else if (tokenName == "1") {
        tokens->push_back({ TokenType::VAL, 1, "" });
      } else if (tokenName == "0") {
        tokens->push_back({ TokenType::VAL, 0, "" });
      }
      tokenName = "";
    } else {
      std::cerr << "ERROR: Unrecognized token: " << (char)c << '\n';
      exit(1);
    }
    if (c == '(') {
      tokens->push_back({ TokenType::PAREN_L, 2, "" });
    } else if (c == ')') {
      tokens->push_back({ TokenType::PAREN_R, 2, "" });
    } else if (c == ':') {
      tokens->push_back({ TokenType::COLS, 2, "" });
    } else if (c == ';') {
      tokens->push_back({ TokenType::SEMICOLS, 2, "" });
    } else if (c == '"') {
      tokens->push_back({ TokenType::QMARK, 2, "" });
    } else if (c == ',') {
      tokens->push_back({ TokenType::COMMA, 2, "" });
    } else if (c == '\n') {
      tokens->push_back({ TokenType::NEWLINE, 2, "" });
    }
  }
  return tokens;
}

// Function to test the tokenizer
static void
test_tokenizer(const std::string& fileName)
{
  std::cout << "TEST: tokenizer\n";
  FILE* infile;
  if (fileName.empty()) {
    infile = stdin; // Use stdin if no file name is provided
  } else {
    infile = fopen(fileName.c_str(), "r");
    if (infile == NULL) {
      std::cerr << "ERROR: Could not open file\n";
      exit(1);
    } else {
      std::cout << "INFO: File successfully loaded\n";
    }
  }

  auto tokens = tokenizer(infile);
  printTokens(*tokens);
  if (infile != stdin) { fclose(infile); }
  delete tokens;
}

// int
// main()
// {
//   // test_tokenizer("./examples/ic1.txt");
//   // test_tokenizer("./examples/ic2.txt");
//   // test_tokenizer("./examples/ic3.txt");
//   // test_tokenizer("./examples/find.txt");
//   test_tokenizer("./examples/findWithFile.csv");
//   // test_tokenizer("");
//   return 0;
// }
