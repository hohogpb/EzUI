#pragma once

#include <format>
#include <io.h>
#include <fcntl.h>
#include <layout-engine/EzUIDocNode.h>
#include <layout-engine/EzUIElement.h>
#include <layout-engine/EzUiStyleNode.h>


using std::stack;
using std::pair;


void PrintXmlTree(const EzUIDocNode* node, int indent = 0);

std::wstring DumpDocTree(const EzUIDocNode* node);

void PrintUITree(const UIElement* node, int indent = 0);

// ===== 样式树 Dump 函数（外部函数） =====

/// 基础 dump：显示树形结构
void DumpStyleTree(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);

/// 详细 dump：显示所有属性
void DumpStyleTreeDetailed(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);

/// 统计 dump：显示树的统计信息
void DumpStyleTreeStats(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);

// ===== Dump 节点的 style 和 layout 重要信息 =====
// 注意：Yoga API 提供了很多 style 获取函数，但许多查询效率不高，这里仅选择一些 style值较为常用和容易获得的字段
// 为简洁起见，仅建议输出关键属性：如 width/height/min/max, margin/padding, flex, justify/align, position, display, overflow
void DumpYogaNodeInfo(std::wostream& os, UIElement* node);

// ===== 递归打印整个 UI/Yoga 树，按depth 加缩进，同时显示 =====
void DumpYogaTree(UIElement* root, int depth = 0, std::wostream& os = std::wcout);