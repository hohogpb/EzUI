#include "pch.h"
#include "EzUiLayoutBuilder.h"
// #include "EzUIElementBuilder.h"
#include "EzUIPropertyResolvers.h"

static YGSize MeasureTextNode(YGNodeConstRef ygNode,
  float width, YGMeasureMode widthMode,
  float height, YGMeasureMode heightMode) {

  auto layoutNode = (EzUiLayoutBox*)YGNodeGetContext(ygNode);
  if (!layoutNode) return { 0, 0 };

  float maxWidth = (widthMode == YGMeasureModeUndefined) ? FLT_MAX : width;
  float maxHeight = (heightMode == YGMeasureModeUndefined) ? FLT_MAX : height;

  auto size = layoutNode->MesureText(maxWidth, maxHeight);

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

EzUiLayoutBox* EzUiLayoutBuilder::BuildTree(EzUiStyledNode* styleNode) {
  EzUiLayoutBuilder builder;
  return builder.Build(styleNode);
}

EzUiLayoutBox* EzUiLayoutBuilder::Build(EzUiStyledNode* styledNode) {
  auto layoutNode = BuildNode(styledNode);

  for (int i = 0; i < (int)styledNode->children.size(); i++) {
    auto& childStyledNode = styledNode->children[i];

    // svg 子节点不在入栈
    if (layoutNode->tag == L"svg") {
      continue;
    }

    // style 子节点不在入栈
    if (layoutNode->tag == L"style") {
      continue;
    }

    // 如果包含文字，就不处理子节点了
    if (!layoutNode->styleNode->node->text.empty()) {
      continue;
    }

    auto childLayoutNode = Build(childStyledNode.get());
    layoutNode->AddChild(childLayoutNode);
  }

  return layoutNode;
}

EzUiLayoutBox* EzUiLayoutBuilder::BuildNode(EzUiStyledNode* styleNode) {
  auto docNode = styleNode->node;

  auto layoutNode = new EzUiLayoutBox(docNode->name);
  layoutNode->styleNode = styleNode;

  auto ygNode = layoutNode->ygNode;

  if (docNode->name == L"svg") {
    //uiNode->docText = DumpDocTree(docNode);
    styleNode->docText = DumpDocTree(docNode);
  }

  if (!docNode->text.empty()) {
    // uiNode->text = docNode->text;
    styleNode->text = docNode->text;
    YGNodeSetMeasureFunc(ygNode, MeasureTextNode);
  }

  EzUIPropertyResolvers resolvers;

  for (auto& [keyRaw, valueRaw] : docNode->attributes) {
    resolvers.Resolve(keyRaw, valueRaw, styleNode, ygNode);
  }

  return layoutNode;
}




