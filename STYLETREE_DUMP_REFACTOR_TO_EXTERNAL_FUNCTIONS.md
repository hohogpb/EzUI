# ? StyleTree Dump - 外部函数实现完成

## ?? 实现变更

已将 StyleTree dump 功能从**成员方法**改为**独立的外部函数**。

---

## ?? 变更内容

### 变更前（成员方法）
```cpp
struct EzUiStyleNode {
  // ... 字段 ...
  
  // 成员方法（不推荐）
  void Dump(std::wostream& os = std::wcout, int indent = 0) const;
  void DumpDetailed(std::wostream& os = std::wcout, int indent = 0) const;
  void DumpStats(std::wostream& os = std::wcout) const;
};

// 使用方式
styleTree->Dump();
```

### 变更后（外部函数）
```cpp
struct EzUiStyleNode {
  // ... 字段 ...
  // 不含任何成员方法
};

// 外部函数（推荐）
void DumpStyleTree(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeDetailed(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeStats(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);

// 使用方式
DumpStyleTree(styleTree.get());
```

---

## ? 优势

### 1. 结构设计更清晰
```cpp
// ? 数据结构纯净
struct EzUiStyleNode {
  EzUIDocNode* node;
  PropertyMap specifiedValues;
  std::vector<std::unique_ptr<EzUiStyleNode>> children;
};
```

### 2. 功能独立分离
```cpp
// ? 调试函数单独在 DumpInfo 中
void DumpStyleTree(...);
void DumpStyleTreeDetailed(...);
void DumpStyleTreeStats(...);
```

### 3. 职责明确
- `EzUiStyleNode` - 纯数据结构
- `DumpInfo` - 调试工具集合

### 4. 易于扩展
```cpp
// 在 DumpInfo 中轻松添加新的 dump 函数
void DumpStyleTreeJSON(...);
void DumpStyleTreeXML(...);
void DumpStyleTreeCSV(...);
```

---

## ?? 文件修改

### 1. EzUiStyleNode.h
**变更**: 移除了 3 个成员方法声明

```cpp
struct EzUiStyleNode {
  EzUIDocNode* node;
  PropertyMap specifiedValues;
  std::vector<std::unique_ptr<EzUiStyleNode>> children;
  // ? 移除了三个 Dump 方法
};
```

### 2. EzUiStyleNode.cpp
**变更**: 清空文件，只保留基础 includes

```cpp
#include "pch.h"
#include "EzUiStyleNode.h"
```

### 3. DumpInfo.h
**变更**: 添加 3 个外部函数声明

```cpp
// ===== 样式树 Dump 函数（外部函数） =====
void DumpStyleTree(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeDetailed(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeStats(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
```

### 4. DumpInfo.cpp
**变更**: 实现 3 个外部函数和辅助函数

```cpp
namespace {
  void DumpStyleTreeRecursive(...);      // 递归树形输出
  void DumpStyleTreeDetailedRecursive(...); // 递归详细输出
  void DumpStyleTreeStatsRecursive(...);    // 递归统计计算
}

void DumpStyleTree(...);       // 公开接口
void DumpStyleTreeDetailed(...); // 公开接口
void DumpStyleTreeStats(...);    // 公开接口
```

### 5. test_style_tree.cpp
**变更**: 更新使用外部函数

```cpp
// 使用外部函数
DumpStyleTree(styleTree.get());
DumpStyleTreeDetailed(styleTree.get());
DumpStyleTreeStats(styleTree.get());
```

---

## ?? 使用方法

### 引入头文件

```cpp
#include <utils/DumpInfo.h>
```

### 基础用法

```cpp
// 简洁树形
DumpStyleTree(styleTree.get());

// 详细属性
DumpStyleTreeDetailed(styleTree.get());

// 统计信息
DumpStyleTreeStats(styleTree.get());
```

### 自定义输出流

```cpp
// 输出到文件
std::wofstream file(L"debug.txt");
DumpStyleTree(styleTree.get(), file);
DumpStyleTreeDetailed(styleTree.get(), file);
DumpStyleTreeStats(styleTree.get(), file);

// 输出到错误流
DumpStyleTree(styleTree.get(), std::wcerr);

// 输出到标准输出（默认）
DumpStyleTree(styleTree.get());
```

### 完整示例

```cpp
#include <utils/DumpInfo.h>

int main() {
  auto styleTree = styleTreeBuilder.Build(domRoot.get(), stylesheet.get());

  std::wcout << L"===== 简洁 =====\n";
  DumpStyleTree(styleTree.get());
  
  std::wcout << L"\n===== 详细 =====\n";
  DumpStyleTreeDetailed(styleTree.get());
  
  std::wcout << L"\n===== 统计 =====\n";
  DumpStyleTreeStats(styleTree.get());

  return 0;
}
```

---

## ?? 函数对比

| 函数 | 用途 | 输出量 |
|------|------|--------|
| `DumpStyleTree()` | 显示树形结构 | ?? 少 |
| `DumpStyleTreeDetailed()` | 显示所有属性 | ?? 中 |
| `DumpStyleTreeStats()` | 显示统计信息 | ?? 少 |

---

## ?? 输出示例

### DumpStyleTree()
```
<div id="main"> [styles: padding=20px; background=blue]
  <button> [styles: transition=all 0.3s ease]
  </button>
</div>
```

### DumpStyleTreeDetailed()
```
Node: div
  DOM Attributes:
    id="main"
  CSS Properties: (2 properties)
    padding = 20px
    background = blue

Node: button
  CSS Properties: (1 properties)
    transition = all 0.3s ease
```

### DumpStyleTreeStats()
```
======== 样式树统计 ========
节点总数: 2
样式属性总数: 3
平均每个节点的样式属性数: 1.5
树的最大深度: 2
```

---

## ? 编译验证

? **编译成功** - 0 错误，0 警告

所有文件编译通过：
- ? `EzUiStyleNode.h`
- ? `EzUiStyleNode.cpp`
- ? `DumpInfo.h`
- ? `DumpInfo.cpp`
- ? `test_style_tree.cpp`

---

## ?? 架构改进

### 前：混合设计
```
EzUiStyleNode (数据结构 + 调试方法)
├─ 数据字段
└─ Dump 方法  ? 混合职责
```

### 后：分离设计
```
EzUiStyleNode (纯数据结构)
├─ node
├─ specifiedValues
└─ children

DumpInfo (调试工具)
├─ DumpStyleTree()
├─ DumpStyleTreeDetailed()
└─ DumpStyleTreeStats()
```

---

## ?? 相关函数

在 `DumpInfo.h/cpp` 中还有其他调试函数：

```cpp
// 文档树
void PrintXmlTree(const EzUIDocNode* node, int indent = 0);
std::wstring DumpDocTree(const EzUIDocNode* node);

// UI 树
void PrintUITree(const UIElement* node, int indent = 0);

// Yoga 布局
void DumpYogaNodeInfo(std::wostream& os, UIElement* node);
void DumpYogaTree(UIElement* root, int depth = 0, std::wostream& os = std::wcout);

// 样式树（新增）
void DumpStyleTree(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeDetailed(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
void DumpStyleTreeStats(const EzUiStyleNode* styleNode, std::wostream& os = std::wcout);
```

---

## ?? 最佳实践

### 1. 调试工作流
```cpp
// 第一步：快速查看
DumpStyleTree(styleTree.get());

// 第二步：如果有问题，查看详细
DumpStyleTreeDetailed(styleTree.get());

// 第三步：分析性能
DumpStyleTreeStats(styleTree.get());
```

### 2. 条件编译
```cpp
#ifdef DEBUG_STYLE
  DumpStyleTreeDetailed(styleTree.get());
  DumpStyleTreeStats(styleTree.get());
#endif
```

### 3. 日志输出
```cpp
std::wofstream log(L"debug.log");
DumpStyleTree(styleTree.get(), log);
```

---

## ?? 迁移指南

如果之前有代码使用了成员方法：

### 旧代码（? 不再支持）
```cpp
styleTree->Dump();
styleTree->DumpDetailed();
styleTree->DumpStats();
```

### 新代码（? 使用外部函数）
```cpp
DumpStyleTree(styleTree.get());
DumpStyleTreeDetailed(styleTree.get());
DumpStyleTreeStats(styleTree.get());
```

---

## ?? 文档更新

之前生成的 STYLETREE_DUMP_*.md 文档已不完全适用，因为现在使用外部函数。
推荐使用新的使用方式：

```cpp
// 包含头文件
#include <utils/DumpInfo.h>

// 使用外部函数
DumpStyleTree(styleTree.get());
DumpStyleTreeDetailed(styleTree.get());
DumpStyleTreeStats(styleTree.get());
```

---

## ?? 总结

? **架构改进**: 数据结构和调试工具分离  
? **代码质量**: 职责更清晰  
? **可维护性**: 易于扩展新的 dump 函数  
? **编译状态**: 成功（0 错误，0 警告）  
? **生产就绪**: 是  

现在 StyleTree dump 功能采用更好的外部函数设计！??
