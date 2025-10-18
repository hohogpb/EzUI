#pragma once

#include <format>
#include <io.h>
#include <fcntl.h>
#include <layout-engine/EzUIDocNode.h>
#include <layout-engine/EzUIElement.h>


using std::stack;
using std::pair;


void PrintXmlTree(const EzUIDocNode* node, int indent = 0);

std::wstring DumpDocTree(const EzUIDocNode* node);

void PrintUITree(const UIElement* node, int indent = 0);

// ===== Dump 单个节点的 style 与 layout 简要信息 =====
// 注意：Yoga API 提供了很多 style 获取函数，如果想把“所有 style”都读出来会很长。
// 这里以常用的属性为例输出：width/height/min/max, margin/padding, flex, justify/align, position, display, overflow
void DumpYogaNodeInfo(std::wostream& os, UIElement* node);

// ===== 递归打印整棵 UI/Yoga 树（depth 控制缩进） =====
void DumpYogaTree(UIElement* root, int depth = 0, std::wostream& os = std::wcout);