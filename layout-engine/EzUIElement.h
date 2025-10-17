#pragma once

#include <gdiplus.h>
#include "yoga/Yoga.h"

using namespace Gdiplus;

struct RectF32 { float x, y, w, h; };

struct YogaAbsoluteRect {
  float left, top, width, height;
};

class UIElement {
public:
  std::wstring tag;
  std::wstring name;
  YGNodeRef ygNode;
  std::vector<UIElement*> children;
  RectF32 rect{};
  Color color{  0, 0, 0 };
  Color backgroundColor{0 , 255, 255, 255 };
  Image* backgroundImage = nullptr;
  

  UIElement(const std::wstring& n = L"") : tag(n) {
    ygNode = YGNodeNew();
    YGNodeSetContext(ygNode, this);
  }

  virtual ~UIElement() {
    for (auto c : children) delete c;
    delete backgroundImage;
    YGNodeFree(ygNode);
  }

  void AddChild(UIElement* child) {
    children.push_back(child);
    YGNodeInsertChild(ygNode, child->ygNode, YGNodeGetChildCount(ygNode));
  }

  virtual void OnRender(Graphics& g);
};