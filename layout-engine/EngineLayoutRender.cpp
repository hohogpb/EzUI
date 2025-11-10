// 好的参考教程：
// https://devyang.space/2020/09/13/yoga%E5%B8%83%E5%B1%80%E8%A7%A3%E6%9E%90/

#include "pch.h"
#include "EngineLayoutRender.h"
#include "yoga/Yoga.h"
#include "EzUIDocParser.h"
//#include "EzUIElement.h"
// #include "EzUITreeBuilder.h"
#include "EzUITextLoader.h"
#include "EzUICssParser.h"
#include "EzUiStyleTreeBuilder.h"
#include "EzUiLayoutBuilder.h"
#include "EzUiPainter.h"

using std::stack;
using std::pair;
using Microsoft::WRL::ComPtr;
using namespace EzUI;

extern EzUIWindow* mainWindow;

ComPtr<ID2D1Factory> gD2DFactory;
ComPtr<ID2D1HwndRenderTarget> gRenderTarget;
ComPtr<IDWriteFactory> gDWriteFactory;

//----------------------------------------------
// 全局 UI 树
//----------------------------------------------

#if 0
UIElement* uiRoot = nullptr;
UIElement* lastHittedUiNode = nullptr;
UIElement* lastFocusedUiNode = nullptr;
#endif

std::unique_ptr<EzUiStyledNode> gStyleTree;

EzUiLayoutBox* gLayoutRoot = nullptr;
//EzUiLayoutBox* gLastHittedLayoutNode = nullptr;
EzUiLayoutBox* gLastFocusedLayoutNode = nullptr;

std::vector<EzUiLayoutBox*> gCurrentHoverChain;

void InitD2D(EzUIWindow* wnd) {
  if (!gD2DFactory)
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, gD2DFactory.GetAddressOf());

  if (!gRenderTarget) {
    RECT rc = wnd->GetClientRect();

    gD2DFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(wnd->GetHwnd(), D2D1::SizeU(rc.right, rc.bottom)),
      &gRenderTarget
    );
  }

  if (!gDWriteFactory)
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&gDWriteFactory);
}

//----------------------------------------------
// 布局初始化
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
  // D2D
  InitD2D(wnd);

  auto domRoot = EzUIDocParser::ParseFile(L"SimpleEzUI.ezui");
  while (!domRoot) {
    std::wcout << L"打开ui文件失败，再次尝试\n";
    domRoot = EzUIDocParser::ParseFile(L"SimpleEzUI.ezui");
  }

  auto stylesheet = EzUICssParser::ParseFile(L"SimpleEzUI.css");

  gStyleTree = EzUiStyleTreeBuilder::BuildTree(domRoot.get(), stylesheet.get());

  gLayoutRoot = EzUiLayoutBuilder::BuildTree(gStyleTree.get());

#if 0
  EzUITreeBuilder uiTreeBuilder;
  uiRoot = uiTreeBuilder.Build(domRoot.get());
#endif

#if 0
  // 根节点
  uiRoot = new UIElement(L"Root");
  //YGNodeStyleSetFlexDirection(uiRoot->ygNode, YGFlexDirectionRow);
  //YGNodeStyleSetWidth(uiRoot->ygNode, 800);
  //YGNodeStyleSetHeight(uiRoot->ygNode, 600);

   // 左边栏
  auto sidebar = new UIElement(L"Sidebar");
  sidebar->color = Color(255, 255, 0, 0);
  YGNodeStyleSetWidth(sidebar->ygNode, 200);
  // YGNodeStyleSetHeight(sidebar->node,  YGPercentValue(100));
  //YGNodeStyleSetHeightPercent(sidebar->ygNode, 100.0f);
  uiRoot->AddChild(sidebar);

#if 0
  // 主内容区
  auto content = new UIElement(L"Content");
  content->color = Color(255, 0, 255, 0);
  YGNodeStyleSetFlexGrow(content->ygNode, 1);
  YGNodeStyleSetFlexDirection(content->ygNode, YGFlexDirectionColumn);
  uiRoot->AddChild(content);

  // 上部
  auto top = new UIElement(L"Top");
  top->color = Color(255, 0, 0, 255);
  YGNodeStyleSetHeight(top->ygNode, 200);
  content->AddChild(top);

  // 下部（自动扩展）
  auto bottom = new UIElement(L"Bottom");
  bottom->color = Color(255, 0, 0, 0);
  YGNodeStyleSetFlexGrow(bottom->ygNode, 1);
  content->AddChild(bottom);
#endif
#endif

}

//----------------------------------------------
// 布局计算
//----------------------------------------------
void EngineLayout_PerformLayout(float width, float height) {
  // 设置根布局大小
  YGNodeStyleSetWidth(gLayoutRoot->ygNode, (float)width);
  YGNodeStyleSetHeight(gLayoutRoot->ygNode, (float)height);

  YGNodeCalculateLayout(gLayoutRoot->ygNode, YGUndefined, YGUndefined, YGDirectionLTR);

  // 更新每个ui节点的rect 和深度
  std::stack<EzUiLayoutBox*> nodeStack;
  nodeStack.push(gLayoutRoot);

  while (!nodeStack.empty()) {
    auto node = nodeStack.top();
    nodeStack.pop();

    node->rect = GetAbsoluteRect(node->ygNode);

    for (auto& child : node->children) {
      nodeStack.push(child);
    }
  }
}

void EngineLayout_Resize(EzUIWindow* wnd, int width, int height) {
  // 调整 RenderTarget 大小
  if (gRenderTarget) {
    D2D1_SIZE_U size = D2D1::SizeU(width, height);
    gRenderTarget->Resize(size);
  }

  if (gLayoutRoot) {
    EngineLayout_PerformLayout((float)width, (float)height);
    wnd->Invalidate();
  }
}

void DrawSvg(ID2D1HwndRenderTarget* rt, EzUI::RectF ygRect, const std::wstring& docText) {
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


static void EngineLayout_DrawLayoutNode(ID2D1HwndRenderTarget* rt, EzUiLayoutBox* aLayoutNode) {
  EzUI::Rect ygRect = GetAbsoluteRect(aLayoutNode->ygNode);

  auto myRect = aLayoutNode->rect;
  // ✅ 使用当前状态的背景颜色（考虑 hover）
  auto bgColor = aLayoutNode->GetBackgroundColor();
  auto tag = aLayoutNode->tag;
  auto text = aLayoutNode->styleNode ? aLayoutNode->styleNode->text : L"";
  auto docText = aLayoutNode->styleNode ? aLayoutNode->styleNode->docText : L"";
  auto opacity = aLayoutNode->GetOpacity();

  if (ygRect.x != myRect.x || ygRect.y != myRect.y ||
    ygRect.width != myRect.width || ygRect.height != myRect.height) {
    // invalid rect
    return;
  }

  D2D1_RECT_F rect(ygRect.x, ygRect.y, ygRect.right(), ygRect.bottom());

  if (bgColor) {
    auto [r, g, b, a] = *bgColor;

    ComPtr<ID2D1SolidColorBrush> bgBrush;
    rt->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a / 255.f), &bgBrush);

    rt->FillRectangle(rect, bgBrush.Get());
  }

  // svg要获取上层节点的opacity
  if (tag == L"svg") {
    DrawSvg(rt, ygRect, docText);
  }

  if (!text.empty()) {
    ComPtr<ID2D1SolidColorBrush> brush;
    rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);

    auto textLayout = aLayoutNode->GetTextLayout();
    if (!textLayout) {
      textLayout = aLayoutNode->UpdateTextLayout(ygRect.width, ygRect.height);
    }

    if (textLayout != nullptr) {
      rt->DrawTextLayout({ rect.left, rect.top }, textLayout, brush.Get());
    }
  }

}

static void EngineLayout_DrawLayoutWithOpacity(ID2D1HwndRenderTarget* dc, EzUiLayoutBox* aLayoutNode) {
  auto opacity = aLayoutNode->GetOpacity();
  auto bounds = aLayoutNode->rect;

  if (opacity < 1.0f) {
    // 推荐把 contentBounds 设置为 node 的边界，而不是 InfiniteRect，
    // 以减少 layer 的工作量和内存开销。
#if 0
    D2D1_LAYER_PARAMETERS params = D2D1::LayerParameters(
      D2D1::InfiniteRect(), // 覆盖范围（可用 SVG 边界替代）
      nullptr, // 无几何遮罩
      D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
      D2D1::IdentityMatrix(),  // 无变换
      finalOpacity, // 👈 这里设置透明度 0.0 ~ 1.0
      nullptr,
      D2D1_LAYER_OPTIONS_NONE
    );
#endif

    D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
    layerParams.contentBounds = D2D1::RectF(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
    layerParams.opacity = opacity;

    // 先 push layer（使 layer 包含 背景 + 子元素 + 前景）
    dc->PushLayer(layerParams, nullptr);

    // 先绘制背景（在子元素之下）
    EngineLayout_DrawLayoutNode(dc, aLayoutNode);

    // 绘制子节点
    for (auto& child : aLayoutNode->children)
      EngineLayout_DrawLayoutWithOpacity(dc, child);

    // 整个 layer 在 Pop 时会按 opacity 合成回去
    dc->PopLayer();
  } else {
    // 不需要 group opacity，则按常规顺序绘制
    EngineLayout_DrawLayoutNode(dc, aLayoutNode);

    for (auto& child : aLayoutNode->children)
      EngineLayout_DrawLayoutWithOpacity(dc, child);

  }
}



void EngineLayout_RenderUI(EzUIWindow* wnd, HDC hdc) {
  auto displayList = EzUiPainter::BuildDisplayList(gLayoutRoot);

  // drawlist 怎么考虑opacity 这种

  gRenderTarget->BeginDraw();
  gRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

  EngineLayout_DrawLayoutWithOpacity(gRenderTarget.Get(), gLayoutRoot);

#if 0
  stack<EzUiLayoutBox*> aStack;
  aStack.push(gLayoutRoot);

  while (!aStack.empty()) {
    auto aNode = aStack.top();
    aStack.pop();

    // 绘制边框
    //aNode->OnRenderD2D(gRenderTarget.Get());
    EngineLayout_DrawLayoutNode(gRenderTarget.Get(), aNode);

    for (auto it = aNode->children.rbegin(); it != aNode->children.rend(); ++it) {
      aStack.push(*it);
    }
  }
#endif

  HRESULT hr = gRenderTarget->EndDraw();
  if (hr == D2DERR_RECREATE_TARGET)
    gRenderTarget.Reset();
}

void EngineLayout_SetHittedUIElement(EzUiLayoutBox* node) {
  // 如果命中对象没变，不做任何事
  if (node == (gCurrentHoverChain.empty() ? nullptr : gCurrentHoverChain.front())) {
    return;
  }

  // 1️ 构建新的 hover 链（命中节点 + 所有父节点）
  std::vector<EzUiLayoutBox*> newChain;
  for (EzUiLayoutBox* n = node; n; n = n->parent) {
    newChain.push_back(n);
  }

  // 2️ 对比旧链与新链
  // 退出的节点：在旧链里但不在新链里
  for (EzUiLayoutBox* oldNode : gCurrentHoverChain) {
    if (std::find(newChain.begin(), newChain.end(), oldNode) == newChain.end()) {
      oldNode->OnMouseLeave();
    }
  }

  // 进入的新节点：在新链里但不在旧链里
  for (EzUiLayoutBox* newNode : newChain) {
    if (std::find(gCurrentHoverChain.begin(), gCurrentHoverChain.end(), newNode) == gCurrentHoverChain.end()) {
      newNode->OnMouseEnter();
    }
  }

  gCurrentHoverChain = std::move(newChain);
  // 设置hoverchain
  // std::vector<Node*> newChain;
#if 0
  if (gLastHittedLayoutNode) {
    gLastHittedLayoutNode->OnMouseLeave();
  }

  gLastHittedLayoutNode = node;
  if (gLastHittedLayoutNode) {
    gLastHittedLayoutNode->OnMouseEnter();
  }
#endif
}

void EngineLayout_SetFocusUIElement() {
  auto lastHittedLayoutNode = gCurrentHoverChain.empty() ? nullptr : gCurrentHoverChain.front();
  if (gLastFocusedLayoutNode == lastHittedLayoutNode) {
    return;
  }
  gLastFocusedLayoutNode = lastHittedLayoutNode;
}


void EngineLayout_HitTest(EzUIAppWindow* appWnd, int x, int y) {
  PointF mousePt(x, y);

  struct HitContext {
    EzUiLayoutBox* node;
    int level;
  };

  int maxLevel = -1;
  EzUiLayoutBox* maxLevelNode = nullptr;

  stack<HitContext> uiNodeStack;
  uiNodeStack.push({ gLayoutRoot, 0 });

  while (!uiNodeStack.empty()) {
    auto [uiNode, level] = uiNodeStack.top();
    uiNodeStack.pop();

    if (!IsIntersect(mousePt, uiNode->rect)) {
      continue;
    }

    if (level > maxLevel) {
      maxLevel = level;
      maxLevelNode = uiNode;
    }

    for (auto it = uiNode->children.rbegin(); it != uiNode->children.rend(); ++it) {
      uiNodeStack.push({ *it, level + 1 });
    }
  }

  EngineLayout_SetHittedUIElement(maxLevelNode);

  // apply hover effect  

  // 如果有需要 刷新窗口
  mainWindow->Invalidate();
}


void EngineLayout_LButtonDown(EzUIAppWindow* appWnd, int x, int y) {
  EngineLayout_SetFocusUIElement();

  mainWindow->Invalidate();
};

// 检测到鼠标左键弹起 要去实现ui元素的click了
void EngineLayout_LButtonUp(EzUIAppWindow* appWnd, int x, int y) {
  EngineLayout_SetFocusUIElement();

  mainWindow->Invalidate();
};