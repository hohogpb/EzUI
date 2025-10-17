// 好的参考教程：
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
// 布局计算
//----------------------------------------------
void EngineLayout_PerformLayout(UIElement* root, float width, float height) {
  // 设置根布局大小
  YGNodeStyleSetWidth(root->ygNode, (float)width);
  YGNodeStyleSetHeight(root->ygNode, (float)height);

  YGNodeCalculateLayout(root->ygNode, YGUndefined, YGUndefined, YGDirectionLTR);
}

//----------------------------------------------
// 全局 UI 树
//----------------------------------------------
UIElement* uiRoot = nullptr;

//----------------------------------------------
// 布局初始化
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
  EzUIDocParser parser;
  auto domRoot = parser.ParseFile(L"SimpleEzUI.ezui");
  while (!domRoot) {
    //MessageBoxA(NULL, "Failed to load SimpleEzUI.ezui", "Error", MB_OK | MB_ICONERROR);
    //return;
    std::wcout << L"打开ui文件失败，再次尝试\n";
    domRoot = parser.ParseFile(L"SimpleEzUI.ezui");
  }

#if 1
  EzUITreeBuilder uiTreeBuilder;
  uiRoot = uiTreeBuilder.Build(domRoot.get());

#else
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
// 绘制
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