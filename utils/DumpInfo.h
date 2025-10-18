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

// ===== Dump �����ڵ�� style �� layout ��Ҫ��Ϣ =====
// ע�⣺Yoga API �ṩ�˺ܶ� style ��ȡ�����������ѡ����� style������������ܳ���
// �����Գ��õ�����Ϊ�������width/height/min/max, margin/padding, flex, justify/align, position, display, overflow
void DumpYogaNodeInfo(std::wostream& os, UIElement* node);

// ===== �ݹ��ӡ���� UI/Yoga ����depth ���������� =====
void DumpYogaTree(UIElement* root, int depth = 0, std::wostream& os = std::wcout);