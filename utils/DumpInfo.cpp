#include "pch.h"
#include "DumpInfo.h"

// ===== �������� wstring ת narrow �������С���� =====
static std::wstring Narrow(const std::wstring& w) {
  return std::wstring(w.begin(), w.end());
}

float safeValue(float v) {
  return std::isnan(v) ? 0.0f : v;
}

void PrintXmlTree(const EzUIDocNode* node, int indent) {
    std::wstring pad(indent, L' ');
    std::wcout << pad << L"<" << node->name;

    // ��ӡ����
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
  // ��ӡ����
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

// ===== Dump �����ڵ�� style �� layout ��Ҫ��Ϣ =====
// ע�⣺Yoga API �ṩ�˺ܶ� style ��ȡ�����������ѡ����� style������������ܳ���
// �����Գ��õ�����Ϊ�������width/height/min/max, margin/padding, flex, justify/align, position, display, overflow
void DumpYogaNodeInfo(std::wostream& os, UIElement* node) {
  YGNodeRef n = node->ygNode;
  // layout
  float lx = safeValue(YGNodeLayoutGetLeft(n));
  float ly = safeValue(YGNodeLayoutGetTop(n));
  float lwidth = safeValue(YGNodeLayoutGetWidth(n));
  float lheight = safeValue(YGNodeLayoutGetHeight(n));

  // ��Ҫ style��ѡ���ú�����
  // ��ߣ�ע�⣺�޷�ֱ��ѯ�� "�Ƿ��ðٷֱ�" �����ã�Yoga û��ͳһ�� getter for percent vs px for every property��
  // ����ֻ��ӡ layout ��һЩֱ�ӿɶ�����ֵ��
  os << "layout(x=" << lx << ", y=" << ly << ", w=" << lwidth << ", h=" << lheight << ")";

  // flex���
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

// ===== �ݹ��ӡ���� UI/Yoga ����depth ���������� =====
void DumpYogaTree(UIElement* root, int depth, std::wostream& os) {
  if (!root) return;

  // ����
  for (int i = 0; i < depth; ++i) os << "  ";

  // ��ӡ�ڵ���
  os << Narrow(root->name);

  // ��ӡ Yog a layout & ���� style ��Ϣ
  os << " => ";
  DumpYogaNodeInfo(os, root);
  os << std::endl;

  // �ݹ��ӽڵ�
  for (auto child : root->children) {
    DumpYogaTree(child, depth + 1, os);
  }
}
