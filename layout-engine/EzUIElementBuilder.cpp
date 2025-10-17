#include "pch.h"
#include "EzUIElementBuilder.h"
#include "EzUIPropertyResolvers.h" 

// 5. �� Build �����е���ǰȷ������������
UIElement* EzUIElementBuilder::Build(EzUIDocNode* docNode) {
  auto uiNode = new UIElement(docNode->name);
  auto ygNode = uiNode->ygNode;

  EzUIPropertyResolvers resolvers;

  for (auto& [keyRaw, valueRaw] : docNode->attributes) {
    resolvers.Resolve(keyRaw, valueRaw, uiNode, ygNode);
    

 

#if 0
    // ===== �߾� / �ڱ߾� / �߿� / λ�� =====
 

    // ===== Flexbox =====
 else 
      // ===== ���� =====


        // ===== �ߴ� =====
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

            // ===== �߾� / �ڱ߾� / �߿� / λ�� =====
           

            // ===== ���� =====

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


