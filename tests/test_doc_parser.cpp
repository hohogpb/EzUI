#include "pch.h"
#include "test_doc_parser.h"
#include "layout-engine/EzUIDocParser.h"
//#include "layout-engine/EzUIElement.h"
//#include "layout-engine/EzUITreeBuilder.h"
#include <fcntl.h>
#include <io.h> 
#include "layout-engine/EzUITextLoader.h"

#if 0
int main() {
  //SetConsoleOutputCP(CP_UTF8);        // 或者 CP_UTF16
  _setmode(_fileno(stdout), _O_U16TEXT);

  EzUITextLoader loader;
  auto source = loader.Load(L"SimpleEzUI.ezui");
  
  auto domRoot = EzUIDocParser::Parse(source);   

  PrintXmlTree(domRoot.get());

  EzUITreeBuilder builder;
  auto uiRoot = builder.Build(domRoot.get());

  PrintUITree(uiRoot);

#if 0
  YGNodeStyleSetWidth(uiRoot->ygNode, (float)800);
  YGNodeStyleSetHeight(uiRoot->ygNode, (float)600);

  YGNodeCalculateLayout(uiRoot->ygNode, YGUndefined, YGUndefined, YGDirectionLTR);

  PrintUITree(uiRoot);

  DumpYogaTree(uiRoot);


  // 创建根节点
  YGNodeRef root = YGNodeNew();
  YGNodeStyleSetWidth(root, 800);
  YGNodeStyleSetHeight(root, 600);
  YGNodeStyleSetFlexDirection(root, YGFlexDirectionColumn);

  // Layer A
  YGNodeRef layerA = YGNodeNew();
  YGNodeStyleSetPositionType(layerA, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(layerA, YGEdgeLeft, 0);
  YGNodeStyleSetPosition(layerA, YGEdgeTop, 0);
  YGNodeStyleSetWidthPercent(layerA, 100);
  YGNodeStyleSetHeightPercent(layerA, 100);

  // Layer B
  YGNodeRef layerB = YGNodeNew();
  YGNodeStyleSetPositionType(layerB, YGPositionTypeAbsolute);
  YGNodeStyleSetPosition(layerA, YGEdgeLeft, 0);
  YGNodeStyleSetPosition(layerA, YGEdgeTop, 0);
  YGNodeStyleSetWidthPercent(layerB, 100);
  YGNodeStyleSetHeightPercent(layerB, 100);

  // 挂到 root
  YGNodeInsertChild(root, layerA, 0);
  YGNodeInsertChild(root, layerB, 1);

  // 计算布局
  YGNodeCalculateLayout(root, YGUndefined, YGUndefined, YGDirectionLTR);

  // 打印结果
  wprintf(L"LayerA: left=%.1f top=%.1f w=%.1f h=%.1f\n",
    YGNodeLayoutGetLeft(layerA),
    YGNodeLayoutGetTop(layerA),
    YGNodeLayoutGetWidth(layerA),
    YGNodeLayoutGetHeight(layerA));

  wprintf(L"LayerB: left=%.1f top=%.1f w=%.1f h=%.1f\n",
    YGNodeLayoutGetLeft(layerB),
    YGNodeLayoutGetTop(layerB),
    YGNodeLayoutGetWidth(layerB),
    YGNodeLayoutGetHeight(layerB));

  YGNodeFreeRecursive(root);
#endif
  return 0;
}
#endif