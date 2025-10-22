#include "pch.h"
#include "EzUIElement.h"

using Microsoft::WRL::ComPtr;
extern ComPtr<IDWriteFactory> gDWriteFactory;

EzUI::RectF UIElement::GetAbsoluteRect(YGNodeRef node) {
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

#if 0
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
#endif

void UIElement::OnRenderD2D(ID2D1HwndRenderTarget* rt) {
  EzUI::Rect ygRect = GetAbsoluteRect(ygNode);

  auto myRect = this->rect;

  if (ygRect.x != myRect.x || ygRect.y != myRect.y ||
    ygRect.width != myRect.width || ygRect.height != myRect.height) {
    // invalid rect
    return;
  }

  // Create a SolidBrush for the background color.
  ID2D1SolidColorBrush* selBrush = nullptr;
  rt->CreateSolidColorBrush(
    D2D1::ColorF(bgColor.r, bgColor.g, bgColor.b, bgColor.a / 255.f),
    &selBrush
  );


  D2D1_RECT_F rect = D2D1::RectF(ygRect.x, ygRect.y, ygRect.right(), ygRect.bottom());
  rt->FillRectangle(rect, selBrush);

  if (selBrush)
    selBrush->Release();

#if 0
  // Draw the background image if it exists.
  if (backgroundImage) {
    g.DrawImage(backgroundImage, rect.left, rect.top, rect.width, rect.height);
  }
#endif
  float finalOpacity = 0.5; // mIsHover ? 1 : opacity;

  if (finalOpacity < 1.f) {
    D2D1_LAYER_PARAMETERS params = D2D1::LayerParameters(
      D2D1::InfiniteRect(), // 覆盖范围（可用 SVG 边界替代）
      nullptr, // 无几何遮罩
      D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
      D2D1::IdentityMatrix(),  // 无变换
      finalOpacity, // 👈 这里设置透明度 0.0 ~ 1.0
      nullptr,
      D2D1_LAYER_OPTIONS_NONE
    );

    rt->PushLayer(params, nullptr);
  }

  // svg要获取上层节点的opacity
  if (tag == L"svg") {
    DrawSvg(rt, ygRect);
  }

  if (!text.empty()) {
    ComPtr<ID2D1SolidColorBrush> brush;
    rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);

    // rt->DrawTextW(text.c_str(), text.length(), textFormat, &rect, brush.Get());    
    auto textLayout = GetTextLayout();
    if (!textLayout) {
      textLayout = UpdateTextLayout(ygRect.width, ygRect.height);
    }

    if (textLayout != nullptr) {
      rt->DrawTextLayout({ rect.left, rect.top }, textLayout, brush.Get());
    }
  }

  if (finalOpacity < 1.f) {
    rt->PopLayer();
  }

#if 0
  for (auto c : children)
    c->OnRenderD2D(rt);
#endif
}


void UIElement::DrawSvg(ID2D1HwndRenderTarget* rt, EzUI::RectF ygRect) {
  // this requires Windows 10 1703
  ComPtr<ID2D1DeviceContext5> dc;
  rt->QueryInterface(dc.GetAddressOf());

  // 创建内存流
  HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, docText.size() * sizeof(wchar_t));
  void* pData = GlobalLock(hMem);
  memcpy(pData, docText.data(), docText.size() * sizeof(wchar_t));
  GlobalUnlock(hMem);

  ComPtr<IStream> svgStream;
  CreateStreamOnHGlobal(hMem, TRUE, &svgStream); // TRUE 表示释放时自动释放内存

  ComPtr<ID2D1SvgDocument> svgDoc;
  dc->CreateSvgDocument(svgStream.Get(), D2D1::SizeF(ygRect.width, ygRect.height), &svgDoc);

#if 0
  // open a stream in the .SVG file
  ComPtr<IStream> svgStream;
  SHCreateStreamOnFile(L"C:/Users/51216/Desktop/cppui/Windows-universal-samples/Samples/D2DSvgImage/cpp/drawing.svg", 0, &svgStream);
#endif

  D2D1_MATRIX_3X2_F oldTransform;
  dc->GetTransform(&oldTransform);

  // 设置新的变换
  auto transform = D2D1::Matrix3x2F::Translation(ygRect.x, ygRect.y);
  dc->SetTransform(transform);

  if (svgDoc) {
    dc->DrawSvgDocument(svgDoc.Get());
  }

  dc->SetTransform(oldTransform);
}

IDWriteTextFormat* UIElement::GetTextFormat() {
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

IDWriteTextLayout* UIElement::GetTextLayout() {
  return mTextLayout.Get();
}

IDWriteTextLayout* UIElement::UpdateTextLayout(float maxWidth, float maxHeight) {
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

YGSize UIElement::MesureText(float maxWidth, float maxHeight) {
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


void UIElement::OnMouseEnter() {

  mIsHover = true;
  std::wcout << tag << L" " << name << L"OnMouseEnter()" << std::endl;
}

// 只设置状态
void UIElement::OnMouseLeave() {
  mIsHover = false;
  std::wcout << tag << L" " << name << L"OnMouseLeave()" << std::endl;
}
