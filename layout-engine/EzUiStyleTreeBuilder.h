#pragma once

#include "EzUIDocNode.h"
#include "EzUICssNode.h"
#include "EzUiStyleNode.h"



class EzUiStyleTreeBuilder {
public:
  std::unique_ptr<EzUiStyleNode> Build(EzUIDocNode* docNode, Stylesheet* stylesheet);

  PropertyMap SpecifiedValues(EzUIDocNode* elem, Stylesheet* stylesheet);

  std::vector<MatchedRule> MatchingRules(EzUIDocNode* elem, Stylesheet* stylesheet);

  std::optional<MatchedRule> MatchRule(EzUIDocNode* elem, const Rule& rule);

  bool Matches(EzUIDocNode* elem, const Selector& selector);

  bool MatchesSimpleSelector(EzUIDocNode* elem, const SimpleSelector& selector);

};

