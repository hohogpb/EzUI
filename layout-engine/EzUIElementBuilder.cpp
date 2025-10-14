#include "pch.h"
#include "EzUIElementBuilder.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <yoga/Yoga.h>
#include <algorithm>
#include <iostream>
#include <cwctype> 

inline std::string Narrow(const std::wstring& w) {
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

static std::unordered_map<std::wstring, YGAlign> alignMap = {
  {L"auto", YGAlignAuto},
  {L"flex-start", YGAlignFlexStart},
  {L"center", YGAlignCenter},
  {L"flex-end", YGAlignFlexEnd},
  {L"stretch", YGAlignStretch},
  {L"baseline", YGAlignBaseline},
  {L"space-between", YGAlignSpaceBetween},
  {L"space-around", YGAlignSpaceAround},
};

static std::unordered_map<std::wstring, YGJustify> justifyMap = {
  {L"flex-start", YGJustifyFlexStart},
  {L"center", YGJustifyCenter},
  {L"flex-end", YGJustifyFlexEnd},
  {L"space-between", YGJustifySpaceBetween},
  {L"space-around", YGJustifySpaceAround},
  {L"space-evenly", YGJustifySpaceEvenly},
};

static std::unordered_map<std::wstring, YGFlexDirection> flexDirMap = {
  {L"row", YGFlexDirectionRow},
  {L"row-reverse", YGFlexDirectionRowReverse},
  {L"column", YGFlexDirectionColumn},
  {L"column-reverse", YGFlexDirectionColumnReverse},
};

static std::unordered_map<std::wstring, YGPositionType> posTypeMap = {
  {L"relative", YGPositionTypeRelative},
  {L"absolute", YGPositionTypeAbsolute},
};

static std::unordered_map<std::wstring, YGDisplay> displayMap = {
  {L"flex", YGDisplayFlex},
  {L"none", YGDisplayNone},
};

static std::unordered_map<std::wstring, YGOverflow> overflowMap = {
  {L"visible", YGOverflowVisible},
  {L"hidden", YGOverflowHidden},
  {L"scroll", YGOverflowScroll},
};

// ---- 辅助: 设置可为百分比或绝对值的属性 ----
template<typename Setter, typename SetterPercent>
void SetMaybePercent(YGNodeRef node, const std::wstring& val, Setter setFunc, SetterPercent setPercentFunc) {
  float num = ParseFloat(val);
  if (IsPercent(val)) setPercentFunc(node, num);
  else setFunc(node, num);
}

UIElement* EzUIElementBuilder::Build(EzUIDocNode* docNode) {
  auto uiNode = new UIElement(docNode->name);

  for (auto& [keyRaw, valueRaw] : docNode->attributes) {
    std::wstring key = ToLower(keyRaw);
    std::wstring val = ToLower(valueRaw);

    // ===== Flexbox =====
    if (key == L"flex-direction" && flexDirMap.contains(val)) {
      YGNodeStyleSetFlexDirection(uiNode->ygNode, flexDirMap[val]);
    } else if (key == L"flex-wrap") {
      if (val == L"nowrap") YGNodeStyleSetFlexWrap(uiNode->ygNode, YGWrapNoWrap);
      else if (val == L"wrap") YGNodeStyleSetFlexWrap(uiNode->ygNode, YGWrapWrap);
      else if (val == L"wrap-reverse") YGNodeStyleSetFlexWrap(uiNode->ygNode, YGWrapWrapReverse);
    } else if (key == L"flex-grow") {
      YGNodeStyleSetFlexGrow(uiNode->ygNode, ParseFloat(val));
    } else if (key == L"flex-shrink") {
      YGNodeStyleSetFlexShrink(uiNode->ygNode, ParseFloat(val));
    } else if (key == L"flex-basis") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetFlexBasis(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetFlexBasisPercent(n, v); });
    }
    // ===== 对齐 =====
    else if (key == L"justify-content" && justifyMap.contains(val)) {
      YGNodeStyleSetJustifyContent(uiNode->ygNode, justifyMap[val]);
    } else if (key == L"align-items" && alignMap.contains(val)) {
      YGNodeStyleSetAlignItems(uiNode->ygNode, alignMap[val]);
    } else if (key == L"align-self" && alignMap.contains(val)) {
      YGNodeStyleSetAlignSelf(uiNode->ygNode, alignMap[val]);
    } else if (key == L"align-content" && alignMap.contains(val)) {
      YGNodeStyleSetAlignContent(uiNode->ygNode, alignMap[val]);
    }

    // ===== 尺寸 =====
    else if (key == L"width") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetWidth(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetWidthPercent(n, v); });
    } else if (key == L"height") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetHeight(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetHeightPercent(n, v); });
    } else if (key == L"min-width") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetMinWidth(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetMinWidthPercent(n, v); });
    } else if (key == L"min-height") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetMinHeight(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetMinHeightPercent(n, v); });
    } else if (key == L"max-width") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetMaxWidth(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetMaxWidthPercent(n, v); });
    } else if (key == L"max-height") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetMaxHeight(n, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetMaxHeightPercent(n, v); });
    } else if (key == L"margin") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetMargin(n, YGEdgeAll, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetMarginPercent(n, YGEdgeAll, v); });
    } else if (key == L"padding") {
      SetMaybePercent(uiNode->ygNode, val,
        [](YGNodeRef n, float v) { YGNodeStyleSetPadding(n, YGEdgeAll, v); },
        [](YGNodeRef n, float v) { YGNodeStyleSetPaddingPercent(n, YGEdgeAll, v); });
    } else if (key == L"border") {
      YGNodeStyleSetBorder(uiNode->ygNode, YGEdgeAll, ParseFloat(val));
    } else if (key == L"color") {
      Color c = ParseColor(val);
      uiNode->color = c;
    }

    // ===== 边距 / 内边距 / 边框 / 位置 =====
    static const std::vector<std::pair<std::wstring, YGEdge>> edges = {
        {L"left", YGEdgeLeft}, {L"right", YGEdgeRight},
        {L"top", YGEdgeTop}, {L"bottom", YGEdgeBottom},
        {L"start", YGEdgeStart}, {L"end", YGEdgeEnd},
    };

    for (auto [suffix, edge] : edges) {
      if (key == L"margin-" + suffix)
        SetMaybePercent(uiNode->ygNode, val,
          [edge](YGNodeRef n, float v) { YGNodeStyleSetMargin(n, edge, v); },
          [edge](YGNodeRef n, float v) { YGNodeStyleSetMarginPercent(n, edge, v); });

      if (key == L"padding-" + suffix)
        SetMaybePercent(uiNode->ygNode, val,
          [edge](YGNodeRef n, float v) { YGNodeStyleSetPadding(n, edge, v); },
          [edge](YGNodeRef n, float v) { YGNodeStyleSetPaddingPercent(n, edge, v); });

      if (key == L"border-" + suffix)
        YGNodeStyleSetBorder(uiNode->ygNode, edge, ParseFloat(val));

      if (key == suffix) {// 位置 
        SetMaybePercent(uiNode->ygNode, val,
          [edge](YGNodeRef n, float v) { YGNodeStyleSetPosition(n, edge, v); },
          [edge](YGNodeRef n, float v) { YGNodeStyleSetPositionPercent(n, edge, v); });
      }
    }

    // ===== 其他 =====
    if (key == L"position" && posTypeMap.contains(val))
      YGNodeStyleSetPositionType(uiNode->ygNode, posTypeMap[val]);
    else if (key == L"overflow" && overflowMap.contains(val))
      YGNodeStyleSetOverflow(uiNode->ygNode, overflowMap[val]);
    else if (key == L"display" && displayMap.contains(val))
      YGNodeStyleSetDisplay(uiNode->ygNode, displayMap[val]);
    else if (key == L"aspect-ratio")
      YGNodeStyleSetAspectRatio(uiNode->ygNode, ParseFloat(val));
    else if (key == L"row-gap")
      YGNodeStyleSetGap(uiNode->ygNode, YGGutterRow, ParseFloat(val));
    else if (key == L"column-gap")
      YGNodeStyleSetGap(uiNode->ygNode, YGGutterColumn, ParseFloat(val));


  }

  return uiNode;
}


