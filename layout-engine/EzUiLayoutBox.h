#pragma once

#include <gdiplus.h>
#include "yoga/Yoga.h"
#include "EzUIHelper.h"
#include "EzUiStyledNode.h"

using Microsoft::WRL::ComPtr;

class EzUiLayoutBox {
public:
  std::wstring tag;
  YGNodeRef ygNode = nullptr;
  EzUiStyledNode* styleNode = nullptr;

  EzUiLayoutBox* parent = nullptr;
  std::vector<EzUiLayoutBox*> children;

  EzUI::RectF rect;

  ComPtr<IDWriteTextFormat> mTextFormat;
  ComPtr<IDWriteTextLayout> mTextLayout;

  bool mIsHover = false;
  bool mIsFocused = false;

  EzUiLayoutBox(const std::wstring& n = L"") : tag(n) {
    ygNode = YGNodeNew();
    YGNodeSetContext(ygNode, this);
  }

  ~EzUiLayoutBox() {
    for (auto child : children) {
      delete child;
    }
    YGNodeFree(ygNode);
  }

  void AddChild(EzUiLayoutBox* child) {
    child->parent = this;
    children.push_back(child);
    YGNodeInsertChild(ygNode, child->ygNode, YGNodeGetChildCount(ygNode));
  }

  IDWriteTextLayout* GetTextLayout() {
    return mTextLayout.Get();
  }

  IDWriteTextLayout* UpdateTextLayout(float maxWidth, float maxHeight);

  IDWriteTextFormat* GetTextFormat();

  YGSize MesureText(float maxWidth, float maxHeight);

  void OnMouseEnter();

  void OnMouseLeave();

  float GetOpacity() const;

  std::optional<EzUI::Color> GetBackgroundColor() const;

  std::optional<EzUI::Color> GetBorderColor() const;

  EzUI::EdgeSizes GetBorder() const;

};

EzUI::RectF GetAbsoluteRect(YGNodeRef node);