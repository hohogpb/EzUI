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
  // 需要触发重新渲染以应用 hover 样式
}

void EzUiLayoutBox::OnMouseLeave() {
  auto name = styleNode ? styleNode->name : L"";

  mIsHover = false;
  std::wcout << tag << L" " << name << L"OnMouseLeave()" << std::endl;
  // 需要触发重新渲染以恢复正常样式
}

float EzUiLayoutBox::GetOpacity() const {
  if (!styleNode) {
    return 1.0f;
  }

  // 如果在 hover 状态，且有 hover 样式，使用 hover 样式
  if (mIsHover && !styleNode->hoverValues.empty()) {
    auto opacityIt = styleNode->hoverValues.find(L"opacity");
    if (opacityIt != styleNode->hoverValues.end()) {
      try {
        float hoverOpacity = std::stof(opacityIt->second);
        // 确保透明度在 0 到 1 之间
        return (std::max)(0.0f, (std::min)(1.0f, hoverOpacity));
      } catch (...) {
        // 如果转换失败，使用正常状态
      }
    }
  }

  // 否则使用正常状态的透明度
  return styleNode->opacity;
}

std::optional<EzUI::Color> EzUiLayoutBox::GetBackgroundColor() const {
  if (!styleNode) {
    return std::nullopt;
  }

  // 如果在 hover 状态，且有 hover 样式，使用 hover 样式
  if (mIsHover && !styleNode->hoverValues.empty()) {
    auto bgColorIt = styleNode->hoverValues.find(L"background-color");
    if (bgColorIt != styleNode->hoverValues.end()) {
      return ParseColor(bgColorIt->second);
    }
  }

  // 否则使用正常状态的背景色
  return styleNode->bgColor;
}

std::optional<EzUI::Color> EzUiLayoutBox::GetBorderColor() const {
  if (!styleNode) {
    return std::nullopt;
  }

  // 如果在 hover 状态，且有 hover 样式，使用 hover 样式
  if (mIsHover && !styleNode->hoverValues.empty()) {
    auto borderColorIt = styleNode->hoverValues.find(L"border-color");
    if (borderColorIt != styleNode->hoverValues.end()) {
      return ParseColor(borderColorIt->second);
    }
  }

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
  if (!styleNode) {
    return EzUI::EdgeSizes();
  }

  // 如果在 hover 状态，且有 hover 样式，使用 hover 样式
  if (mIsHover && !styleNode->hoverValues.empty()) {
    auto borderWidthIt = styleNode->hoverValues.find(L"border-width");
    if (borderWidthIt != styleNode->hoverValues.end()) {
      try {
        float width = std::stof(borderWidthIt->second);
        return EzUI::EdgeSizes{ width, width, width, width };
      } catch (...) {
        // 转换失败，继续使用正常状态
      }
    }
  }

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
