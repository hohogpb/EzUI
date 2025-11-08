#pragma once

#include "EzUIDocNode.h"
#include "EzUICssNode.h"
#include <ostream>

// Map from CSS property names to values.
using PropertyMap = std::unordered_map<std::wstring, std::wstring>;

struct EzUiStyleNode {
  EzUIDocNode* node;
  PropertyMap specifiedValues;
  std::vector<std::unique_ptr<EzUiStyleNode>> children;
};


using MatchedRule = std::pair<std::tuple<int, int, int>, Rule*>;