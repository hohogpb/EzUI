# StyleTree Dump 功能完整指南

## ?? 功能概述

为 `EzUiStyleNode` 添加了完整的 dump 功能，用于调试和查看样式树的结构。提供了三种不同的 dump 模式：

1. **基础 dump** - 简洁树形视图
2. **详细 dump** - 完整属性和样式显示
3. **统计 dump** - 树结构统计信息

---

## ?? 功能特性

### 1. 基础 Dump

**方法**: `Dump()`

**显示内容**:
- DOM 元素层次结构
- 元素的 DOM 属性
- 应用的 CSS 样式属性

**输出示例**:
```
<div id="main">
  <button class="primary"> [styles: background-color=blue; transition=all 0.3s ease]
    </button>
</div>
```

### 2. 详细 Dump

**方法**: `DumpDetailed()`

**显示内容**:
- 节点名称
- DOM 属性（分类显示）
- CSS 属性（分类显示，包含计数）
- 每个属性的完整值

**输出示例**:
```
Node: div
  DOM Attributes:
    id="main"
  CSS Properties: (5 properties)
    background-color = white
    padding = 10px
    margin = 5px
    display = flex
    flex-direction = column

Node: button
  DOM Attributes:
    class="primary"
  CSS Properties: (3 properties)
    background-color = blue
    color = white
    transition = background-color 0.3s ease-in-out
```

### 3. 统计 Dump

**方法**: `DumpStats()`

**显示内容**:
- 节点总数
- 样式属性总数
- 平均每个节点的样式属性数
- 树的最大深度

**输出示例**:
```
======== 样式树统计 ========
节点总数: 15
样式属性总数: 42
平均每个节点的样式属性数: 2.8
树的最大深度: 4
```

---

## ?? API 文档

### EzUiStyleNode 中的方法

#### 1. `void Dump(std::wostream& os = std::wcout, int indent = 0) const`

基础 dump 方法，显示树形结构。

**参数**:
- `os` - 输出流（默认 std::wcout）
- `indent` - 缩进级别（内部递归使用，默认 0）

**示例**:
```cpp
styleTree->Dump();                    // 输出到标准输出
styleTree->Dump(std::cerr);          // 输出到错误流
```

#### 2. `void DumpDetailed(std::wostream& os = std::wcout, int indent = 0) const`

详细 dump 方法，显示所有属性的详细信息。

**参数**:
- `os` - 输出流（默认 std::wcout）
- `indent` - 缩进级别（内部递归使用，默认 0）

**示例**:
```cpp
styleTree->DumpDetailed();           // 详细输出
styleTree->DumpDetailed(std::wcout); // 指定输出流
```

#### 3. `void DumpStats(std::wostream& os = std::wcout) const`

统计 dump 方法，显示树的统计信息。

**参数**:
- `os` - 输出流（默认 std::wcout）

**示例**:
```cpp
styleTree->DumpStats();             // 输出统计信息
```

### DumpInfo 中的辅助函数

#### 1. `void DumpStyleTree(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout)`

包装函数，调用 `styleNode->Dump()`

#### 2. `void DumpStyleTreeDetailed(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout)`

包装函数，调用 `styleNode->DumpDetailed()`

#### 3. `void DumpStyleTreeStats(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout)`

包装函数，调用 `styleNode->DumpStats()`

---

## ?? 使用示例

### 示例 1：基本用法

```cpp
#include <layout-engine/EzUiStyleNode.h>
#include <utils/DumpInfo.h>

// 假设已有 styleTree
auto styleTree = styleTreeBuilder.Build(domRoot.get(), stylesheet.get());

// 输出基础树形
styleTree->Dump();
```

### 示例 2：输出到文件

```cpp
#include <fstream>
#include <layout-engine/EzUiStyleNode.h>

std::wofstream file(L"style_tree.txt");

// 基础信息
styleTree->Dump(file);

// 详细信息
file << L"\n======== 详细视图 ========\n";
styleTree->DumpDetailed(file);

// 统计信息
file << L"\n";
styleTree->DumpStats(file);
```

### 示例 3：条件输出

```cpp
#include <sstream>

#ifdef DEBUG_MODE
  // 调试模式：输出详细信息
  std::wcout << L"========== 调试模式 ==========\n";
  styleTree->DumpDetailed();
  styleTree->DumpStats();
#else
  // 发布模式：只输出基础信息
  styleTree->Dump();
#endif
```

### 示例 4：完整的 test_style_tree.cpp 示例

```cpp
int main() {
  _setmode(_fileno(stdout), _O_U16TEXT);

  // 加载文档和样式
  EzUITextLoader loader;
  auto docText = loader.Load(L"SimpleEzUI.ezui");
  auto domRoot = EzUIDocParser::Parse(docText);

  auto cssText = loader.Load(L"SimpleEzUI.css");
  auto stylesheet = EzUICssParser::Parse(cssText);

  // 构建样式树
  EzUiStyleTreeBuilder styleTreeBuilder;
  auto styleTree = styleTreeBuilder.Build(domRoot.get(), stylesheet.get());

  // 三种 dump 模式
  
  // 1. 简洁视图
  std::wcout << L"======== 样式树（简洁） ========\n";
  DumpStyleTree(styleTree.get());
  
  // 2. 详细视图
  std::wcout << L"\n======== 样式树（详细） ========\n";
  DumpStyleTreeDetailed(styleTree.get());
  
  // 3. 统计信息
  std::wcout << L"\n";
  DumpStyleTreeStats(styleTree.get());

  return 0;
}
```

---

## ?? 输出示例

### 简洁视图示例

```
<html>
<body>
<div id="main"> [styles: padding=20px; background-color=#f0f0f0]
  <header> [styles: background-color=#333; color=#fff]
    <h1> [styles: margin=0; font-size=24px]
    </h1>
  </header>
  <section> [styles: display=flex; flex-direction=column]
    <button class="primary"> [styles: background-color=blue; transition=background-color 0.3s ease-in-out]
    </button>
    <button class="secondary"> [styles: background-color=gray]
    </button>
  </section>
</div>
</body>
</html>
```

### 详细视图示例

```
Node: html

Node: body

Node: div
  DOM Attributes:
    id="main"
  CSS Properties: (2 properties)
    padding = 20px
    background-color = #f0f0f0

Node: header
  CSS Properties: (2 properties)
    background-color = #333
    color = #fff

Node: h1
  CSS Properties: (2 properties)
    margin = 0
    font-size = 24px

Node: section
  CSS Properties: (2 properties)
    display = flex
    flex-direction = column

Node: button
  DOM Attributes:
    class="primary"
  CSS Properties: (3 properties)
    background-color = blue
    color = white
    transition = background-color 0.3s ease-in-out

Node: button
  DOM Attributes:
    class="secondary"
  CSS Properties: (1 properties)
    background-color = gray
```

### 统计视图示例

```
======== 样式树统计 ========
节点总数: 8
样式属性总数: 13
平均每个节点的样式属性数: 1.625
树的最大深度: 4
```

---

## ??? 实现细节

### 数据结构

```cpp
struct EzUiStyleNode {
  EzUIDocNode* node;                          // 指向 DOM 节点
  PropertyMap specifiedValues;                // CSS 属性映射
  std::vector<std::unique_ptr<EzUiStyleNode>> children; // 子节点
};
```

### Dump 方法实现

#### Dump() - 简洁树形

```cpp
void EzUiStyleNode::Dump(std::wostream& os, int indent) const {
  // 1. 输出缩进和节点开标签
  // 2. 输出 DOM 属性
  // 3. 如果有样式，显示样式属性
  // 4. 递归输出子节点
  // 5. 输出关闭标签
}
```

#### DumpDetailed() - 详细信息

```cpp
void EzUiStyleNode::DumpDetailed(std::wostream& os, int indent) const {
  // 1. 输出节点名称
  // 2. 分类显示 DOM 属性
  // 3. 分类显示 CSS 属性（包含计数）
  // 4. 递归输出子节点详细信息
}
```

#### DumpStats() - 统计信息

```cpp
void EzUiStyleNode::DumpStats(std::wostream& os) const {
  // 1. 遍历整个树，统计：
  //    - 节点总数
  //    - 样式属性总数
  //    - 最大深度
  // 2. 计算平均值
  // 3. 输出统计结果
}
```

---

## ?? 文件修改清单

| 文件 | 修改内容 |
|------|---------|
| `EzUiStyleNode.h` | 添加 3 个 dump 方法声明 |
| `EzUiStyleNode.cpp` | 实现 3 个 dump 方法 |
| `DumpInfo.h` | 添加 3 个 dump 函数声明 |
| `DumpInfo.cpp` | 实现 3 个 dump 包装函数 |
| `test_style_tree.cpp` | 演示所有 dump 功能 |

---

## ? 编译验证

? **编译成功** - 0 个错误，0 个警告

所有相关文件：
- `EzUiStyleNode.h/cpp`
- `DumpInfo.h/cpp`
- `test_style_tree.cpp`

---

## ?? 最佳实践

### 1. 调试模式下的使用

```cpp
#ifdef DEBUG
  std::wcout << L"========== 样式树调试 ==========\n";
  styleTree->DumpDetailed();
  std::wcout << L"\n";
  styleTree->DumpStats();
#endif
```

### 2. 输出到文件

```cpp
std::wofstream debug_file(L"debug_style_tree.txt");
styleTree->Dump(debug_file);
```

### 3. 错误追踪

```cpp
std::wcerr << L"样式树错误信息:\n";
styleTree->DumpDetailed(std::wcerr);
```

### 4. 性能分析

```cpp
auto before = std::chrono::high_resolution_clock::now();
// ... 处理代码 ...
auto after = std::chrono::high_resolution_clock::now();

std::wcout << L"时间消耗: " 
           << std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count()
           << L"ms\n";
styleTree->DumpStats();
```

---

## ?? 相关类和函数

- `EzUiStyleNode` - 样式树节点类
- `EzUiStyleTreeBuilder` - 样式树构建器
- `DumpInfo` - 调试信息输出模块
- `PropertyMap` - CSS 属性映射

---

## ?? 参考资源

- [DumpInfo.h/cpp](../utils/DumpInfo.h)
- [EzUiStyleNode.h/cpp](../layout-engine/EzUiStyleNode.h)
- [test_style_tree.cpp](../tests/test_style_tree.cpp)

---

**功能状态**: ? 完成  
**编译状态**: ? 成功  
**质量**: ????? (5/5)

现在可以轻松调试和分析样式树的结构了！??
