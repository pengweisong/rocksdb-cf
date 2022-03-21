#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include "VertexEdge.h"

std::string kDBPath = "/tmp/different_column_family_test";

int main() {
  Edge e;
  e.type = 1;
  e.src = 2;
  e.dst = 3;
  e.rank = 100;
  e.version = 0;

  auto s = e.toString();
  auto e1 = Edge::fromString(s);

  assert(e1.type == e.type);
  assert(e1.src == e.src);
  assert(e1.dst == e.dst);
  assert(e1.rank == e.rank);
  assert(e1.version == e.version);

  std::cout << e1.rank << std::endl;

  return 0;
}