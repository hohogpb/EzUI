#include "pch.h"
#include "DumpInfo.h"
#include <stack>
#include <sstream>

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
  for (auto& child : root->children) {
    DumpYogaTree(child, depth + 1, os);
  }
}

// ===== 样式树 Dump 函数（外部函数）=====

namespace {
  // 递归输出树形结构
  void DumpStyleTreeRecursive(const EzUiStyleNode* node, std::wostream& os, int indent) {
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
  void DumpStyleTreeDetailedRecursive(const EzUiStyleNode* node, std::wostream& os, int indent) {
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
  void DumpStyleTreeStatsRecursive(const EzUiStyleNode* node, 
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
}

void DumpStyleTree(const EzUiStyleNode* styleNode, std::wostream& os) {
  if (!styleNode) return;
  DumpStyleTreeRecursive(styleNode, os, 0);
}

void DumpStyleTreeDetailed(const EzUiStyleNode* styleNode, std::wostream& os) {
  if (!styleNode) return;
  DumpStyleTreeDetailedRecursive(styleNode, os, 0);
}

void DumpStyleTreeStats(const EzUiStyleNode* styleNode, std::wostream& os) {
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
