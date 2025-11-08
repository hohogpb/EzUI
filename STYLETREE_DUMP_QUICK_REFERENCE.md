# StyleTree Dump 快速参考

## ?? 快速开始

### 最简单的用法

```cpp
// 基础输出 - 树形结构
styleTree->Dump();
```

### 三种输出模式

```cpp
// 1. 简洁 - 树形结构视图
styleTree->Dump();

// 2. 详细 - 显示所有属性
styleTree->DumpDetailed();

// 3. 统计 - 树的统计信息
styleTree->DumpStats();
```

## ?? 输出对比

| 模式 | 用途 | 输出量 |
|------|------|--------|
| `Dump()` | 快速查看树结构 | ?? 少 |
| `DumpDetailed()` | 深度调试 | ?? 中 |
| `DumpStats()` | 性能分析 | ?? 少 |

## ?? 典型使用场景

### 场景 1：快速检查样式是否正确应用

```cpp
styleTree->Dump();
// 查看元素是否有预期的样式属性
```

### 场景 2：调试为什么某个样式没有生效

```cpp
styleTree->DumpDetailed();
// 检查该元素的 CSS 属性列表
```

### 场景 3：性能优化

```cpp
styleTree->DumpStats();
// 检查树的复杂度，找出性能瓶颈
```

### 场景 4：生成调试报告

```cpp
std::wofstream file(L"debug_report.txt");
styleTree->Dump(file);
file << L"\n";
styleTree->DumpDetailed(file);
file << L"\n";
styleTree->DumpStats(file);
```

## ?? 输出示例

### Dump() 输出

```
<body>
<div class="container"> [styles: padding=20px]
  <button> [styles: background-color=blue]
  </button>
</div>
</body>
```

### DumpDetailed() 输出

```
Node: div
  DOM Attributes:
    class="container"
  CSS Properties: (1 properties)
    padding = 20px

Node: button
  CSS Properties: (1 properties)
    background-color = blue
```

### DumpStats() 输出

```
======== 样式树统计 ========
节点总数: 3
样式属性总数: 2
平均每个节点的样式属性数: 0.667
树的最大深度: 2
```

## ?? 完整示例

```cpp
#include <layout-engine/EzUiStyleNode.h>
#include <utils/DumpInfo.h>

int main() {
  // ... 加载和构建样式树 ...
  
  auto styleTree = styleTreeBuilder.Build(domRoot.get(), stylesheet.get());

  // 三个命令看完整的调试信息
  std::wcout << L"======== 简洁 ========\n";
  styleTree->Dump();
  
  std::wcout << L"\n======== 详细 ========\n";
  styleTree->DumpDetailed();
  
  std::wcout << L"\n======== 统计 ========\n";
  styleTree->DumpStats();

  return 0;
}
```

## ?? 关键方法

| 方法 | 签名 | 描述 |
|------|------|------|
| `Dump()` | `void Dump(std::wostream& os = std::wcout, int indent = 0) const` | 树形输出 |
| `DumpDetailed()` | `void DumpDetailed(std::wostream& os = std::wcout, int indent = 0) const` | 详细输出 |
| `DumpStats()` | `void DumpStats(std::wostream& os = std::wcout) const` | 统计输出 |

## ?? 提示

- 使用 `Dump()` 快速检查树结构
- 使用 `DumpDetailed()` 深度调试
- 使用 `DumpStats()` 性能分析
- 所有方法都支持输出到文件或流

## ?? 相关函数

DumpInfo 中的辅助函数：
- `DumpStyleTree(styleNode)` → 调用 `Dump()`
- `DumpStyleTreeDetailed(styleNode)` → 调用 `DumpDetailed()`
- `DumpStyleTreeStats(styleNode)` → 调用 `DumpStats()`

## ? 功能清单

- ? 树形结构输出
- ? 详细属性输出
- ? 统计信息输出
- ? 自定义输出流
- ? 完全递归支持
- ? 宽字符支持

---

**版本**: 1.0  
**状态**: ? 就绪  
**最后更新**: 2024
