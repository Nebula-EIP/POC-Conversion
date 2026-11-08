#include "ir.hpp"
#include "templates.hpp"
#include <unordered_set>
namespace nebula {
Result<std::vector<Diagnostic>> validate(const Graph &graph) {
  Result<std::vector<Diagnostic>> result;
  std::vector<Diagnostic> errors;
  std::unordered_set<std::string> functions;
  for (const auto &function : graph.functions) {
    if (!functions.insert(function.name).second)
      errors.push_back({Diagnostic::Level::Error, "IR_DUPLICATE_FUNCTION",
                        "Fonction dupliquée: " + function.name});
    std::unordered_set<std::string> symbols;
    for (const auto &parameter : function.parameters)
      if (!symbols.insert(parameter.name).second)
        errors.push_back({Diagnostic::Level::Error, "IR_DUPLICATE_SYMBOL",
                          "Paramètre dupliqué: " + parameter.name});
    for (const auto &statement : function.body)
      if (const auto *variable = std::get_if<Variable>(&statement))
        if (!symbols.insert(variable->name).second)
          errors.push_back({Diagnostic::Level::Error, "IR_DUPLICATE_SYMBOL",
                            "Variable dupliquée: " + variable->name});
  }
  result.value = errors;
  result.diagnostics = errors;
  return result;
}
Result<std::string> IrConverter::blocksToC(const Graph &graph) {
  auto checked = validate(graph);
  if (!checked.value || !checked.value->empty())
    return {std::nullopt, checked.diagnostics};
  return {graphToC(graph), {}};
}
Result<Graph> IrConverter::cToBlocks(std::string_view source, const CompileOptions &options) {
  return TemplatesConverter().cToBlocks(source, options);
}
} // namespace nebula