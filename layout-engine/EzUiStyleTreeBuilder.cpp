#include "pch.h"
#include "EzUiStyleTreeBuilder.h"


std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::BuildTree(EzUIDocNode* docNode, Stylesheet* stylesheet) {
  EzUiStyleTreeBuilder builder;
  auto styledNode = builder.Build(docNode, stylesheet);
  return styledNode;
}

std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::Build(EzUIDocNode* docNode, Stylesheet* stylesheet) {
  auto styled = std::make_unique<EzUiStyledNode>();
  styled->node = docNode;

  if (docNode) {
    // ? 首先添加 HTML 属性（最低优先级）
    for (const auto& [attrName, attrValue] : docNode->attributes) {
      styled->specifiedValues[attrName] = attrValue;
    }

    // 解析正常状态的样式（CSS 规则会覆盖 HTML 属性）
    PropertyMap cssValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Normal);
    for (const auto& [propName, propValue] : cssValues) {
      styled->specifiedValues[propName] = propValue;
    }
    
    // 解析 :hover 伪类样式
    styled->hoverValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Hover);
    
    // 解析 :focus 伪类样式
    styled->focusValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Focus);
  }

  for (auto& child : docNode->children) {
    styled->children.push_back(Build(child.get(), stylesheet));
  }

  return styled;
}

PropertyMap EzUiStyleTreeBuilder::SpecifiedValues(EzUIDocNode* elem, Stylesheet* stylesheet, PseudoClassState state) {
  PropertyMap values;
  auto rules = MatchingRules(elem, stylesheet, state);
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

std::vector<MatchedRule> EzUiStyleTreeBuilder::MatchingRules(EzUIDocNode* elem, Stylesheet* stylesheet, PseudoClassState state) {
  std::vector<MatchedRule> result;
  for (auto& rule : stylesheet->rules) {
    auto matched = MatchRule(elem, rule, state);
    if (matched)
      result.push_back(*matched);
  }
  return result;
}

std::optional<MatchedRule> EzUiStyleTreeBuilder::MatchRule(EzUIDocNode* elem, const Rule& rule, PseudoClassState state) {
  for (auto& sel : rule.selectors) {
    if (Matches(elem, sel, state)) {
      MatchedRule aRule;
      aRule.first = sel.Specificity();
      aRule.second = const_cast<Rule*>(&rule);
      return aRule;
    }
  }
  return std::nullopt;
}

bool EzUiStyleTreeBuilder::Matches(EzUIDocNode* elem, const Selector& selector, PseudoClassState state) {
  return MatchesSimpleSelector(elem, selector, state);
}

bool EzUiStyleTreeBuilder::MatchesSimpleSelector(EzUIDocNode* elem, const SimpleSelector& selector, PseudoClassState state) {
  if (selector.tagName && elem->name != *selector.tagName)
    return false;

  if (selector.id && (!elem->GetId().has_value() || elem->GetId().value() != *selector.id))
    return false;

  for (auto& cls : selector.GetClassNames()) {
    if (!elem->HasClass(cls))
      return false;
  }

  // 检查伪类
  for (auto& pseudoClass : selector.GetPseudoClasses()) {
    if (!MatchesPseudoClass(pseudoClass, state))
      return false;
  }

  return true;
}

bool EzUiStyleTreeBuilder::MatchesPseudoClass(const std::wstring& pseudoClass, PseudoClassState state) {
  if (pseudoClass == L"hover") {
    return state == PseudoClassState::Hover;
  } else if (pseudoClass == L"focus") {
    return state == PseudoClassState::Focus;
  }
  
  // 正常状态总是匹配（不使用伪类的选择器）
  if (state == PseudoClassState::Normal) {
    return true;
  }
  
  return false;
}