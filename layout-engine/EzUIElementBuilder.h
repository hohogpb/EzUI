#pragma once

#include "EzUIDocNode.h"
#include "EzUIElement.h"

class EzUIElementBuilder {
public:
  UIElement* Build(EzUIDocNode* docNode);
};

