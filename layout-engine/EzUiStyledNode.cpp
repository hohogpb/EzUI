#include "pch.h"
#include "EzUiStyledNode.h"


// 递归输出树形结构
void DumpStyleTreeRecursive(const EzUiStyledNode* node, std::wostream& os, int indent) {
  if (!node || !node->node) return;

  std::wstring pad(indent, L' ');

  // 输出节点开标签
  os << pad << L"<" << node->node->name;

  // 输出 DOM 属性
  for (const auto& [k, v] : node->node->attributes) {
    os << L" " << k << L"=\"" << v << L"\"";
  }

  os << L">";

  // 输出样式属性
  if (!node->specifiedValues.empty()) {
    os << L" [styles: ";
    bool first = true;
    for (const auto& [prop, val] : node->specifiedValues) {
      if (!first) os << L"; ";
      os << prop << L"=" << val;
      first = false;
    }
    os << L"]";
  }

  os << std::endl;

  // 递归输出子节点
  for (const auto& child : node->children) {
    if (child) {
      DumpStyleTreeRecursive(child.get(), os, indent + 2);
    }
  }

  // 输出关闭标签
  os << pad << L"</" << node->node->name << L">" << std::endl;
}

// 递归输出详细信息
void DumpStyleTreeDetailedRecursive(const EzUiStyledNode* node, std::wostream& os, int indent) {
  if (!node || !node->node) return;

  std::wstring pad(indent, L' ');

  // 输出节点名称
  os << pad << L"Node: " << node->node->name << std::endl;

  // 输出 DOM 属性
  if (!node->node->attributes.empty()) {
    os << pad << L"  DOM Attributes:" << std::endl;
    for (const auto& [k, v] : node->node->attributes) {
      os << pad << L"    " << k << L"=\"" << v << L"\"" << std::endl;
    }
  }

  // 输出样式属性
  if (!node->specifiedValues.empty()) {
    os << pad << L"  CSS Properties: (" << node->specifiedValues.size() << L" properties)" << std::endl;
    for (const auto& [prop, val] : node->specifiedValues) {
      os << pad << L"    " << prop << L" = " << val << std::endl;
    }
  }

  os << std::endl;

  // 递归输出子节点
  for (const auto& child : node->children) {
    if (child) {
      DumpStyleTreeDetailedRecursive(child.get(), os, indent + 2);
    }
  }
}

// 递归计算统计信息
void DumpStyleTreeStatsRecursive(const EzUiStyledNode* node,
  int& nodeCount,
  int& totalPropertyCount,
  int& maxDepth,
  int currentDepth) {
  if (!node) return;

  nodeCount++;
  totalPropertyCount += node->specifiedValues.size();
  maxDepth = (std::max)(maxDepth, currentDepth);

  for (const auto& child : node->children) {
    if (child) {
      DumpStyleTreeStatsRecursive(child.get(), nodeCount, totalPropertyCount, maxDepth, currentDepth + 1);
    }
  }
}


void DumpStyleTree(const EzUiStyledNode* styleNode, std::wostream& os) {
  if (!styleNode) return;
  DumpStyleTreeRecursive(styleNode, os, 0);
}

void DumpStyleTreeDetailed(const EzUiStyledNode* styleNode, std::wostream& os) {
  if (!styleNode) return;
  DumpStyleTreeDetailedRecursive(styleNode, os, 0);
}

void DumpStyleTreeStats(const EzUiStyledNode* styleNode, std::wostream& os) {
  if (!styleNode) return;

  int nodeCount = 0;
  int totalPropertyCount = 0;
  int maxDepth = 0;

  DumpStyleTreeStatsRecursive(styleNode, nodeCount, totalPropertyCount, maxDepth, 0);

  os << L"======== 样式树统计 ========" << std::endl;
  os << L"节点总数: " << nodeCount << std::endl;
  os << L"样式属性总数: " << totalPropertyCount << std::endl;
  os << L"平均每个节点的样式属性数: "
    << (nodeCount > 0 ? (float)totalPropertyCount / nodeCount : 0) << std::endl;
  os << L"树的最大深度: " << maxDepth << std::endl;
  os << std::endl;
}

// ===== Dump 节点的 style 和 layout 重要信息 =====
