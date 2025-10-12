#include "pch.h"
#include "EngineLayoutRender.h"
#include <gdiplus.h>
#include "yoga/Yoga.h"

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

void EngineLayout_InitGDIPlus() {
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

struct RectF32 { float x, y, w, h; };

class UIElement {
public:
  std::string name;
  YGNodeRef node;
  std::vector<UIElement*> children;
  RectF32 rect{};
  Color color{ 255, 200, 200, 200 };

  UIElement(const std::string& n = "") : name(n) {
    node = YGNodeNew();
    YGNodeSetContext(node, this);
  }

  virtual ~UIElement() {
    for (auto c : children) delete c;
    YGNodeFree(node);
  }

  void AddChild(UIElement* child) {
    children.push_back(child);
    YGNodeInsertChild(node, child->node, YGNodeGetChildCount(node));
  }

  virtual void OnRender(Graphics& g) {
    SolidBrush brush(color);
    g.FillRectangle(&brush, rect.x, rect.y, rect.w, rect.h);


    for (auto c : children)
      c->OnRender(g);
  }
};

//----------------------------------------------
// 布局计算
//----------------------------------------------
void EngineLayout_PerformLayout(UIElement* root, float width, float height) {
  // 设置根布局大小
  YGNodeStyleSetWidth(root->node, (float)width);
  YGNodeStyleSetHeight(root->node, (float)height);

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);

#if 1
  // 递归更新 absolute rect（累加父偏移）
  std::function<void(UIElement*, float, float)> update =
    [&](UIElement* elem, float parentAbsX, float parentAbsY) {
    // 取局部布局值（相对于父节点）
    float localLeft = YGNodeLayoutGetLeft(elem->node);
    float localTop = YGNodeLayoutGetTop(elem->node);
    float localWidth = YGNodeLayoutGetWidth(elem->node);
    float localHeight = YGNodeLayoutGetHeight(elem->node);

    // 计算绝对位置（累加父偏移）
    float absX = parentAbsX + localLeft;
    float absY = parentAbsY + localTop;

    // 如果你的 UIElement 使用 RECT / int 值存储，按需转换（四舍五入）
    elem->rect.x = absX;
    elem->rect.y = absY;
    elem->rect.w = localWidth;
    elem->rect.h = localHeight;

    // 递归其子节点
    for (auto child : elem->children) {
      update(child, absX, absY);
    }
  };

  // 根节点父偏移为 0,0（如果你想让根有 margin 可修改）
  update(root, 0.0f, 0.0f);
#endif

#if 0

  std::function<void(UIElement*)> update = [&](UIElement* e) {
    e->rect.x = YGNodeLayoutGetLeft(e->node);
    e->rect.y = YGNodeLayoutGetTop(e->node);
    e->rect.w = YGNodeLayoutGetWidth(e->node);
    e->rect.h = YGNodeLayoutGetHeight(e->node);
    for (auto c : e->children)
      update(c);
  };
  update(root);
#endif
}

//----------------------------------------------
// 全局 UI 树
//----------------------------------------------
UIElement* g_root = nullptr;

//----------------------------------------------
// 布局初始化
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
  g_root = new UIElement("Root");
  YGNodeStyleSetFlexDirection(g_root->node, YGFlexDirectionRow);
  YGNodeStyleSetWidth(g_root->node, 800);
  YGNodeStyleSetHeight(g_root->node, 600);

  // 左边栏
  auto sidebar = new UIElement("Sidebar");
  sidebar->color = Color(255, 255, 0, 0);
  YGNodeStyleSetWidth(sidebar->node, 200);
  // YGNodeStyleSetHeight(sidebar->node,  YGPercentValue(100));
  YGNodeStyleSetHeightPercent(sidebar->node, 100.0f);
  g_root->AddChild(sidebar);


  // 主内容区
  auto content = new UIElement("Content");
  content->color = Color(255, 0, 255, 0);
  YGNodeStyleSetFlexGrow(content->node, 1);
  YGNodeStyleSetFlexDirection(content->node, YGFlexDirectionColumn);
  g_root->AddChild(content);

  // 上部
  auto top = new UIElement("Top");
  top->color = Color(255, 0, 0, 255);
  YGNodeStyleSetHeight(top->node, 200);
  content->AddChild(top);

  // 下部（自动扩展）
  auto bottom = new UIElement("Bottom");
  bottom->color = Color(255, 0, 0, 0);
  YGNodeStyleSetFlexGrow(bottom->node, 1);
  content->AddChild(bottom);

}

//----------------------------------------------
// 绘制
//----------------------------------------------
void EngineLayout_RenderUI(EzUIWindow* wnd, HDC hdc) {
  Graphics g(hdc);
  g.Clear(Color(255, 245, 245, 245));

  if (g_root)
    g_root->OnRender(g);
}


void EngineLayout_Resize(EzUIWindow* wnd, int width, int height) {
  if (g_root) {
    EngineLayout_PerformLayout(g_root, (float)width, (float)height);
    wnd->Invalidate();
  }
}