#include "parser.h"
#include "tokenizer.h"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <optional>

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
    // std::cerr << "EVALUATION ERROR: definition is malformed\n";
    // return std::nullopt;
    return 0;
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
      case OperationType::NOT: return !rightValue;
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

static void
evaluateAndPrintAll(const std::string& name,
                    const std::vector<std::string>& arguments,
                    SynTree* definition)
{
  std::vector<std::string> argNames = arguments; // Extract argument names
  size_t numArgs = argNames.size();
  std::vector<unsigned char> argumentValues(numArgs, 0);

  std::cout << "EVALUATION ALL: " << name << "\n";

  // Generate all possible combinations of values for the arguments
  bool finished = false;
  while (!finished) {
    // Print current combination of arguments
    for (size_t i = 0; i < numArgs; ++i) {
      if (i > 0) std::cout << '|';
      std::cout << static_cast<int>(argumentValues[i]);
    }

    // Evaluate the definition with current argument values
    auto resultOpt = evaluateSynTree(definition, argNames, argumentValues);

    if (resultOpt) {
      std::cout << "|" << static_cast<int>(resultOpt.value()) << "\n";
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
}

namespace find {

static std::string
constructMinterm(const std::vector<unsigned char>& row, size_t N)
{
  std::string minterm;
  for (size_t i = 0; i < N; ++i) {
    if (row[i] == 0) { minterm += "!"; }
    minterm += 'a' + i;
    if (i < N - 1) { minterm += " & "; }
  }
  return minterm;
}

static std::pair<std::string, std::vector<std::string>>
getBooleanExpression(const Table& table)
{
  std::vector<std::string> minterms;
  std::vector<std::string> variables;

  // Populate the variable names
  for (size_t i = 0; i < table.N; ++i) {
    variables.push_back(std::string(1, 'a' + i));
  }

  // Construct minterms
  for (size_t i = 0; i < table.M; ++i) {
    if (table.output[i] == 1) {
      std::vector<unsigned char> row(table.input.begin() + i * table.N,
                                     table.input.begin() + (i + 1) * table.N);
      minterms.push_back(constructMinterm(row, table.N));
    }
  }

  // Combine minterms into an expression
  std::string expression;
  for (size_t i = 0; i < minterms.size(); ++i) {
    expression += "(" + minterms[i] + ")";
    if (i < minterms.size() - 1) { expression += " | "; }
  }

  // Return the expression and the variable names
  return std::make_pair(expression, variables);
}

}

void
interpreter(Command command)
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

      if (programNameSpace.size() == 0) {
        std::cout << "RUNTIME ERROR: could not find definition for " << name
                  << " in scope\n";
        return;
      }

      for (auto i : programNameSpace)
        if (i.name == name) {
          definition = i.definition;
          arguments = i.argNames;
          break;
        }

      // printSyntaxTree(definition);

      if (arguments.size() != values.size()) {
        std::cerr << "SYNTAX ERROR: incomplete RUN command definition\n";
        return;
      }

      if (definition == nullptr) {
        std::cerr << "EVALUATION ERROR: function " << name << " undefined\n";
        return;
      }

      std::optional<unsigned char> answer =
        evaluateSynTree(definition, arguments, values);

      if (answer.has_value())
        std::cout << "EVALUATION RUN: " << static_cast<int>(answer.value())
                  << '\n';

      return;
    }

    case CommandType::CLEAR: {
      programNameSpace.clear();
      programNameSpace.resize(0);
      for (auto i : programNameSpace) {
        if (i.definition != nullptr) i.definition->destroy();
      }
      return;
    }

    case CommandType::ALL: {
      std::string name = command.name;
      std::vector<std::string> arguments;
      SynTree* definition;

      if (programNameSpace.size() == 0) {
        std::cout << "RUNTIME ERROR: could not find definition for " << name
                  << " in scope\n";
        return;
      }
      for (auto i : programNameSpace) {
        if (i.name == name) {
          arguments = i.argNames;
          definition = i.definition;
          break;
        }
      }

      if (arguments.size() == 0) {
        std::cerr << "EVALUATION ERROR: function " << name << " undefined\n";
        return;
      }

      evaluateAndPrintAll(name, arguments, definition);
      return;
    }

    case CommandType::TRIVIAL: return;

    case CommandType::EXIT: exit(0);

    case CommandType::FIND: {
      std::string functionName;
      { // Gen a random function name
        size_t length = (rand() % 9) + 1;
        const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
        const size_t alphabetSize = sizeof(alphabet) - 1;

        for (size_t i = 0; i < length; ++i) {
          functionName += alphabet[rand() % alphabetSize];
        }
      }
      auto boolExpr = find::getBooleanExpression(command.table);
      std::string rawDef = "DEFINE " + functionName + "(";
      for (size_t i = 0; i < boolExpr.second.size() - 1; ++i) {
        rawDef += boolExpr.second[i] + ", ";
      }
      rawDef += boolExpr.second[boolExpr.second.size() - 1];
      rawDef += "): ";
      rawDef += "\"" + boolExpr.first + "\"\n";
      std::string filename = "." + functionName + ".txt";

      { // save definition
        FILE* outFile = fopen(filename.c_str(), "w");
        if (outFile) {
          fprintf(outFile, "%s", rawDef.c_str());
          fclose(outFile);
        } else {
          std::cerr << "Unable to open file";
        }
      }

      { // read file with command
        FILE* infile;
        infile = fopen(filename.c_str(), "r");
        // infile = fopen("/dev/stdin", "r"); // Open stdin for reading
        if (infile == NULL) {
          std::cerr << "ERROR: Could not open file\n";
          exit(1);
        } else {
          std::cout << "INFO: File successfully loaded\n";
        }

        std::vector<Token>* tokens = tokenizer(infile);
        auto command = parser(0, tokens);
        interpreter(command.second);
      }
      std::cout << "EVALUATION FIND: formula found: " << boolExpr.first << ' '
                << " with name: " << functionName << '\n';
    };
  }
}
