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
    // ���Զ�λ�ĸ��ڵ��ֹͣ�ۼӣ�����Ը�����������ϵ��
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
