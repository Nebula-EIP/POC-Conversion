#include "clang_adapter.hpp"
#include <memory>
#include <utility>
#ifdef NEBULA_HAS_CLANG
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#endif
namespace nebula {
#ifdef NEBULA_HAS_CLANG
namespace {
class Visitor final : public clang::RecursiveASTVisitor<Visitor> {
public:
  explicit Visitor(Graph &graph) : graph_(graph) {}
  bool VisitFunctionDecl(clang::FunctionDecl *declaration) {
    if (!declaration->isThisDeclarationADefinition() || !declaration->getIdentifier())
      return true;
    Function function;
    function.name = declaration->getNameAsString();
    function.returnType = declaration->getReturnType().getAsString();
    for (const auto *parameter : declaration->parameters())
      function.parameters.push_back(
          {parameter->getType().getAsString(), parameter->getNameAsString(), std::nullopt});
    graph_.functions.push_back(std::move(function));
    return true;
  }

private:
  Graph &graph_;
};
class Consumer final : public clang::ASTConsumer {
public:
  explicit Consumer(Graph &graph) : visitor_(graph) {}
  void HandleTranslationUnit(clang::ASTContext &context) override {
    visitor_.TraverseDecl(context.getTranslationUnitDecl());
  }

private:
  Visitor visitor_;
};
class Action final : public clang::ASTFrontendAction {
public:
  explicit Action(Graph &graph) : graph_(graph) {}
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &,
                                                        llvm::StringRef) override {
    return std::make_unique<Consumer>(graph_);
  }

private:
  Graph &graph_;
};
} // namespace
#endif
Result<std::string> ClangConverter::blocksToC(const Graph &graph) {
  return {graphToC(graph),
          {{Diagnostic::Level::Info, "B_GENERATOR",
            "Generation depuis le modele Nebula; Clang ne construit pas l AST "
            "depuis zero."}}};
}
Result<Graph> ClangConverter::cToBlocks(std::string_view source, const CompileOptions &options) {
#ifdef NEBULA_HAS_CLANG
  Graph graph;
  std::vector<std::string> args = {"-std=" + options.standard};
  args.insert(args.end(), options.arguments.begin(), options.arguments.end());
  if (!clang::tooling::runToolOnCodeWithArgs(std::make_unique<Action>(graph), std::string(source),
                                             args, "input.c"))
    return {std::nullopt,
            {{Diagnostic::Level::Error, "B_PARSE", "Clang n a pas pu analyser le source C."}}};
  return {graph, {}};
#else
  return {std::nullopt,
          {{Diagnostic::Level::Error, "B_CLANG_UNAVAILABLE",
            "POC B requiert LLVM/Clang 17+; configurez CMAKE_PREFIX_PATH vers "
            "LLVMConfig.cmake."}}};
#endif
}
} // namespace nebula