#include "pch.h"
#include "EzUIElementBuilder.h"
#include "EzUIPropertyResolvers.h" 

// 5. 在 Build 方法中调用前确保解析器存在
UIElement* EzUIElementBuilder::Build(EzUIDocNode* docNode) {
  auto uiNode = new UIElement(docNode->name);
  auto ygNode = uiNode->ygNode;

  EzUIPropertyResolvers resolvers;

  for (auto& [keyRaw, valueRaw] : docNode->attributes) {
    resolvers.Resolve(keyRaw, valueRaw, uiNode, ygNode);
    

 

#if 0
    // ===== 边距 / 内边距 / 边框 / 位置 =====
 

    // ===== Flexbox =====
 else if (key == L"flex-wrap") {
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


        // ===== 尺寸 =====
      else if (key == L"min-width") {
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
            } else else if (key == L"border") {
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

#endif
  }

  return uiNode;
}


