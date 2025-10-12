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
// ���ּ���
//----------------------------------------------
void EngineLayout_PerformLayout(UIElement* root, float width, float height) {
  // ���ø����ִ�С
  YGNodeStyleSetWidth(root->node, (float)width);
  YGNodeStyleSetHeight(root->node, (float)height);

  YGNodeCalculateLayout(root->node, YGUndefined, YGUndefined, YGDirectionLTR);

#if 1
  // �ݹ���� absolute rect���ۼӸ�ƫ�ƣ�
  std::function<void(UIElement*, float, float)> update =
    [&](UIElement* elem, float parentAbsX, float parentAbsY) {
    // ȡ�ֲ�����ֵ������ڸ��ڵ㣩
    float localLeft = YGNodeLayoutGetLeft(elem->node);
    float localTop = YGNodeLayoutGetTop(elem->node);
    float localWidth = YGNodeLayoutGetWidth(elem->node);
    float localHeight = YGNodeLayoutGetHeight(elem->node);

    // �������λ�ã��ۼӸ�ƫ�ƣ�
    float absX = parentAbsX + localLeft;
    float absY = parentAbsY + localTop;

    // ������ UIElement ʹ�� RECT / int ֵ�洢������ת�����������룩
    elem->rect.x = absX;
    elem->rect.y = absY;
    elem->rect.w = localWidth;
    elem->rect.h = localHeight;

    // �ݹ����ӽڵ�
    for (auto child : elem->children) {
      update(child, absX, absY);
    }
  };

  // ���ڵ㸸ƫ��Ϊ 0,0����������ø��� margin ���޸ģ�
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
// ȫ�� UI ��
//----------------------------------------------
UIElement* g_root = nullptr;

//----------------------------------------------
// ���ֳ�ʼ��
//----------------------------------------------
void EngineLayout_InitUILayout(EzUIWindow* wnd) {
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