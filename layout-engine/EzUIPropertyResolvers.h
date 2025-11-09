#pragma once

#include "EzUIDocNode.h"
#include "EzUiLayoutBox.h"

class EzUIPropertyResolvers {
public:
  void Resolve(const std::wstring& keyRaw, const std::wstring& valRaw, EzUiStyledNode* uiNode, YGNodeRef ygNode);
};

