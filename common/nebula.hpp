#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace nebula {

struct Diagnostic {
  enum class Level { Info, Warning, Error };

  Level level;
  std::string code;
  std::string message;
};

template <class T> struct Result {
  std::optional<T> value;
  std::vector<Diagnostic> diagnostics;

  bool ok() const {
    return value.has_value();
  }
};

struct CompileOptions {
  std::string standard = "c11";
  std::vector<std::string> arguments;
};

struct Literal {
  std::string value;
};

struct Reference {
  std::string name;
};

struct Binary {
  std::string op;
  std::string left;
  std::string right;
};

struct Call {
  std::string callee;
  std::vector<std::string> arguments;
};

struct OpaqueSource {
  std::string source;
};

using Expression = std::variant<Literal, Reference, Binary, Call, OpaqueSource>;

struct Variable {
  std::string type;
  std::string name;
  std::optional<Expression> initializer;
};

struct Return {
  std::optional<Expression> value;
};

struct Assignment {
  std::string name;
  Expression value;
};

struct If {
  std::string condition;
  std::vector<std::string> thenBody;
  std::vector<std::string> elseBody;
};

struct While {
  std::string condition;
  std::vector<std::string> body;
};

struct For {
  std::string initializer;
  std::string condition;
  std::string increment;
  std::vector<std::string> body;
};

using Statement = std::variant<Variable, Assignment, Return, If, While, For, OpaqueSource>;

struct Function {
  std::string returnType = "int";
  std::string name;
  std::vector<Variable> parameters;
  std::vector<Statement> body;
};

struct Graph {
  std::vector<Function> functions;
};

class IConverter {
public:
  virtual ~IConverter() = default;
  virtual Result<std::string> blocksToC(const Graph &) = 0;
  virtual Result<Graph> cToBlocks(std::string_view, const CompileOptions &) = 0;
  virtual std::string name() const = 0;
};

std::string diagnosticsToString(const std::vector<Diagnostic> &);
std::string expressionToC(const Expression &);
std::string statementToC(const Statement &, int indent = 1);
std::string graphToC(const Graph &);
bool structurallyEqual(const Graph &, const Graph &);
} // namespace nebula