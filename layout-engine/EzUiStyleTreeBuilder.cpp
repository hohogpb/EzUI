#include "pch.h"
#include "EzUiStyleTreeBuilder.h"


std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::BuildTree(EzUIDocNode* docNode, Stylesheet* stylesheet) {
  EzUiStyleTreeBuilder builder;
  return builder.Build(docNode, stylesheet);
}

std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::Build(EzUIDocNode* docNode, Stylesheet* stylesheet) {

  auto styled = std::make_unique<EzUiStyledNode>();
  styled->node = docNode;

  //if (docNode->type == NodeType::Element && root.element) {
  if (docNode) {
    styled->specifiedValues = SpecifiedValues(docNode, stylesheet);
  }

  for (auto& child : docNode->children) {
    styled->children.push_back(Build(child.get(), stylesheet));
  }

  return styled;
}

PropertyMap EzUiStyleTreeBuilder::SpecifiedValues(EzUIDocNode* elem, Stylesheet* stylesheet) {
  PropertyMap values;
  auto rules = MatchingRules(elem, stylesheet);
  std::sort(rules.begin(), rules.end(),
    [](const MatchedRule& a, const MatchedRule& b) {
    return a.first < b.first;
  });

  for (auto& [spec, rule] : rules) {
    for (auto& decl : rule->declarations) {
      std::wstring notEnoughValue;
      if (!decl.value.empty())
        notEnoughValue = decl.value[0];

      values[decl.name] = notEnoughValue;
    }
  }
  return values;
}

std::vector<MatchedRule> EzUiStyleTreeBuilder::MatchingRules(EzUIDocNode* elem, Stylesheet* stylesheet) {
  std::vector<MatchedRule> result;
  for (auto& rule : stylesheet->rules) {
    auto matched = MatchRule(elem, rule);
    if (matched)
      result.push_back(*matched);
  }
  return result;
}

std::optional<MatchedRule> EzUiStyleTreeBuilder::MatchRule(EzUIDocNode* elem, const Rule& rule) {
  for (auto& sel : rule.selectors) {
    if (Matches(elem, sel)) {
      MatchedRule aRule;
      aRule.first = sel.Specificity();
      aRule.second = const_cast<Rule*>(&rule);
      return aRule;
    }
  }
  return std::nullopt;
}

bool EzUiStyleTreeBuilder::Matches(EzUIDocNode* elem, const Selector& selector) {
  // if (selector.type == Selector::Type::Simple)
  return MatchesSimpleSelector(elem, selector);
  // return false;
}

bool EzUiStyleTreeBuilder::MatchesSimpleSelector(EzUIDocNode* elem, const SimpleSelector& selector) {
  if (selector.tagName && elem->name != *selector.tagName)
    return false;

  if (selector.id && (!elem->GetId().has_value() || elem->GetId().value() != *selector.id))
    return false;

  for (auto& cls : selector.GetClassNames()) {
    if (!elem->HasClass(cls))
      return false;
  }

  return true;
}