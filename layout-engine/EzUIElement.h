#pragma once

#include <gdiplus.h>
#include "yoga/Yoga.h"
#include "EzUIHelper.h"

using Microsoft::WRL::ComPtr;
using std::weak_ptr;
using std::unique_ptr;
using std::vector;
using std::shared_ptr;

class UIElement {
public:
  std::wstring tag;
  std::wstring name;
  std::wstring docText;
  std::wstring text;

  EzUI::RectF rect;

  EzUI::Color color{ 0, 0, 0 };
  EzUI::Color bgColor{ 255 , 255, 255, 0 };

  Gdiplus::Image* bgImage = nullptr;

  int zDepth;

  YGNodeRef ygNode;

  UIElement* parent;
  vector<UIElement*> children;

  UIElement(const std::wstring& n = L"") : tag(n) {
    ygNode = YGNodeNew();
    YGNodeSetContext(ygNode, this);
  }

  virtual ~UIElement() {
    for (auto child : children) {
      delete child;
    }
    delete bgImage;
    YGNodeFree(ygNode);
  }

  void AddChild(UIElement* child) {
    child->parent = this;   // ÉèÖÃ parent
    children.push_back(child);
    YGNodeInsertChild(ygNode, child->ygNode, YGNodeGetChildCount(ygNode));
  }

  // virtual void OnRender(Graphics& g);

  virtual void OnRenderD2D(ID2D1HwndRenderTarget* rt);

  virtual void DrawSvg(ID2D1HwndRenderTarget* rt, EzUI::RectF rect);

  virtual IDWriteTextFormat* GetTextFormat();

  virtual IDWriteTextLayout* GetTextLayout();

  virtual IDWriteTextLayout* UpdateTextLayout(float maxWidth, float maxHeight);

  YGSize MesureText(float maxWidth, float maxHeight);

  static EzUI::RectF GetAbsoluteRect(YGNodeRef node);

  void OnMouseLeave();
  void OnMouseEnter();
protected:
  ComPtr<IDWriteTextFormat> mTextFormat;
  ComPtr<IDWriteTextLayout> mTextLayout;

};