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

  // 情况 1：自己处于 hover 状态，有 hover 样式
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

  // 情况 2：正常状态，首先检查 CSS 中定义的 opacity 值
  // ✅ 检查 specifiedValues 中是否有 CSS 提供的 opacity
  auto opacityIt = styleNode->specifiedValues.find(L"opacity");
  if (opacityIt != styleNode->specifiedValues.end()) {
    try {
      float opacity = std::stof(opacityIt->second);
      return (std::max)(0.0f, (std::min)(1.0f, opacity));
    } catch (...) {
      // 转换失败，继续使用兜底方案
    }
  }

  // 兜底：使用 styleNode->opacity（由 opacityResolver 设置）
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

// ✅ 新增：获取背景图片的 D2D Bitmap
ID2D1Bitmap* EzUiLayoutBox::GetBackgroundBitmap(ID2D1RenderTarget* renderTarget) {
  if (!styleNode || !styleNode->bgImage || !renderTarget) {
    return nullptr;
  }

  // 检查缓存是否有效
  if (mCachedBitmap && mCachedImagePath == L"background") {
    return mCachedBitmap.Get();
  }

  try {
    // 从 Gdiplus::Image 转换到 D2D Bitmap
    UINT width = styleNode->bgImage->GetWidth();
    UINT height = styleNode->bgImage->GetHeight();

    if (width == 0 || height == 0) {
      return nullptr;
    }

    // 将 Image 转换为 Bitmap（这样才能调用 LockBits）
    Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
    Gdiplus::Graphics graphics(&bitmap);
    graphics.DrawImage(styleNode->bgImage, 0, 0, width, height);

    ComPtr<ID2D1Bitmap> d2dBitmap;
    
    // 获取图片像素数据
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, width, height);
    
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok) {
      try {
        // 创建 D2D Bitmap
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
          D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
        );
        
        HRESULT hr = renderTarget->CreateBitmap(
          D2D1::SizeU(width, height),
          bitmapData.Scan0,
          bitmapData.Stride,
          props,
          &d2dBitmap
        );
        
        if (SUCCEEDED(hr) && d2dBitmap) {
          mCachedBitmap = d2dBitmap;
          mCachedImagePath = L"background";
        }
      } catch (...) {
        // 转换失败
      }
      
      bitmap.UnlockBits(&bitmapData);
    }

    return d2dBitmap.Get();
  } catch (...) {
    OutputDebugStringW((L"[GetBackgroundBitmap] Exception converting image\n"));
    return nullptr;
  }
}

// ✅ 新增：获取 img 标签的图片源
std::wstring EzUiLayoutBox::GetImageSource() const {
  if (!styleNode) {
    OutputDebugStringW(L"[GetImageSource] styleNode is nullptr\n");
    return L"";
  }
  
  // 对于 <img> 标签，从 src 属性获取
  if (tag == L"img") {
    auto srcIt = styleNode->specifiedValues.find(L"src");
    if (srcIt != styleNode->specifiedValues.end()) {
      OutputDebugStringW((L"[GetImageSource] Found src: " + srcIt->second + L"\n").c_str());
      return srcIt->second;
    } else {
      OutputDebugStringW(L"[GetImageSource] src attribute not found in specifiedValues\n");
      // 调试：列出所有 specifiedValues
      OutputDebugStringW(L"[GetImageSource] Available specifiedValues:\n");
      for (const auto& [key, val] : styleNode->specifiedValues) {
        OutputDebugStringW((L"  - " + key + L": " + val + L"\n").c_str());
      }
    }
  } else {
    OutputDebugStringW((L"[GetImageSource] tag is not 'img': " + tag + L"\n").c_str());
  }
  
  return L"";
}

// ✅ 新增：获取图片的原生尺寸
bool EzUiLayoutBox::GetImageNativeSize(UINT& outWidth, UINT& outHeight) const {
  if (tag != L"img") {
    OutputDebugStringW((L"[GetImageNativeSize] tag is not 'img': " + tag + L"\n").c_str());
    return false;
  }

  auto imageSrc = GetImageSource();
  if (imageSrc.empty()) {
    OutputDebugStringW(L"[GetImageNativeSize] imageSrc is empty\n");
    return false;
  }

  try {
    Gdiplus::Image image(imageSrc.c_str());
    if (image.GetLastStatus() != Gdiplus::Ok) {
      OutputDebugStringW((L"[GetImageNativeSize] Failed to load image: " + imageSrc + L"\n").c_str());
      return false;
    }

    UINT width = image.GetWidth();
    UINT height = image.GetHeight();

    if (width == 0 || height == 0) {
      OutputDebugStringW((L"[GetImageNativeSize] Invalid image size: " + imageSrc + L"\n").c_str());
      return false;
    }

    outWidth = width;
    outHeight = height;
    
    OutputDebugStringW((L"[GetImageNativeSize] Image size: " + std::to_wstring(width) + L"x" + std::to_wstring(height) + L"\n").c_str());
    return true;
  } catch (...) {
    OutputDebugStringW((L"[GetImageNativeSize] Exception loading image: " + imageSrc + L"\n").c_str());
    return false;
  }
}
