#include "pch.h"
#include "test_style_tree.h"
#include "layout-engine/EzUIDocParser.h"
// #include "layout-engine/EzUIElement.h"
//#include "layout-engine/EzUITreeBuilder.h"
#include "layout-engine/EzUITextLoader.h" 
#include <fcntl.h>
#include <io.h>
#include <layout-engine/EzUICssParser.h>
#include <layout-engine/EzUiStyleTreeBuilder.h>
#include <layout-engine/EzUiLayoutBuilder.h>
#include <layout-engine/EzUiStyledNode.h>


int main() {
  _setmode(_fileno(stdout), _O_U16TEXT);

  // 加载 HTML/XML 文档
  EzUITextLoader loader;
  auto docText = loader.Load(L"SimpleEzUI.ezui");
  auto domRoot = EzUIDocParser::Parse(docText);

  std::wcout << L"======== DOM 树 ========" << std::endl;
  PrintXmlTree(domRoot.get(), 0);
  std::wcout << std::endl;

  // 加载 CSS 样式表
  auto cssText = loader.Load(L"SimpleEzUI.css");
  auto stylesheet = EzUICssParser::Parse(cssText);

  std::wcout << L"======== 样式表信息 ========" << std::endl;
  std::wcout << L"规则数: " << stylesheet->rules.size() << std::endl;
  std::wcout << std::endl;

  // 构建样式树
  EzUiStyleTreeBuilder styleTreeBuilder;
  auto styleTree = styleTreeBuilder.Build(domRoot.get(), stylesheet.get());

  // 1. 基础 dump：简洁视图
  std::wcout << L"======== 样式树（简洁） ========" << std::endl;
  DumpStyleTree(styleTree.get());
  std::wcout << std::endl;

  // 2. 详细 dump：显示所有属性
  std::wcout << L"======== 样式树（详细） ========" << std::endl;
  DumpStyleTreeDetailed(styleTree.get());
  std::wcout << std::endl;

  // 3. 统计信息
  std::wcout << std::endl;
  DumpStyleTreeStats(styleTree.get());

  // 构建布局树   
  auto layoutTree = EzUiLayoutBuilder::BuildTree(styleTree.get());

  return 0;
}
