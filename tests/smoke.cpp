#include "ir.hpp"
#include "templates.hpp"
#include <cassert>
#include <iostream>
using namespace nebula;
int main() {
  Graph graph;
  graph.functions.push_back(
      Function{"int",
               "add",
               {{"int", "a", std::nullopt}, {"int", "b", std::nullopt}},
               {Variable{"int", "result", Binary{"+", "a", "b"}}, Return{Reference{"result"}}}});
  auto generated = TemplatesConverter().blocksToC(graph);
  assert(generated.ok());
  assert(generated.value->find("(a + b)") != std::string::npos);
  auto imported = TemplatesConverter().cToBlocks(*generated.value, {});
  assert(imported.ok());
  auto valid = validate(graph);
  assert(valid.ok() && valid.value->empty());
  auto rejected = TemplatesConverter().cToBlocks("#define LIMIT 10", {});
  assert(!rejected.ok());
  std::cout << "nebula smoke tests passed\n";
}