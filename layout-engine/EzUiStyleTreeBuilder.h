#pragma once

#include "EzUIDocNode.h"
#include "EzUICssNode.h"
#include "EzUiStyledNode.h"



class EzUiStyleTreeBuilder {
public:
  static std::unique_ptr<EzUiStyledNode> BuildTree(EzUIDocNode* docNode, Stylesheet* stylesheet);

  std::unique_ptr<EzUiStyledNode> Build(EzUIDocNode* docNode, Stylesheet* stylesheet);

  PropertyMap SpecifiedValues(EzUIDocNode* elem, Stylesheet* stylesheet);

  std::vector<MatchedRule> MatchingRules(EzUIDocNode* elem, Stylesheet* stylesheet);

  std::optional<MatchedRule> MatchRule(EzUIDocNode* elem, const Rule& rule);

  bool Matches(EzUIDocNode* elem, const Selector& selector);

  bool MatchesSimpleSelector(EzUIDocNode* elem, const SimpleSelector& selector);

};

