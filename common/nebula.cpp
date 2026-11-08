#include "nebula.hpp"
#include <sstream>
#include <type_traits>
namespace nebula {

namespace {
std::string pad(int indent) {
  return std::string(static_cast<size_t>(indent) * 2, ' ');
}
} // namespace

std::string expressionToC(const Expression &expression) {
  return std::visit(
      [](const auto &value) -> std::string {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, Literal>) {
          return value.value;
        } else if constexpr (std::is_same_v<T, Reference>) {
          return value.name;
        } else if constexpr (std::is_same_v<T, Binary>) {
          return "(" + value.left + " " + value.op + " " + value.right + ")";
        } else if constexpr (std::is_same_v<T, Call>) {
          std::string result = value.callee + "(";
          for (size_t i = 0; i < value.arguments.size(); ++i) {
            result += (i ? ", " : "") + value.arguments[i];
          }
          return result + ")";
        } else {
          return value.source;
        }
      },
      expression);
}

std::string statementToC(const Statement &statement, int indent) {
  return std::visit(
      [indent](const auto &value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        const auto prefix = pad(indent);

        if constexpr (std::is_same_v<T, Variable>) {
          return prefix + value.type + " " + value.name +
                 (value.initializer ? " = " + expressionToC(*value.initializer) : "") + ";\n";
        } else if constexpr (std::is_same_v<T, Assignment>) {
          return prefix + value.name + " = " + expressionToC(value.value) + ";\n";
        } else if constexpr (std::is_same_v<T, Return>) {
          return prefix + "return" + (value.value ? " " + expressionToC(*value.value) : "") + ";\n";
        } else if constexpr (std::is_same_v<T, OpaqueSource>) {
          return prefix + value.source + "\n";
        } else if constexpr (std::is_same_v<T, If>) {
          return prefix + "if (" + value.condition + ") {\n" + prefix + "}\n";
        } else if constexpr (std::is_same_v<T, While>) {
          return prefix + "while (" + value.condition + ") {\n" + prefix + "}\n";
        } else {
          return prefix + "for (" + value.initializer + "; " + value.condition + "; " +
                 value.increment + ") {\n" + prefix + "}\n";
        }
      },
      statement);
}

std::string graphToC(const Graph &graph) {
  std::string result;

  for (const auto &function : graph.functions) {
    result += function.returnType + " " + function.name + "(";
    for (size_t i = 0; i < function.parameters.size(); ++i) {
      result += (i ? ", " : "") + function.parameters[i].type + " " + function.parameters[i].name;
    }
    result += ") {\n";

    for (const auto &statement : function.body) {
      result += statementToC(statement);
    }
    result += "}\n\n";
  }

  return result;
}

std::string diagnosticsToString(const std::vector<Diagnostic> &diagnostics) {
  std::ostringstream output;
  for (const auto &diagnostic : diagnostics) {
    output << diagnostic.code << ": " << diagnostic.message << "\n";
  }
  return output.str();
}

bool structurallyEqual(const Graph &left, const Graph &right) {
  return graphToC(left) == graphToC(right);
}
} // namespace nebula