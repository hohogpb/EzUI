// �õĲο��̳̣�
// https://devyang.space/2020/09/13/yoga%E5%B8%83%E5%B1%80%E8%A7%A3%E6%9E%90/

#include "pch.h"
#include "EngineLayoutRender.h"
#include <gdiplus.h>
#include "yoga/Yoga.h"
#include "EzUIDocParser.h"

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

void EngineLayout_InitGDIPlus() {
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

struct RectF32 { float x, y, w, h; };

struct YogaAbsoluteRect {
  float left, top, width, height;
};

YogaAbsoluteRect GetAbsoluteRect(YGNodeRef node) {
  float left = YGNodeLayoutGetLeft(node);
  float top = YGNodeLayoutGetTop(node);
  float width = YGNodeLayoutGetWidth(node);
  float height = YGNodeLayoutGetHeight(node);

  YGNodeRef parent = YGNodeGetOwner(node);

  while (parent) {
    YGPositionType posType = YGNodeStyleGetPositionType(parent);
    // ���Զ�λ�ĸ��ڵ��ֹͣ�ۼӣ�����Ը�����������ϵ��
    if (posType == YGPositionTypeAbsolute)
      break;

    left += YGNodeLayoutGetLeft(parent);
    top += YGNodeLayoutGetTop(parent);
    parent = YGNodeGetOwner(parent);
  }

  return { left, top, width, height };
}

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

    YogaAbsoluteRect rect = GetAbsoluteRect(node);

    RECT r = {
        (LONG)rect.left,
        (LONG)rect.top,
        (LONG)(rect.left + rect.width),
        (LONG)(rect.top + rect.height)
    };

    SolidBrush brush(color);
    g.FillRectangle(&brush, rect.left, rect.top, rect.width, rect.height);

    for (auto c : children)
      c->OnRender(g);
  }
};

//----------------------------------------------
// ���ּ���
//----------------------------------------------
void EngineLayout_PerformLayout(UIElement* root, float width, float height) {
  // ���ø����ִ�С
  YGNodeStyleSetWidth(root->node, (float)width);
  YGNodeStyleSetHeight(root->node, (float)height);

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);
}

//----------------------------------------------
// ȫ�� UI ��
//----------------------------------------------
UIElement* g_root = nullptr;

//----------------------------------------------
// ���ֳ�ʼ��
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
  EzUIDocParser parser;
  parser.ParseFile(L"SimpleEzUI.ezui");


  g_root = new UIElement("Root");
  YGNodeStyleSetFlexDirection(g_root->node, YGFlexDirectionRow);
  YGNodeStyleSetWidth(g_root->node, 800);
  YGNodeStyleSetHeight(g_root->node, 600);

  // �����
  auto sidebar = new UIElement("Sidebar");
  sidebar->color = Color(255, 255, 0, 0);
  YGNodeStyleSetWidth(sidebar->node, 200);
  // YGNodeStyleSetHeight(sidebar->node,  YGPercentValue(100));
  YGNodeStyleSetHeightPercent(sidebar->node, 100.0f);
  g_root->AddChild(sidebar);


  // ��������
  auto content = new UIElement("Content");
  content->color = Color(255, 0, 255, 0);
  YGNodeStyleSetFlexGrow(content->node, 1);
  YGNodeStyleSetFlexDirection(content->node, YGFlexDirectionColumn);
  g_root->AddChild(content);

  // �ϲ�
  auto top = new UIElement("Top");
  top->color = Color(255, 0, 0, 255);
  YGNodeStyleSetHeight(top->node, 200);
  content->AddChild(top);

  // �²����Զ���չ��
  auto bottom = new UIElement("Bottom");
  bottom->color = Color(255, 0, 0, 0);
  YGNodeStyleSetFlexGrow(bottom->node, 1);
  content->AddChild(bottom);
}

//----------------------------------------------
// ����
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