#include "parser.h"
#include "tokenizer.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#if 0
  g++ -g -o interperate interperate.cpp tokenizer.cpp parser.cpp && ./interperate && rm ./interperate && exit
;
#endif // 0

struct FunctionDefinition
{
  std::string name;
  std::vector<std::string> argNames;
  SynTree* definition;
};
std::vector<FunctionDefinition> programNameSpace;

std::optional<unsigned char>
evaluateSynTree(SynTree* node,
                const std::vector<std::string>& argNames,
                const std::vector<unsigned char>& values)
{
  if (!node) {

    std::cerr << "EVALUATION ERROR: definition is malformed\n";
    return std::nullopt;
  }

  if (node->val.type == AlgebraType::VALUE) {
    return node->val.value;

  } else if (node->val.type == AlgebraType::OPERATION) {
    auto leftValueOpt = evaluateSynTree(node->left, argNames, values);
    auto rightValueOpt = evaluateSynTree(node->right, argNames, values);

    if (!leftValueOpt || !rightValueOpt) return std::nullopt;

    unsigned char leftValue = leftValueOpt.value();
    unsigned char rightValue = rightValueOpt.value();

    switch (node->val.operation) {
      case OperationType::AND: return leftValue & rightValue;
      case OperationType::OR: return leftValue | rightValue;
      case OperationType::NOT: return !leftValue;
      default: return std::nullopt;
    }

  } else if (node->val.type == AlgebraType::VARIABLE) {
    for (size_t i = 0; i < argNames.size(); ++i) {
      if (argNames[i] == node->val.variable) { return values[i]; }
    }
    std::cerr << "EVALUATION ERROR: Variable " << node->val.variable
              << " not found.\n";
    return std::nullopt;
  }

  return std::nullopt;
}

void
evaluateAndPrintAll(const std::string& name,
                    const std::vector<std::string>& arguments,
                    SynTree* definition)
{
  std::vector<std::string> argNames = arguments; // Extract argument names
  size_t numArgs = argNames.size();
  std::vector<unsigned char> argumentValues(numArgs, 0);

  std::cout << "Evaluating command: " << name << "\n";

  // Generate all possible combinations of values for the arguments
  bool finished = false;
  while (!finished) {
    // Print current combination of arguments
    std::cout << "Arguments: (";
    for (size_t i = 0; i < numArgs; ++i) {
      if (i > 0) std::cout << ", ";
      std::cout << argNames[i] << "=" << static_cast<int>(argumentValues[i]);
    }
    std::cout << ")\n";

    // Evaluate the definition with current argument values
    auto resultOpt = evaluateSynTree(definition, argNames, argumentValues);

    if (resultOpt) {
      std::cout << "Result: " << static_cast<int>(resultOpt.value()) << "\n";
    } else {
      std::cerr << "Evaluation failed for arguments: (";
      for (size_t i = 0; i < numArgs; ++i) {
        if (i > 0) std::cerr << ", ";
        std::cerr << argNames[i] << "=" << static_cast<int>(argumentValues[i]);
      }
      std::cerr << ")\n";
    }

    {
      int i;
      for (i = argumentValues.size() - 1; i >= 0; --i) {
        if (argumentValues[i] == 0) {
          argumentValues[i] = 1;
          break;
        } else {
          argumentValues[i] = 0;
        }
      }

      // If all elements are 1, break the loop
      if (i < 0) { break; }
    }
  }

  std::cout << "Evaluation complete.\n";
}

void
interperate(Command command)
{
  switch (command.type) {

    case CommandType::DEFINE: {
      FunctionDefinition def{ .name = command.name,
                              .argNames = command.arguments,
                              .definition = command.definition };
      programNameSpace.push_back(def);
      return;
    } // END DEFINE

    case CommandType::RUN: {
      // return;
      std::vector<std::string> arguments;
      std::string name = command.name;
      std::vector<unsigned char> values = command.values;
      SynTree* definition;

      for (auto i : programNameSpace)
        if (i.name == name) {
          definition = i.definition;
          arguments = i.argNames;
        }

      if (definition == nullptr) {
        std::cerr << "EVALUATION ERROR: function " << name << " undefined\n";
      }

      std::optional<unsigned char> answer =
        evaluateSynTree(definition, arguments, values);

      if (answer.has_value())
        std::cout << "RESULT: " << (size_t)answer.value() << '\n';

      return;
    }

    case CommandType::CLEAR: {
      programNameSpace.clear();
      programNameSpace.resize(0);
      return;
    }

    case CommandType::ALL: {
      std::string name = command.name;
      std::vector<std::string> arguments;
      SynTree* definition;

      for (auto i : programNameSpace) {
        if (i.name == name) {
          arguments = i.argNames;
          definition = i.definition;
        }
      }

      if (arguments.size() == 0) {
        std::cerr << "EVALUATION ERROR: function " << name << " undefined\n";
      }

      evaluateAndPrintAll(name, arguments, definition);
      return;
    }

    case CommandType::TRIVIAL: return;

    case CommandType::FIND: return;
  }
}

int
main()
{
  std::string fileName = "./examples/ic1.txt";
  FILE* infile;
  infile = fopen(fileName.c_str(), "r");
  if (infile == NULL) {
    std::cerr << "ERROR: Could not open file\n";
    exit(1);
  } else {
    std::cout << "INFO: File successfully loaded\n";
  }

  std::vector<Token>* tokens = tokenizer(infile);
  auto command = parser(0, tokens);

  interperate(command.second);
  // printSyntaxTree(programNameSpace.at(0).definition);

  std::cout << command.first << '\n';
  // printTokens(*tokens);
  command = parser(command.first, tokens);
  command = parser(command.first, tokens);
  interperate(command.second);

  fclose(infile);
  delete tokens;
  return 0;
}
