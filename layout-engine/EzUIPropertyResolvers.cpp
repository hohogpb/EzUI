#include "pch.h"
#include "EzUIPropertyResolvers.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <yoga/Yoga.h>
#include <algorithm>
#include <iostream>
#include <cwctype> 
#include "EzUIHelper.h"

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

// Helper to trim whitespace
inline std::wstring Trim(const std::wstring& s) {
  const wchar_t* WHITESPACE = L" \n\r\t\f\v";
  size_t first = s.find_first_not_of(WHITESPACE);
  if (std::wstring::npos == first) {
    return s;
  }
  size_t last = s.find_last_not_of(WHITESPACE);
  return s.substr(first, (last - first + 1));
}

// HSL to RGB conversion helper
EzUI::Color HslToRgb(float h, float s, float l, float a) {
  float r, g, b;
  if (s == 0.0f) {
    r = g = b = l; // achromatic
  } else {
    auto hue2rgb = [](float p, float q, float t) {
      if (t < 0.0f) t += 1.0f;
      if (t > 1.0f) t -= 1.0f;
      if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
      if (t < 1.0f / 2.0f) return q;
      if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
      return p;
    };
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    r = hue2rgb(p, q, h + 1.0f / 3.0f);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 1.0f / 3.0f);
  }
  return { (BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255), (BYTE)(a * 255) };
}

static const std::unordered_map<std::wstring, EzUI::Color> namedColors = {
  {L"black",       {  0,   0,   0, 255}},
  {L"silver",      {192, 192, 192, 255}},
  {L"gray",        {128, 128, 128, 255}},
  {L"white",       {255, 255, 255, 255}},
  {L"maroon",      {128,   0,   0, 255}},
  {L"red",         {255,   0,   0, 255}},
  {L"purple",      {128,   0, 128, 255}},
  {L"fuchsia",     {255,   0, 255, 255}},
  {L"green",       {  0, 128,   0, 255}},
  {L"lime",        {  0, 255,   0, 255}},
  {L"olive",       {128, 128,   0, 255}},
  {L"yellow",      {255, 255,   0, 255}},
  {L"navy",        {  0,   0, 128, 255}},
  {L"blue",        {  0,   0, 255, 255}},
  {L"teal",        {  0, 128, 128, 255}},
  {L"aqua",        {  0, 255, 255, 255}},
  {L"transparent", {  0,    0,  0,   0}}
};

EzUI::Color ParseColor(const std::wstring& str) {
  std::wstring lowerStr = ToLower(Trim(str));
  if (lowerStr.empty()) return { 255, 255, 255, 255 };

  // 1. Named colors
  auto it = namedColors.find(lowerStr);
  if (it != namedColors.end()) {
    return it->second;
  }

  // 2. Hexadecimal
  if (lowerStr[0] == L'#') {
    std::string s = Narrow(lowerStr);
    unsigned int hex;
    try {
      hex = std::stoul(s.substr(1), nullptr, 16);
    } catch (...) {
      return { 255, 255, 255, 255 };
    }
    BYTE r, g, b, a = 255;
    switch (s.size()) {
    case 4: // #RGB
      r = ((hex >> 8) & 0xF) * 17;
      g = ((hex >> 4) & 0xF) * 17;
      b = (hex & 0xF) * 17;
      break;
    case 5: // #RGBA
      r = ((hex >> 12) & 0xF) * 17;
      g = ((hex >> 8) & 0xF) * 17;
      b = ((hex >> 4) & 0xF) * 17;
      a = (hex & 0xF) * 17;
      break;
    case 7: // #RRGGBB
      r = (hex >> 16) & 0xFF;
      g = (hex >> 8) & 0xFF;
      b = hex & 0xFF;
      break;
    case 9: // #RRGGBBAA
      r = (hex >> 24) & 0xFF;
      g = (hex >> 16) & 0xFF;
      b = (hex >> 8) & 0xFF;
      a = hex & 0xFF;
      break;
    default:
      return { 255, 255, 255, 255 };
    }
    return { r, g, b, a };
  }

  // 3. rgb, rgba, hsl, hsla
  size_t openParen = lowerStr.find(L'(');
  size_t closeParen = lowerStr.rfind(L')');
  if (openParen != std::wstring::npos && closeParen != std::wstring::npos) {
    std::wstring type = lowerStr.substr(0, openParen);
    std::wstring valuesStr = lowerStr.substr(openParen + 1, closeParen - openParen - 1);
    std::vector<std::wstring> values;
    std::wstringstream wss(valuesStr);
    std::wstring value;
    while (std::getline(wss, value, L',')) {
      values.push_back(Trim(value));
    }

    try {
      if (type == L"rgb" && values.size() == 3) {
        return { (BYTE)std::stoi(values[0]), (BYTE)std::stoi(values[1]), (BYTE)std::stoi(values[2]), 255 };
      }
      if (type == L"rgba" && values.size() == 4) {
        return { (BYTE)std::stoi(values[0]), (BYTE)std::stoi(values[1]), (BYTE)std::stoi(values[2]), (BYTE)(std::stof(values[3]) * 255) };
      }
      if (type == L"hsl" && values.size() == 3) {
        float h = std::stof(values[0]) / 360.0f;
        float s = std::stof(values[1].substr(0, values[1].find(L'%'))) / 100.0f;
        float l = std::stof(values[2].substr(0, values[2].find(L'%'))) / 100.0f;
        return HslToRgb(h, s, l, 1.0f);
      }
      if (type == L"hsla" && values.size() == 4) {
        float h = std::stof(values[0]) / 360.0f;
        float s = std::stof(values[1].substr(0, values[1].find(L'%'))) / 100.0f;
        float l = std::stof(values[2].substr(0, values[2].find(L'%'))) / 100.0f;
        float a = std::stof(values[3]);
        return HslToRgb(h, s, l, a);
      }
    } catch (...) {
      return { 255, 255, 255, 255 };
    }
  }

  return { 255, 255, 255, 255 }; // Default fallback
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

static unordered_map<wstring, YGEdge> marginEdgeDict = {
  { L"margin-left", YGEdgeLeft },
  { L"margin-right", YGEdgeRight },
  { L"margin-top", YGEdgeTop },
  { L"margin-bottom", YGEdgeBottom },
};

static unordered_map<wstring, YGEdge> paddingEdgeDict = {
  { L"padding-left", YGEdgeLeft },
  { L"padding-right", YGEdgeRight },
  { L"padding-top", YGEdgeTop },
  { L"padding-bottom", YGEdgeBottom },
};

static unordered_map<wstring, YGEdge> borderEdgeDict = {
  { L"border-left", YGEdgeLeft },
  { L"border-right", YGEdgeRight },
  { L"border-top", YGEdgeTop },
  { L"border-bottom", YGEdgeBottom },
};

// 1. 定义属性解析器接口
// value uiNode
using PropertyResolver = function<void(const wstring&, const wstring&, EzUiStyledNode*, YGNodeRef)>;

// ---- 辅助: 设置可为百分比或绝对值的属性 ----
template<typename Setter, typename SetterPercent>
void SetMaybePercent(YGNodeRef node, const wstring& val, Setter setFunc, SetterPercent setPercentFunc) {
  float num = ParseFloat(val);
  if (IsPercent(val)) setPercentFunc(node, num);
  else setFunc(node, num);
}

static void colorResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  EzUI::Color c = ParseColor(val);
  uiNode->color = c;
}

static void nameResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  uiNode->name = val;
}

static void flexDirectionResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  auto lowercaseVal = ToLower(val);
  YGNodeStyleSetFlexDirection(ygNode, flexDirMap[lowercaseVal]);
}

static void widthResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetWidth(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetWidthPercent(n, v); });
}

static void heightResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetHeight(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetHeightPercent(n, v); });
}

static void minWidthResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetMinWidth(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetMinWidthPercent(n, v); });
}

static void minHeightResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetMinHeight(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetMinHeightPercent(n, v); });
}

static void maxWidthResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetMaxWidth(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetMaxWidthPercent(n, v); });
}

static void maxHeightResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetMaxHeight(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetMaxHeightPercent(n, v); });
}

static void posResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  auto edge = ygEdgeDict[key];
  SetMaybePercent(ygNode, val,
    [edge](YGNodeRef n, float v) { YGNodeStyleSetPosition(n, edge, v); },
    [edge](YGNodeRef n, float v) { YGNodeStyleSetPositionPercent(n, edge, v); });
}

static void positionResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetPositionType(ygNode, positionTypeMap[val]);
}

static void paddingResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetPadding(n, YGEdgeAll, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetPaddingPercent(n, YGEdgeAll, v); });

}

static void marginResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetMargin(n, YGEdgeAll, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetMarginPercent(n, YGEdgeAll, v); });
}

static void justifyContentResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetJustifyContent(ygNode, justifyMap[val]);
}

static void alignItemsResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAlignItems(ygNode, alignMap[val]);
}

static void alignSelfResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAlignSelf(ygNode, alignMap[val]);
}

static void alignContentResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAlignContent(ygNode, alignMap[val]);
}

static void backgroundColorResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  EzUI::Color c = ParseColor(val);
  uiNode->bgColor = c;
}

static void backgroundImageResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  auto lowerVal = ToLower(val);
  if (lowerVal.rfind(L"url(", 0) == 0) { // Starts with url(
    size_t start = val.find(L'(') + 1;
    size_t end = val.rfind(L')');
    if (start != std::wstring::npos && end != std::wstring::npos) {
      std::wstring path = val.substr(start, end - start);
      // Trim quotes
      if (path.length() >= 2 && path.front() == L'"' && path.back() == L'"') {
        path = path.substr(1, path.length() - 2);
      }
      delete uiNode->bgImage;
      uiNode->bgImage = new Gdiplus::Image(path.c_str());
      if (uiNode->bgImage->GetLastStatus() != Gdiplus::Ok) {
        delete uiNode->bgImage;
        uiNode->bgImage = nullptr;
      }
    }
  } else if (lowerVal == L"none") {
    delete uiNode->bgImage;
    uiNode->bgImage = nullptr;
  }
}

static void backgroundResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  // Shorthand: try to parse as image first, then as color.
  auto lowerVal = ToLower(val);
  if (lowerVal.rfind(L"url(", 0) == 0 || lowerVal == L"none") {
    backgroundImageResolver(key, val, uiNode, ygNode);
  } else {
    backgroundColorResolver(key, val, uiNode, ygNode);
  }
}

static void flexResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  std::wstring trimmedVal = Trim(ToLower(val));

  if (trimmedVal == L"none") {
    YGNodeStyleSetFlexGrow(ygNode, 0);
    YGNodeStyleSetFlexShrink(ygNode, 0);
    YGNodeStyleSetFlexBasisAuto(ygNode);
    return;
  }
  if (trimmedVal == L"auto") {
    YGNodeStyleSetFlexGrow(ygNode, 1);
    YGNodeStyleSetFlexShrink(ygNode, 1);
    YGNodeStyleSetFlexBasisAuto(ygNode);
    return;
  }

  std::wstringstream wss(trimmedVal);
  std::vector<std::wstring> parts;
  std::wstring part;
  while (wss >> part) {
    parts.push_back(part);
  }

  if (parts.empty()) return;

  // Default values
  float flexGrow = 0.0f;
  float flexShrink = 1.0f;
  bool basisIsAuto = true;
  float basisValue = 0.0f;
  bool basisIsPercent = false;

  if (parts.size() == 1) {
    const auto& p1 = parts[0];
    if (IsPercent(p1) || p1.find(L"px") != std::wstring::npos) { // e.g., "50%" or "100px"
      flexGrow = 1.0f;
      flexShrink = 1.0f;
      basisIsAuto = false;
      basisValue = ParseFloat(p1);
      basisIsPercent = IsPercent(p1);
    } else { // e.g., "2"
      flexGrow = ParseFloat(p1);
      flexShrink = 1.0f;
      basisIsAuto = false; // flex-basis defaults to 0%
      basisValue = 0.0f;
      basisIsPercent = true;
    }
  } else if (parts.size() == 2) {
    const auto& p1 = parts[0];
    const auto& p2 = parts[1];
    flexGrow = ParseFloat(p1);
    if (IsPercent(p2) || p2.find(L"px") != std::wstring::npos || p2 == L"auto") { // e.g., "1 50%" or "1 auto"
      flexShrink = 1.0f;
      if (p2 == L"auto") {
        basisIsAuto = true;
      } else {
        basisIsAuto = false;
        basisValue = ParseFloat(p2);
        basisIsPercent = IsPercent(p2);
      }
    } else { // e.g., "2 3"
      flexShrink = ParseFloat(p2);
      basisIsAuto = false; // flex-basis defaults to 0%
      basisValue = 0.0f;
      basisIsPercent = true;
    }
  } else if (parts.size() >= 3) {
    const auto& p1 = parts[0];
    const auto& p2 = parts[1];
    const auto& p3 = parts[2];
    flexGrow = ParseFloat(p1);
    flexShrink = ParseFloat(p2);
    if (p3 == L"auto") {
      basisIsAuto = true;
    } else {
      basisIsAuto = false;
      basisValue = ParseFloat(p3);
      basisIsPercent = IsPercent(p3);
    }
  }

  YGNodeStyleSetFlexGrow(ygNode, flexGrow);
  YGNodeStyleSetFlexShrink(ygNode, flexShrink);
  if (basisIsAuto) {
    YGNodeStyleSetFlexBasisAuto(ygNode);
  } else {
    if (basisIsPercent) {
      YGNodeStyleSetFlexBasisPercent(ygNode, basisValue);
    } else {
      YGNodeStyleSetFlexBasis(ygNode, basisValue);
    }
  }
}


static void flexWrapResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  if (val == L"nowrap")
    YGNodeStyleSetFlexWrap(ygNode, YGWrapNoWrap);
  else if (val == L"wrap")
    YGNodeStyleSetFlexWrap(ygNode, YGWrapWrap);
  else if (val == L"wrap-reverse")
    YGNodeStyleSetFlexWrap(ygNode, YGWrapWrapReverse);
}

static void flexGrowResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetFlexGrow(ygNode, ParseFloat(val));
}

static void flexShrinkResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetFlexShrink(ygNode, ParseFloat(val));
}

static void flexBasisResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  SetMaybePercent(ygNode, val,
    [](YGNodeRef n, float v) { YGNodeStyleSetFlexBasis(n, v); },
    [](YGNodeRef n, float v) { YGNodeStyleSetFlexBasisPercent(n, v); });
}

static void marginPartResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  auto edge = marginEdgeDict[key];
  SetMaybePercent(ygNode, val,
    [edge](YGNodeRef n, float v) { YGNodeStyleSetMargin(n, edge, v); },
    [edge](YGNodeRef n, float v) { YGNodeStyleSetMarginPercent(n, edge, v); });
}

static void paddingPartResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  auto edge = paddingEdgeDict[key];
  SetMaybePercent(ygNode, val,
    [edge](YGNodeRef n, float v) { YGNodeStyleSetPadding(n, edge, v); },
    [edge](YGNodeRef n, float v) { YGNodeStyleSetPaddingPercent(n, edge, v); });
}

static void borderResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetBorder(ygNode, YGEdgeAll, ParseFloat(val));
}

static void borderPartResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  auto edge = borderEdgeDict[key];
  YGNodeStyleSetBorder(ygNode, edge, ParseFloat(val));
}

static void overflowResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  if (overflowMap.contains(val))
    YGNodeStyleSetOverflow(ygNode, overflowMap[val]);
}

static void displayResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  if (displayMap.contains(val))
    YGNodeStyleSetDisplay(ygNode, displayMap[val]);
}

static void aspectRatioResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetAspectRatio(ygNode, ParseFloat(val));
}

static void gapResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetGap(ygNode, YGGutterAll, ParseFloat(val));
}

static void rowGapResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetGap(ygNode, YGGutterRow, ParseFloat(val));
}

static void columnGapResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  YGNodeStyleSetGap(ygNode, YGGutterColumn, ParseFloat(val));
}

static void versionResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {

}

static void viewBoxResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {

}

static void classResolver(const wstring& key, const wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {

}

static void opacityResolver(const std::wstring& key, const std::wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  float opacity = ParseFloat(val);
  // Clamp opacity to [0, 1]
  if (opacity < 0.0f) opacity = 0.0f;
  if (opacity > 1.0f) opacity = 1.0f;
  uiNode->opacity = opacity;
}

static void srcResolver(const std::wstring& key, const std::wstring& val, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  
}


// 3. 声明并初始化 propertyResolverDict
static unordered_map<wstring, PropertyResolver> propertyResolverDict = {
  // 可按需注册具体属性解析器 
  { L"color", colorResolver},
  { L"name", nameResolver},
  { L"flex-direction", flexDirectionResolver},
  { L"width", widthResolver },
  { L"height", heightResolver },
  { L"min-width", minWidthResolver },
  { L"min-height", minHeightResolver },
  { L"max-width", maxWidthResolver },
  { L"max-height", maxHeightResolver },
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
  { L"background-color", backgroundColorResolver},
  { L"background-image", backgroundImageResolver},
  { L"flex", flexResolver},
  { L"flex-wrap", flexWrapResolver},
  { L"flex-grow", flexGrowResolver},
  { L"flex-shrink", flexShrinkResolver},
  { L"flex-basis", flexBasisResolver},
  { L"margin-left", marginPartResolver},
  { L"margin-right", marginPartResolver},
  { L"margin-top", marginPartResolver},
  { L"margin-bottom", marginPartResolver},
  { L"padding-left", paddingPartResolver},
  { L"padding-right", paddingPartResolver},
  { L"padding-top", paddingPartResolver},
  { L"padding-bottom", paddingPartResolver},
  { L"border", borderResolver},
  { L"border-left", borderPartResolver},
  { L"border-right", borderPartResolver},
  { L"border-top", borderPartResolver},
  { L"border-bottom", borderPartResolver},
  { L"overflow", overflowResolver},
  { L"display", displayResolver},
  { L"aspect-ratio", aspectRatioResolver},
  { L"gap", gapResolver},
  { L"row-gap", rowGapResolver},
  { L"column-gap", columnGapResolver},

  // for svg
  { L"version", versionResolver},
  { L"viewbox", viewBoxResolver},
  // ...
  { L"class", classResolver},
  { L"opacity", opacityResolver},
  { L"src", srcResolver},
};


static void ensurePropertyResolved(const wstring& key) {
  if (!propertyResolverDict.contains(key)) {
    throw std::runtime_error("No resolver for property: " + Narrow(key));
  }
}

void EzUIPropertyResolvers::Resolve(const std::wstring& keyRaw, const std::wstring& valRaw, EzUiStyledNode* uiNode, YGNodeRef ygNode) {
  std::wstring key = ToLower(keyRaw);

  ensurePropertyResolved(key);

  propertyResolverDict[key](key, valRaw, uiNode, ygNode);
}
