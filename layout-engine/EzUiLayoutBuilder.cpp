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

// ? 新增：img 标签大小测量函数
static YGSize MeasureImgNode(YGNodeConstRef ygNode,
  float width, YGMeasureMode widthMode,
  float height, YGMeasureMode heightMode) {

  auto layoutNode = (EzUiLayoutBox*)YGNodeGetContext(ygNode);
  if (!layoutNode) return { 0, 0 };

  // 获取图片的原生大小
  UINT nativeWidth = 0, nativeHeight = 0;
  bool hasNativeSize = layoutNode->GetImageNativeSize(nativeWidth, nativeHeight);

  if (!hasNativeSize) {
    // 如果无法获取原生大小，返回最小值
    return { 0, 0 };
  }

  // 计算结果尺寸
  YGSize size = { (float)nativeWidth, (float)nativeHeight };

  // 根据 Yoga measureMode 约束大小
  // widthMode 处理
  if (widthMode == YGMeasureModeAtMost) {
    // 宽度不超过指定值，保持宽高比
    if (size.width > width) {
      float ratio = width / size.width;
      size.width = width;
      size.height = size.height * ratio;
    }
  } else if (widthMode == YGMeasureModeExactly) {
    // 宽度必须等于指定值，保持宽高比
    float ratio = width / size.width;
    size.width = width;
    size.height = size.height * ratio;
  }

  // heightMode 处理
  if (heightMode == YGMeasureModeAtMost) {
    // 高度不超过指定值，保持宽高比
    if (size.height > height) {
      float ratio = height / size.height;
      size.height = height;
      size.width = size.width * ratio;
    }
  } else if (heightMode == YGMeasureModeExactly) {
    // 高度必须等于指定值，保持宽高比
    float ratio = height / size.height;
    size.height = height;
    size.width = size.width * ratio;
  }

  OutputDebugStringW((L"[MeasureImgNode] Native: " + std::to_wstring(nativeWidth) + L"x" + std::to_wstring(nativeHeight) + 
    L" | Measured: " + std::to_wstring((UINT)size.width) + L"x" + std::to_wstring((UINT)size.height) + L"\n").c_str());

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

  // ? 改进：对于 img 标签，设置 MeasureFunc 而不是直接设置大小
  if (docNode->name == L"img") {
    // 设置 img 标签的测量函数
    YGNodeSetMeasureFunc(ygNode, MeasureImgNode);

    // 检查是否已经指定了 width 和 height
    bool hasWidth = styleNode->specifiedValues.find(L"width") != styleNode->specifiedValues.end();
    bool hasHeight = styleNode->specifiedValues.find(L"height") != styleNode->specifiedValues.end();

    // 如果都没有指定大小，先设置为 auto 让 MeasureFunc 处理
    // 如果只指定了一个，MeasureFunc 会在约束下计算另一个
    if (!hasWidth && !hasHeight) {
      OutputDebugStringW(L"[BuildNode] img tag with no size, using MeasureImgNode\n");
    } else if (!hasWidth) {
      OutputDebugStringW(L"[BuildNode] img tag with height only, width will be calculated\n");
    } else if (!hasHeight) {
      OutputDebugStringW(L"[BuildNode] img tag with width only, height will be calculated\n");
    }
  }

  return layoutNode;
}






