#pragma once

#include "EzUIDocNode.h"
#include "EzUIElement.h"

class EzUIPropertyResolvers {
public:
  void Resolve(const std::wstring& keyRaw, const std::wstring& valRaw, UIElement* uiNode, YGNodeRef ygNode);
};

