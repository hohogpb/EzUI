#pragma once

#include <gdiplus.h>
#include "yoga/Yoga.h"
#include "EzUIHelper.h"
#include "EzUiStyledNode.h"

using Microsoft::WRL::ComPtr;

enum class CursorType {
  Default,
  Pointer,    // 手型，常用于按钮
  Text,
};

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
  
  // ? 新增：D2D Bitmap 缓存（用于背景图片或 img 标签的图片）
  ComPtr<ID2D1Bitmap> mCachedBitmap;
  std::wstring mCachedImagePath;  // 缓存的图片路径

  EzUiLayoutBox(const std::wstring& n = L"") : tag(n) {
    ygNode = YGNodeNew();
    YGNodeSetContext(ygNode, this);
  }

  ~EzUiLayoutBox() {
    for (auto child : children) {
      delete child;
    }
    YGNodeFree(ygNode);
    // mCachedBitmap 会自动释放（ComPtr）
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

  // ? 新增：获取背景图片的 D2D Bitmap
  ID2D1Bitmap* GetBackgroundBitmap(ID2D1RenderTarget* renderTarget);

  // ? 新增：获取 img 标签的图片源
  std::wstring GetImageSource() const;

  // ? 新增：获取图片的原生尺寸（用于 img 标签的自适应大小）
  bool GetImageNativeSize(UINT& outWidth, UINT& outHeight) const;

  CursorType cursor = CursorType::Default;
  CursorType GetCursorType() const {
    if (tag == L"button") {
      return CursorType::Pointer;
    } else if (tag == L"text") {
      return CursorType::Text;
    }
    return cursor;
  }
};

EzUI::RectF GetAbsoluteRect(YGNodeRef node);