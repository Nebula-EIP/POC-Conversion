#include "clang_adapter.hpp"
#include "ir.hpp"
#include "templates.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
using namespace nebula;
int main(int argc, char **argv) {
  std::string source = "int add(int a, int b) { int result = a + b; return result; }";
  if (argc > 1) {
    std::ifstream input(argv[1]);
    source.assign(std::istreambuf_iterator<char>(input), {});
  }
  Graph graph;
  Function add{"int",
               "add",
               {{"int", "a", std::nullopt}, {"int", "b", std::nullopt}},
               {Variable{"int", "result", Binary{"+", "a", "b"}}, Return{Reference{"result"}}}};
  graph.functions.push_back(add);
  std::vector<std::unique_ptr<IConverter>> converters;
  converters.push_back(std::make_unique<TemplatesConverter>());
  converters.push_back(std::make_unique<ClangConverter>());
  converters.push_back(std::make_unique<IrConverter>());
  const int repetitions = argc > 2 ? std::max(1, std::atoi(argv[2])) : 31;
  std::cout << "converter,repetitions,median_ms,p95_ms,status\n";
  for (auto &converter : converters) {
    std::vector<double> samples;
    Result<std::string> generated;
    for (int i = 0; i < repetitions; ++i) {
      const auto start = std::chrono::steady_clock::now();
      generated = converter->blocksToC(graph);
      samples.push_back(
          std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start)
              .count());
    }
    std::sort(samples.begin(), samples.end());
    const auto percentile = [&](double p) {
      return samples[static_cast<size_t>(p * static_cast<double>(samples.size() - 1))];
    };
    std::cout << converter->name() << ',' << repetitions << ',' << percentile(0.50) << ','
              << percentile(0.95) << ',' << (generated.ok() ? "ok" : "error") << "\n";
  }
  auto imported = TemplatesConverter().cToBlocks(source, {});
  std::cout << "A import: " << (imported.ok() ? "ok" : diagnosticsToString(imported.diagnostics));
}