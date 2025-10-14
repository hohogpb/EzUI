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

  RECT r = {
    (LONG)rect.left,
    (LONG)rect.top,
    (LONG)(rect.left + rect.width),
    (LONG)(rect.top + rect.height)
  };

  SolidBrush brush(color);
  g.FillRectangle(&brush, rect.left, rect.top, rect.width, rect.height);

  for (auto c : children)
    c->OnRender(g);
}
