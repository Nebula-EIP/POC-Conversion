#pragma once
#include "nebula.hpp"
namespace nebula {
class ClangConverter final : public IConverter {
public:
  Result<std::string> blocksToC(const Graph &) override;
  Result<Graph> cToBlocks(std::string_view, const CompileOptions &) override;
  std::string name() const override {
    return "B-clang-ast";
  }
};
} // namespace nebula