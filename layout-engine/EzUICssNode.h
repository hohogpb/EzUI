#pragma once

#include <variant>

// Simple selector: tag#id.class1.class2...
struct SimpleSelector {
  std::optional<std::wstring> tagName;
  std::optional<std::wstring> id;
  std::vector<std::wstring> classNames;
  std::optional<std::wstring> pseudo;

  std::tuple<int, int, int> Specificity() const {
    int a = id.has_value() ? 1 : 0;                // id 选择器数量
    int b = classNames.size();                     // class 数量
    int c = tagName.has_value() ? 1 : 0;           // tag 数量
    return { a, b, c };
  }

  std::vector<std::wstring> GetClassNames() const {
    return classNames;
  }
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


struct AtRule {
  std::wstring name;
  std::wstring prelude;
  // ...;
};

struct Stylesheet {
  std::vector<Rule> rules;
  std::vector<AtRule> atRules;
};