#include "pch.h"
#include "DumpInfo.h"

// ===== 辅助：将 wstring 转 narrow 用于输出小工具 =====
static std::wstring Narrow(const std::wstring& w) {
  return std::wstring(w.begin(), w.end());
}

float safeValue(float v) {
  return std::isnan(v) ? 0.0f : v;
}

void PrintXmlTree(const EzUIDocNode* node, int indent) {
    std::wstring pad(indent, L' ');
    std::wcout << pad << L"<" << node->name;

    // 打印属性
    for (auto& [k, v] : node->attributes)
        std::wcout << L" " << k << L"=\"" << v << L"\"";

    if (node->children.empty() && node->text.empty()) {
        std::wcout << L"/>" << std::endl;
        return;
    }

    std::wcout << L">";
    if (!node->text.empty()) std::wcout << node->text;
    std::wcout << std::endl;

    for (auto& c : node->children)
        PrintXmlTree(c.get(), indent + 2);

    std::wcout << pad << L"</" << node->name << L">" << std::endl;
}

void PrintUITree(const UIElement* node, int indent) {
  std::wstring pad(indent, L' ');
  std::wcout << pad << node->name << std::endl;

#if 0
  // 打印属性
  for (auto& [k, v] : node->attributes)
    std::wcout << L" " << k << L"=\"" << v << L"\"";

  if (node->children.empty() && node->text.empty()) {
    std::wcout << L"/>" << std::endl;
    return;
  }

  std::wcout << L">";
  if (!node->text.empty()) std::wcout << node->text;
  std::wcout << std::endl;
#endif

  for (auto& c : node->children)
    PrintUITree(c, indent + 2);

  //std::wcout << pad << L"</" << node->name << L">" << std::endl;
}

// ===== Dump 单个节点的 style 与 layout 简要信息 =====
// 注意：Yoga API 提供了很多 style 获取函数，如果想把“所有 style”都读出来会很长。
// 这里以常用的属性为例输出：width/height/min/max, margin/padding, flex, justify/align, position, display, overflow
void DumpYogaNodeInfo(std::wostream& os, UIElement* node) {
  YGNodeRef n = node->ygNode;
  // layout
  float lx = safeValue(YGNodeLayoutGetLeft(n));
  float ly = safeValue(YGNodeLayoutGetTop(n));
  float lwidth = safeValue(YGNodeLayoutGetWidth(n));
  float lheight = safeValue(YGNodeLayoutGetHeight(n));

  // 简要 style（选择常用函数）
  // 宽高（注意：无法直接询问 "是否用百分比" 的设置，Yoga 没有统一的 getter for percent vs px for every property，
  // 这里只打印 layout 与一些直接可读的数值）
  os << "layout(x=" << lx << ", y=" << ly << ", w=" << lwidth << ", h=" << lheight << ")";

  // flex相关
  float flex = YGNodeStyleGetFlex(n);
  float flexGrow = YGNodeStyleGetFlexGrow(n);
  float flexShrink = YGNodeStyleGetFlexShrink(n);
  os << " flex=" << flex << " grow=" << flexGrow << " shrink=" << flexShrink;

  // justify / align / direction / display / overflow
  os << " justify=" << YGNodeStyleGetJustifyContent(n);
  os << " alignItems=" << YGNodeStyleGetAlignItems(n);
  os << " display=" << YGNodeStyleGetDisplay(n);
  os << " overflow=" << YGNodeStyleGetOverflow(n);
}

// ===== 递归打印整棵 UI/Yoga 树（depth 控制缩进） =====
void DumpYogaTree(UIElement* root, int depth, std::wostream& os) {
  if (!root) return;

  // 缩进
  for (int i = 0; i < depth; ++i) os << "  ";

  // 打印节点名
  os << Narrow(root->name);

  // 打印 Yog a layout & 部分 style 信息
  os << " => ";
  DumpYogaNodeInfo(os, root);
  os << std::endl;

  // 递归子节点
  for (auto child : root->children) {
    DumpYogaTree(child, depth + 1, os);
  }
}
