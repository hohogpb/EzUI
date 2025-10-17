#include "pch.h"
#include "EzUIElement.h"

YogaAbsoluteRect GetAbsoluteRect(YGNodeRef node) {
  float left = YGNodeLayoutGetLeft(node);
  float top = YGNodeLayoutGetTop(node);
  float width = YGNodeLayoutGetWidth(node);
  float height = YGNodeLayoutGetHeight(node);

  YGNodeRef parent = YGNodeGetOwner(node);

  while (parent) {
    YGPositionType posType = YGNodeStyleGetPositionType(parent);
    // 绝对定位的父节点就停止累加（它相对根或自身坐标系）
    if (posType == YGPositionTypeAbsolute)
      break;

    left += YGNodeLayoutGetLeft(parent);
    top += YGNodeLayoutGetTop(parent);
    parent = YGNodeGetOwner(parent);
  }

  return { left, top, width, height };
}

void UIElement::OnRender(Graphics& g) {
  YogaAbsoluteRect rect = GetAbsoluteRect(ygNode);

  // Create a SolidBrush for the background color.
  SolidBrush backBrush(backgroundColor);
  g.FillRectangle(&backBrush, rect.left, rect.top, rect.width, rect.height);

  // Draw the background image if it exists.
  if (backgroundImage) {
    g.DrawImage(backgroundImage, rect.left, rect.top, rect.width, rect.height);
  }     

  for (auto c : children)
    c->OnRender(g);
}
