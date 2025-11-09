#include "pch.h"
#include "EzUiLayoutBox.h"


EzUI::RectF GetAbsoluteRect(YGNodeRef node) {
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

extern ComPtr<IDWriteFactory> gDWriteFactory;

IDWriteTextLayout* EzUiLayoutBox::UpdateTextLayout(float maxWidth, float maxHeight) {

  std::wstring text = styleNode ? styleNode->text : L"";

  auto textFormat = GetTextFormat();
  if (textFormat == nullptr) return nullptr;

  mTextLayout.Reset();

  gDWriteFactory->CreateTextLayout(
    text.c_str(),
    text.size(),
    textFormat,
    maxWidth,
    maxHeight,
    &mTextLayout
  );

  return mTextLayout.Get();
}

IDWriteTextFormat* EzUiLayoutBox::GetTextFormat() {
  std::wstring text = styleNode ? styleNode->text : L"";
  if (text.empty()) return nullptr;
  // use cache
  if (mTextFormat != nullptr) return mTextFormat.Get();

  mTextFormat.Reset();
  // update cache
  gDWriteFactory->CreateTextFormat(
    L"Roboto",
    nullptr,
    DWRITE_FONT_WEIGHT_NORMAL,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    13.0f,
    L"zh-cn",
    &mTextFormat
  );

  return mTextFormat.Get();
}

YGSize EzUiLayoutBox::MesureText(float maxWidth, float maxHeight) {
  std::wstring text = styleNode ? styleNode->text : L"";
  if (text.empty()) {
    return YGSize(0, 0);
  }

  auto textFormat = GetTextFormat();
  if (textFormat == nullptr) {
    return YGSize(0, 0);
  }

  textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
  textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
  textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

  ComPtr<IDWriteTextLayout> textLayout;

  gDWriteFactory->CreateTextLayout(
    text.c_str(),
    text.size(),
    textFormat,
    maxWidth,
    maxHeight,
    &textLayout
  );

  DWRITE_TEXT_METRICS metrics;
  textLayout->GetMetrics(&metrics);

  YGSize size;
  size.width = metrics.width;
  size.height = metrics.height;

  textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

  return size;
}

void EzUiLayoutBox::OnMouseEnter() {
  auto name = styleNode ? styleNode->name : L"";
  mIsHover = true;
  std::wcout << tag << L" " << name << L"OnMouseEnter()" << std::endl;
}

// 只设置状态
void EzUiLayoutBox::OnMouseLeave() {
  auto name = styleNode ? styleNode->name : L"";

  mIsHover = false;
  std::wcout << tag << L" " << name << L"OnMouseLeave()" << std::endl;
}

float EzUiLayoutBox::GetOpacity() const {

  float opacity = styleNode->opacity;

  // styleNode->specifiedValues
  return opacity;
}

std::optional<EzUI::Color> EzUiLayoutBox::GetBackgroundColor() const {
  if (styleNode) {
    return styleNode->bgColor;
  }
  return std::nullopt;
}

std::optional<EzUI::Color> EzUiLayoutBox::GetBorderColor() const {
#if 0
  if (styleNode) {
    auto it = styleNode->specifiedValues.find(L"border-color");
    if (it != styleNode->specifiedValues.end()) {
      const auto& borderValue = it->second;
      if (borderValue.type == PropertyValue::Type::Color) {
        return borderValue.colorValue;
      }
    }
  }
#endif
  return std::nullopt;
}

EzUI::EdgeSizes EzUiLayoutBox::GetBorder() const {
#if 0
  if (styleNode) {
    auto it = styleNode->specifiedValues.find(L"border-width");
    if (it != styleNode->specifiedValues.end()) {
      const auto& borderValue = it->second;
      if (borderValue.type == PropertyValue::Type::EdgeSizes) {
        return borderValue.edgeSizesValue;
      }
    }
  }
#endif
  return EzUI::EdgeSizes();
}
