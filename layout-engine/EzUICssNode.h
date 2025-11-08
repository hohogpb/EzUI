#pragma once

#include <variant>

// Simple selector: tag#id.class1.class2...
struct SimpleSelector {
  std::optional<std::wstring> tagName;
  std::optional<std::wstring> id;
  std::vector<std::wstring> classes;
  std::optional<std::wstring> pseudo;
};

using Selector = SimpleSelector;

struct Declaration {
  std::wstring name;
  std::vector<std::wstring> value;
};

struct Rule {
  std::vector<Selector> selectors;
  std::vector<Declaration> declarations;
};

struct Stylesheet {
  std::vector<Rule> rules;
};