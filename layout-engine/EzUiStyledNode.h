#pragma once

#include <gdiplus.h>
#include "EzUIDocNode.h"
#include "EzUICssNode.h"
#include "EzUIHelper.h"
#include <ostream>

// Map from CSS property names to values.
using PropertyMap = std::unordered_map<std::wstring, std::wstring>;

struct EzUiStyledNode {
  EzUIDocNode* node;
  PropertyMap specifiedValues;
  std::vector<std::unique_ptr<EzUiStyledNode>> children;

  std::wstring docText;
  std::wstring text;
  std::wstring name;
  Gdiplus::Image* bgImage = nullptr;
  EzUI::Color color{ 0, 0, 0 };
  EzUI::Color bgColor{ 255 , 255, 255, 0 };
  float opacity = 1.0f;

};


using MatchedRule = std::pair<std::tuple<int, int, int>, Rule*>;



void DumpStyleTree(const EzUiStyledNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeDetailed(const EzUiStyledNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeStats(const EzUiStyledNode* styleNode, std::wostream& os = std::wcout);
