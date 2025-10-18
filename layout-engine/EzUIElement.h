#pragma once

#include <gdiplus.h>
#include "yoga/Yoga.h"

using namespace Gdiplus;
using Microsoft::WRL::ComPtr;

struct RectF32 { float x, y, w, h; };

struct YogaAbsoluteRect {
  float left, top, width, height;
};

class UIElement {
public:
  std::wstring tag;
  std::wstring name;
  std::wstring docText;
  std::wstring text;

  YGNodeRef ygNode;
  std::vector<UIElement*> children;
  RectF32 rect{};
  Color color{ 0, 0, 0 };
  Color backgroundColor{ 0 , 255, 255, 255 };
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

  virtual void OnRenderD2D(ID2D1HwndRenderTarget* rt);

  virtual void DrawSvg(ID2D1HwndRenderTarget* rt, YogaAbsoluteRect rect);

  virtual IDWriteTextFormat* GetTextFormat();

  virtual IDWriteTextLayout* GetTextLayout();

  virtual IDWriteTextLayout* UpdateTextLayout(float maxWidth, float maxHeight);

  YGSize MesureText(float maxWidth, float maxHeight);

protected:
  ComPtr<IDWriteTextFormat> mTextFormat;
  ComPtr<IDWriteTextLayout> mTextLayout;

};