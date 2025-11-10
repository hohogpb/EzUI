#pragma once

#include "EzUIDocNode.h"
#include "EzUICssNode.h"
#include "EzUiStyledNode.h"

// Œ±¿‡◊¥Ã¨√∂æŸ
enum class PseudoClassState {
  Normal = 0,
  Hover = 1,
  Focus = 2,
};

class EzUiStyleTreeBuilder {
public:
  static std::unique_ptr<EzUiStyledNode> BuildTree(EzUIDocNode* docNode, Stylesheet* stylesheet);

  std::unique_ptr<EzUiStyledNode> Build(EzUIDocNode* docNode, Stylesheet* stylesheet);

  PropertyMap SpecifiedValues(EzUIDocNode* elem, Stylesheet* stylesheet, PseudoClassState state = PseudoClassState::Normal);

  std::vector<MatchedRule> MatchingRules(EzUIDocNode* elem, Stylesheet* stylesheet, PseudoClassState state = PseudoClassState::Normal);

  std::optional<MatchedRule> MatchRule(EzUIDocNode* elem, const Rule& rule, PseudoClassState state = PseudoClassState::Normal);

  bool Matches(EzUIDocNode* elem, const Selector& selector, PseudoClassState state = PseudoClassState::Normal);

  bool MatchesSimpleSelector(EzUIDocNode* elem, const SimpleSelector& selector, PseudoClassState state = PseudoClassState::Normal);

  // ºÏ≤ÈŒ±¿‡ «∑Ò∆•≈‰
  bool MatchesPseudoClass(const std::wstring& pseudoClass, PseudoClassState state);
};

