#include "pch.h"
#include "EzUIDocNode.h"

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

// 递归辅助函数
static void DumpDocTreeRecursive(const EzUIDocNode* node, int indent, std::wostringstream& oss) {
  if (!node) return;

  std::wstring pad(indent, L' ');

  // 输出开始标签和属性
  oss << pad << L"<" << node->name;

  for (const auto& [k, v] : node->attributes) {
    oss << L" " << k << L"=\"" << v << L"\"";
  }

  // 处理自闭合标签
  if (node->children.empty() && node->text.empty()) {
    oss << L"/>" << std::endl;
    return;
  }

  oss << L">";

  // 输出文本内容
  if (!node->text.empty()) {
    oss << node->text;
  }

  oss << std::endl;

  // 递归处理所有子节点
  for (const auto& child : node->children) {
    DumpDocTreeRecursive(child.get(), indent + 2, oss);
  }

  // 输出结束标签
  oss << pad << L"</" << node->name << L">" << std::endl;
}

std::wstring DumpDocTree(const EzUIDocNode* node) {
  if (!node) return L"";

  std::wostringstream oss;
  DumpDocTreeRecursive(node, 0, oss);
  return oss.str();
}