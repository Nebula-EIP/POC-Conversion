#pragma once
#include "nebula.hpp"
namespace nebula {
class IrConverter final : public IConverter {
public:
  Result<std::string> blocksToC(const Graph &) override;
  Result<Graph> cToBlocks(std::string_view, const CompileOptions &) override;
  std::string name() const override {
    return "C-shared-ir";
  }
};
Result<std::vector<Diagnostic>> validate(const Graph &);
} // namespace nebula