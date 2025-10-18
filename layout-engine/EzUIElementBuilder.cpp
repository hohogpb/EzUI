#include "pch.h"
#include "EzUIElementBuilder.h"
#include "EzUIPropertyResolvers.h" 
#include <utils/DumpInfo.h>

using Microsoft::WRL::ComPtr;

extern ComPtr<IDWriteFactory> gDWriteFactory;

YGSize MeasureTextNode(YGNodeConstRef ygNode,
  float width, YGMeasureMode widthMode,
  float height, YGMeasureMode heightMode) {

  auto uiNode = (UIElement*)YGNodeGetContext(ygNode);
  if (!uiNode) return { 0, 0 };

  float maxWidth = (widthMode == YGMeasureModeUndefined) ? FLT_MAX : width;
  float maxHeight = (heightMode == YGMeasureModeUndefined) ? FLT_MAX : height;

  auto size = uiNode->MesureText(maxWidth, maxHeight);

  // 根据 Yoga measureMode 约束大小
  if (widthMode == YGMeasureModeAtMost)
    size.width = min(size.width, width);
  else if (widthMode == YGMeasureModeExactly)
    size.width = width;

  if (heightMode == YGMeasureModeAtMost)
    size.height = min(size.height, height);
  else if (heightMode == YGMeasureModeExactly)
    size.height = height;

  return size;
}

// 5. 在 Build 方法中调用前确保解析器存在
UIElement* EzUIElementBuilder::Build(EzUIDocNode* docNode) {
  auto uiNode = new UIElement(docNode->name);
  auto ygNode = uiNode->ygNode;

  if (docNode->name == L"svg") {
    uiNode->docText = DumpDocTree(docNode);
  }

  if (!docNode->text.empty()) {
    uiNode->text = docNode->text;
    YGNodeSetMeasureFunc(ygNode, MeasureTextNode);
  }

  EzUIPropertyResolvers resolvers;

  for (auto& [keyRaw, valueRaw] : docNode->attributes) {
    resolvers.Resolve(keyRaw, valueRaw, uiNode, ygNode);
  }




  return uiNode;
}


