#include "pch.h"
#include "EzUIPropertyResolvers.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <yoga/Yoga.h>
#include <algorithm>
#include <iostream>
#include <cwctype> 

using std::unordered_map;
using std::wstring;
using std::function;

inline std::string Narrow(const wstring& w) {
  return std::string(w.begin(), w.end());
}

inline float ParseFloat(const std::wstring& val) {
  try { return std::stof(Narrow(val)); } catch (...) { return 0.0f; }
}

inline bool IsPercent(const std::wstring& val) {
  return val.find(L"%") != std::wstring::npos;
}

inline std::wstring ToLower(const std::wstring& s) {
  std::wstring out = s;
  std::transform(out.begin(), out.end(), out.begin(),
    [](wchar_t c) { return std::towlower(c); });
  return out;
}

Color ParseColor(const std::wstring& str) {
  Color c{ 255, 255, 255, 255 };
  if (str.empty()) return c;
  std::string s(str.begin(), str.end());
  if (s[0] == '#') {
    unsigned int hex = std::stoul(s.substr(1), nullptr, 16);
    if (s.size() == 7) { // #RRGGBB
      BYTE r = ((hex >> 16) & 0xFF);
      BYTE g = ((hex >> 8) & 0xFF);
      BYTE b = (hex & 0xFF);
      BYTE a = 255;
      c = Color(a, r, g, b);
    } else if (s.size() == 9) { // #RRGGBBAA
      BYTE r = ((hex >> 24) & 0xFF);
      BYTE g = ((hex >> 16) & 0xFF);
      BYTE b = ((hex >> 8) & 0xFF);
      BYTE a = (hex & 0xFF);
      c = Color(a, r, g, b);
    }
  }
  return c;
}

static unordered_map<wstring, YGAlign> alignMap = {
  {L"auto", YGAlignAuto},
  {L"flex-start", YGAlignFlexStart},
  {L"center", YGAlignCenter},
  {L"flex-end", YGAlignFlexEnd},
  {L"stretch", YGAlignStretch},
  {L"baseline", YGAlignBaseline},
  {L"space-between", YGAlignSpaceBetween},
  {L"space-around", YGAlignSpaceAround},
};

static unordered_map<wstring, YGJustify> justifyMap = {
  {L"flex-start", YGJustifyFlexStart},
  {L"center", YGJustifyCenter},
  {L"flex-end", YGJustifyFlexEnd},
  {L"space-between", YGJustifySpaceBetween},
  {L"space-around", YGJustifySpaceAround},
  {L"space-evenly", YGJustifySpaceEvenly},
};

static unordered_map<wstring, YGFlexDirection> flexDirMap = {
  {L"row", YGFlexDirectionRow},
  {L"row-reverse", YGFlexDirectionRowReverse},
  {L"column", YGFlexDirectionColumn},
  {L"column-reverse", YGFlexDirectionColumnReverse},
};

static unordered_map<wstring, YGPositionType> positionTypeMap = {
  {L"relative", YGPositionTypeRelative},
  {L"absolute", YGPositionTypeAbsolute},
};

static unordered_map<wstring, YGDisplay> displayMap = {
  {L"flex", YGDisplayFlex},
  {L"none", YGDisplayNone},
};

static unordered_map<wstring, YGOverflow> overflowMap = {
  {L"visible", YGOverflowVisible},
  {L"hidden", YGOverflowHidden},
  {L"scroll", YGOverflowScroll},
};

static unordered_map<wstring, YGEdge> ygEdgeDict = {
  { L"left", YGEdgeLeft },
  { L"right", YGEdgeRight },
  { L"top", YGEdgeTop },
  { L"bottom", YGEdgeBottom },
  { L"start", YGEdgeStart },
  { L"end", YGEdgeEnd },
};


// 1. 定义属性解析器接口
// value uiNode
using PropertyResolver = function<void(const wstring&, const wstring&, UIElement*, YGNodeRef)>;

// ---- 辅助: 设置可为百分比或绝对值的属性 ----
template<typename Setter, typename SetterPercent>
void SetMaybePercent(YGNodeRef node, const wstring& val, Setter setFunc, SetterPercent setPercentFunc) {
  float num = ParseFloat(val);
  if (IsPercent(val)) setPercentFunc(node, num);
  else setFunc(node, num);
}

static void colorResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  Color c = ParseColor(val);
  uiNode->color = c;
}

static void nameResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  // 这里 name 属性已经在 UIElement 构造函数中设置过了
}

static void flexDirectionResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  auto lowercaseVal = ToLower(val);
  YGNodeStyleSetFlexDirection(ygNode, flexDirMap[lowercaseVal]);
}

static void widthResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetWidth(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetWidthPercent(n, v); });
}

static void heightResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetHeight(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetHeightPercent(n, v); });
}

static void posResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  auto edge = ygEdgeDict[key];
  SetMaybePercent(ygNode, val,
    [edge](YGNodeRef n, float v) { YGNodeStyleSetPosition(n, edge, v); },
    [edge](YGNodeRef n, float v) { YGNodeStyleSetPositionPercent(n, edge, v); });
}

static void positionResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetPositionType(ygNode, positionTypeMap[val]);
}

static void paddingResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetPadding(n, YGEdgeAll, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetPaddingPercent(n, YGEdgeAll, v); });

}

static void marginResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetMargin(n, YGEdgeAll, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetMarginPercent(n, YGEdgeAll, v); });
}

static void justifyContentResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetJustifyContent(ygNode, justifyMap[val]);
}

static void alignItemsResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAlignItems(ygNode, alignMap[val]);
}

static void alignSelfResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAlignSelf(ygNode, alignMap[val]);
}

static void alignContentResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAlignContent(ygNode, alignMap[val]);
}

static void backgroundResolver(const wstring& key, const wstring& val, UIElement* uiNode, YGNodeRef ygNode) {
  
}

// 3. 声明并初始化 propertyResolverDict
static unordered_map<wstring, PropertyResolver> propertyResolverDict = {
  // 可按需注册具体属性解析器 
  { L"color", colorResolver},
  { L"name", nameResolver},
  { L"flex-direction", flexDirectionResolver},
  { L"width", widthResolver },
  { L"height", heightResolver },
  { L"left", posResolver },
  { L"top", posResolver },
  { L"right", posResolver },
  { L"bottom", posResolver },
  { L"position", positionResolver},
  { L"padding", paddingResolver},
  { L"margin", marginResolver},
  { L"justify-content", justifyContentResolver},
  { L"align-items", alignItemsResolver},
  { L"align-self", alignSelfResolver},
  { L"align-content", alignContentResolver},
  { L"background", backgroundResolver},
  // ...
};

static void ensurePropertyResolved(const wstring& key) {
  if (!propertyResolverDict.contains(key)) {
    throw std::runtime_error("No resolver for property: " + Narrow(key));
  }
}

void EzUIPropertyResolvers::Resolve(const std::wstring& keyRaw, const std::wstring& valRaw, UIElement* uiNode, YGNodeRef ygNode) {
  std::wstring key = ToLower(keyRaw);

  ensurePropertyResolved(key);

  propertyResolverDict[key](key, valRaw, uiNode, ygNode);
}
