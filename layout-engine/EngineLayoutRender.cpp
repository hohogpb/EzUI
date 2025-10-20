// �õĲο��̳̣�
// https://devyang.space/2020/09/13/yoga%E5%B8%83%E5%B1%80%E8%A7%A3%E6%9E%90/

#include "pch.h"
#include "EngineLayoutRender.h"
#include "yoga/Yoga.h"
#include "EzUIDocParser.h"
#include "EzUIElement.h"
#include "EzUITreeBuilder.h"

using std::stack;
using std::pair;
using Microsoft::WRL::ComPtr;
using namespace EzUI;

extern EzUIWindow* mainWindow;

ComPtr<ID2D1Factory> gD2DFactory;
ComPtr<ID2D1HwndRenderTarget> gRenderTarget;
ComPtr<IDWriteFactory> gDWriteFactory;

//----------------------------------------------
// ȫ�� UI ��
//----------------------------------------------
UIElement* uiRoot = nullptr;
UIElement* lastHittedUiNode = nullptr;
UIElement* lastFocusedUiNode = nullptr;

//----------------------------------------------
// ���ֳ�ʼ��
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
  // D2D
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


  EzUIDocParser parser;
  auto domRoot = parser.ParseFile(L"SimpleEzUI.ezui");
  while (!domRoot) {
    //MessageBoxA(NULL, "Failed to load SimpleEzUI.ezui", "Error", MB_OK | MB_ICONERROR);
    //return;
    std::wcout << L"��ui�ļ�ʧ�ܣ��ٴγ���\n";
    domRoot = parser.ParseFile(L"SimpleEzUI.ezui");
  }

#if 1
  EzUITreeBuilder uiTreeBuilder;
  uiRoot = uiTreeBuilder.Build(domRoot.get());

#else
  // ���ڵ�
  uiRoot = new UIElement(L"Root");
  //YGNodeStyleSetFlexDirection(uiRoot->ygNode, YGFlexDirectionRow);
  //YGNodeStyleSetWidth(uiRoot->ygNode, 800);
 //YGNodeStyleSetHeight(uiRoot->ygNode, 600);

  // �����
  auto sidebar = new UIElement(L"Sidebar");
  sidebar->color = Color(255, 255, 0, 0);
  YGNodeStyleSetWidth(sidebar->ygNode, 200);
  // YGNodeStyleSetHeight(sidebar->node,  YGPercentValue(100));
  //YGNodeStyleSetHeightPercent(sidebar->ygNode, 100.0f);
  uiRoot->AddChild(sidebar);

#if 0
  // ��������
  auto content = new UIElement(L"Content");
  content->color = Color(255, 0, 255, 0);
  YGNodeStyleSetFlexGrow(content->ygNode, 1);
  YGNodeStyleSetFlexDirection(content->ygNode, YGFlexDirectionColumn);
  uiRoot->AddChild(content);

  // �ϲ�
  auto top = new UIElement(L"Top");
  top->color = Color(255, 0, 0, 255);
  YGNodeStyleSetHeight(top->ygNode, 200);
  content->AddChild(top);

  // �²����Զ���չ��
  auto bottom = new UIElement(L"Bottom");
  bottom->color = Color(255, 0, 0, 0);
  YGNodeStyleSetFlexGrow(bottom->ygNode, 1);
  content->AddChild(bottom);
#endif
#endif

}

//----------------------------------------------
// ���ּ���
//----------------------------------------------
void EngineLayout_PerformLayout(UIElement* root, float width, float height) {
  // ���ø����ִ�С
  YGNodeStyleSetWidth(root->ygNode, (float)width);
  YGNodeStyleSetHeight(root->ygNode, (float)height);

  YGNodeCalculateLayout(root->ygNode, YGUndefined, YGUndefined, YGDirectionLTR);

  // ����ÿ��ui�ڵ��rect �����
  std::stack<UIElement*> nodeStack;
  nodeStack.push(root);


  while (!nodeStack.empty()) {
    auto node = nodeStack.top();
    nodeStack.pop();

    node->rect = UIElement::GetAbsoluteRect(node->ygNode);

    for (auto& child : node->children) {
      nodeStack.push(child);
    }
  }
}

void EngineLayout_Resize(EzUIWindow* wnd, int width, int height) {
  // ���� RenderTarget ��С
  if (gRenderTarget) {
    D2D1_SIZE_U size = D2D1::SizeU(width, height);
    gRenderTarget->Resize(size);
  }

  if (uiRoot) {
    EngineLayout_PerformLayout(uiRoot, (float)width, (float)height);
    wnd->Invalidate();
  }
}

void EngineLayout_RenderUI(EzUIWindow* wnd, HDC hdc) {
#if 0  
  Graphics g(hdc);
  g.Clear(Color(255, 245, 245, 245));

  if (uiRoot)
    uiRoot->OnRender(g);
#endif

  gRenderTarget->BeginDraw();
  //gRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

#if 0
  if (uiRoot)
    uiRoot->OnRenderD2D(gRenderTarget.Get());
#endif

  stack<UIElement*> uiNodeStack;
  uiNodeStack.push(uiRoot);

  while (!uiNodeStack.empty()) {
    auto uiNode = uiNodeStack.top();
    uiNodeStack.pop();

    // ���Ʊ߿�
    uiNode->OnRenderD2D(gRenderTarget.Get());

    for (auto it = uiNode->children.rbegin(); it != uiNode->children.rend(); ++it) {
      uiNodeStack.push(*it);
    }
  }

  HRESULT hr = gRenderTarget->EndDraw();
  if (hr == D2DERR_RECREATE_TARGET)
    gRenderTarget.Reset();

}

void EngineLayout_SetHittedUIElement(UIElement* uiNode) {
  if (uiNode == lastHittedUiNode) {
    // mouse move
    return;
  }

  if (lastHittedUiNode)
    lastHittedUiNode->OnMouseLeave();
  lastHittedUiNode = uiNode;
  lastHittedUiNode->OnMouseEnter();
}

void EngineLayout_SetFocusUIElement() {
  if (lastFocusedUiNode == lastHittedUiNode) {
    return;
  }
  lastFocusedUiNode = lastHittedUiNode;
  
}

void EngineLayout_HitTest(EzUIAppWindow* appWnd, int x, int y) {
  PointF mousePt(x, y);

  struct HitContext {
    UIElement* node;
    int level;
  };

  int maxLevel = -1;
  UIElement* maxLevelNode = nullptr;

  stack<HitContext> uiNodeStack;
  uiNodeStack.push({ uiRoot, 0 });

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

  // �������Ҫ ˢ�´���
  mainWindow->Invalidate();
}


void EngineLayout_LButtonDown(EzUIAppWindow* appWnd, int x, int y) {
  EngineLayout_SetFocusUIElement();

  mainWindow->Invalidate();
};

// ��⵽���������� Ҫȥʵ��uiԪ�ص�click��
void EngineLayout_LButtonUp(EzUIAppWindow* appWnd, int x, int y) {
  EngineLayout_SetFocusUIElement();

  mainWindow->Invalidate();
};