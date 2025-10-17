// �õĲο��̳̣�
// https://devyang.space/2020/09/13/yoga%E5%B8%83%E5%B1%80%E8%A7%A3%E6%9E%90/

#include "pch.h"
#include "EngineLayoutRender.h"
#include <gdiplus.h>
#include "yoga/Yoga.h"
#include "EzUIDocParser.h"
#include "EzUIElement.h"
#include "EzUITreeBuilder.h"

using std::stack;
using std::pair;

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

void EngineLayout_InitGDIPlus() {
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}


//----------------------------------------------
// ���ּ���
//----------------------------------------------
void EngineLayout_PerformLayout(UIElement* root, float width, float height) {
  // ���ø����ִ�С
  YGNodeStyleSetWidth(root->ygNode, (float)width);
  YGNodeStyleSetHeight(root->ygNode, (float)height);

  YGNodeCalculateLayout(root->ygNode, YGUndefined, YGUndefined, YGDirectionLTR);
}

//----------------------------------------------
// ȫ�� UI ��
//----------------------------------------------
UIElement* uiRoot = nullptr;

//----------------------------------------------
// ���ֳ�ʼ��
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
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
// ����
//----------------------------------------------
void EngineLayout_RenderUI(EzUIWindow* wnd, HDC hdc) {
  Graphics g(hdc);
  g.Clear(Color(255, 245, 245, 245));

  if (uiRoot)
    uiRoot->OnRender(g);
}


void EngineLayout_Resize(EzUIWindow* wnd, int width, int height) {
  if (uiRoot) {
    EngineLayout_PerformLayout(uiRoot, (float)width, (float)height);
    wnd->Invalidate();
  }
}