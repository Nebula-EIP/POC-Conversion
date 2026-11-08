#include "templates.hpp"
#include <regex>
namespace nebula {
Result<std::string> TemplatesConverter::blocksToC(const Graph &graph) {
  return {graphToC(graph), {}};
}

Result<Graph> TemplatesConverter::cToBlocks(std::string_view source, const CompileOptions &) {
  Result<Graph> result;
  const std::string text(source);

  if (text.find("#define") != std::string::npos || text.find("switch") != std::string::npos ||
      text.find("goto") != std::string::npos) {
    result.diagnostics.push_back(
        {Diagnostic::Level::Error, "A_UNSUPPORTED", "Construction hors du sous-ensemble POC A"});
    return result;
  }

  const std::regex functionPattern(R"((\w+)\s+(\w+)\s*\(([^)]*)\)\s*\{([\s\S]*)\})");
  std::smatch functionMatch;
  if (!std::regex_search(text, functionMatch, functionPattern)) {
    result.diagnostics.push_back({Diagnostic::Level::Error, "A_PARSE", "Fonction C non reconnue"});
    return result;
  }

  Function function{functionMatch[1], functionMatch[2]};
  const std::string parameters = functionMatch[3].str();
  const std::regex parameterPattern(R"((\w+)\s+(\w+))");
  for (auto iterator = std::sregex_iterator(parameters.begin(), parameters.end(), parameterPattern);
       iterator != std::sregex_iterator(); ++iterator) {
    function.parameters.push_back({(*iterator)[1], (*iterator)[2], std::nullopt});
  }

  const std::string body = functionMatch[4].str();
  const std::regex declarationPattern(R"((\w+)\s+(\w+)\s*=\s*([^;]+);)");
  for (auto iterator = std::sregex_iterator(body.begin(), body.end(), declarationPattern);
       iterator != std::sregex_iterator(); ++iterator) {
    function.body.push_back(Variable{(*iterator)[1], (*iterator)[2], Literal{(*iterator)[3]}});
  }

  const std::regex returnPattern(R"(return\s+([^;]+);)");
  std::smatch returnMatch;
  if (std::regex_search(body, returnMatch, returnPattern)) {
    function.body.push_back(Return{Literal{returnMatch[1]}});
  }

  result.value = Graph{{function}};
  return result;
}
} // namespace nebula