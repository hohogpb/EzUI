# ?? 修复：img src 属性无法识别问题

## 问题描述

当在 HTML 中指定 `<img src="image.png" />` 时，绘制时无法找到图片源，导致图片无法显示。

```xml
<!-- ? src 属性没有被识别 -->
<img src="photo.jpg" width="200" height="150" />
```

## 根本原因

在样式树构建过程中，**HTML 属性（如 `src`）没有被添加到 `specifiedValues` 中**。

### 问题流程

```
EzUIDocNode
  ├─ 正确解析了 src 属性 ?
  └─ 存储在 attributes 中 ?
       └─ 传递给 EzUiStyleTreeBuilder
            └─ Build() 方法 ? 忽略了 HTML 属性
                 └─ 只处理 CSS 规则
                      └─ specifiedValues 没有 src
                           └─ GetImageSource() 找不到 src ?
```

### 受影响的代码路径

```cpp
// EngineLayoutRender.cpp
if (tag == L"img") {
  auto imageSrc = aLayoutNode->GetImageSource();  // ← 返回空字符串
  if (!imageSrc.empty()) {
    // 这里永远不会执行
  }
}

// EzUiLayoutBox.cpp
std::wstring EzUiLayoutBox::GetImageSource() const {
  auto srcIt = styleNode->specifiedValues.find(L"src");  // ← 找不到 src
  if (srcIt != styleNode->specifiedValues.end()) {
    return srcIt->second;
  }
  return L"";  // ← 总是返回空字符串
}
```

---

## 解决方案

### 修改 EzUiStyleTreeBuilder::Build()

在构建样式树时，**将 HTML 属性添加到 `specifiedValues`** 中（作为最低优先级，CSS 规则可以覆盖）。

#### 修改前 ?

```cpp
std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::Build(EzUIDocNode* docNode, Stylesheet* stylesheet) {
  auto styled = std::make_unique<EzUiStyledNode>();
  styled->node = docNode;

  if (docNode) {
    // ? 只处理 CSS 规则，忽略 HTML 属性
    styled->specifiedValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Normal);
    
    styled->hoverValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Hover);
    styled->focusValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Focus);
  }

  for (auto& child : docNode->children) {
    styled->children.push_back(Build(child.get(), stylesheet));
  }

  return styled;
}
```

#### 修改后 ?

```cpp
std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::Build(EzUIDocNode* docNode, Stylesheet* stylesheet) {
  auto styled = std::make_unique<EzUiStyledNode>();
  styled->node = docNode;

  if (docNode) {
    // ? 首先添加 HTML 属性（最低优先级）
    for (const auto& [attrName, attrValue] : docNode->attributes) {
      styled->specifiedValues[attrName] = attrValue;
    }

    // CSS 规则会覆盖 HTML 属性
    PropertyMap cssValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Normal);
    for (const auto& [propName, propValue] : cssValues) {
      styled->specifiedValues[propName] = propValue;
    }
    
    styled->hoverValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Hover);
    styled->focusValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Focus);
  }

  for (auto& child : docNode->children) {
    styled->children.push_back(Build(child.get(), stylesheet));
  }

  return styled;
}
```

### 关键改进

| 方面 | 修改前 | 修改后 |
|------|--------|--------|
| HTML 属性处理 | ? 完全忽略 | ? 添加到 specifiedValues |
| 优先级 | 无 | ? HTML 属性 < CSS 规则 |
| src 识别 | ? 不可用 | ? 正确识别 |
| CSS 覆盖 | 无 | ? CSS 可覆盖 HTML 属性 |

---

## 工作流程

### 现在的工作流程（修复后）

```
<img src="photo.jpg" width="200" height="150" class="myimg" />
  ↓
EzUIDocParser 解析
  ↓
EzUIDocNode
  ├─ name: "img"
  ├─ attributes: {
  │    "src": "photo.jpg",
  │    "width": "200",
  │    "height": "150",
  │    "class": "myimg"
  │  }
  └─ ...
  ↓
EzUiStyleTreeBuilder::Build()
  ├─ 步骤 1: 复制所有 HTML 属性到 specifiedValues
  │  specifiedValues = {
  │    "src": "photo.jpg",        ? src 现在在这里
  │    "width": "200",
  │    "height": "150",
  │    "class": "myimg"
  │  }
  │
  ├─ 步骤 2: 应用 CSS 规则（覆盖）
  │  CSS 中: img { width: 300; }
  │  specifiedValues = {
  │    "src": "photo.jpg",        ? 保留
  │    "width": "300",            ? 被 CSS 覆盖
  │    "height": "150",
  │    "class": "myimg"
  │  }
  │
  └─ 返回 EzUiStyledNode
     ↓
EzUiLayoutBuilder 构建布局框
  ↓
EngineLayout_DrawLayoutNode() 渲染
  ├─ GetImageSource()
  │  └─ 在 specifiedValues 中查找 "src" ? 找到！
  │     └─ 返回 "photo.jpg"
  │
  └─ 加载并显示图片 ?
```

---

## 优先级说明

修复后的优先级如下（**高 → 低**）：

```
1. CSS 规则（最高）       - 来自 CSS 文件
   ↓
2. HTML 属性（中等）      - 来自元素上的属性
   ↓
3. 默认值（最低）         - 来自框架默认设置
```

### 示例

```xml
<!-- HTML -->
<img src="original.jpg" width="200" />
```

```css
/* CSS */
img {
  width: 300;      /* 这会覆盖 HTML 属性 */
}

img.special {
  src: "override.jpg";  /* 这会覆盖 HTML src */
}
```

结果：
```
width: 300         ? 使用 CSS 值（覆盖 HTML）
src: "original.jpg" ? 使用 HTML 值（CSS 没有指定）
```

---

## 调试支持

已添加详细的调试输出到 `GetImageSource()` 方法：

```cpp
[GetImageSource] Found src: photo.jpg
[GetImageSource] src attribute not found in specifiedValues
[GetImageSource] Available specifiedValues:
  - width: 200
  - height: 150
  - class: myimg
```

这样可以快速识别 src 属性是否被正确识别。

---

## 测试用例

### 测试 1：基本 img 标签

```xml
<img src="test.png" width="100" height="100" />
```

**预期**：图片正确加载和显示  
**验证**：调试输出显示 `Found src: test.png`

### 测试 2：带 CSS 覆盖的 img 标签

```xml
<!-- HTML -->
<img src="original.jpg" class="large" />

<!-- CSS -->
<style>
  img.large {
    width: 400;
    height: 400;
  }
</style>
```

**预期**：图片加载，尺寸应用 CSS 值  
**验证**：调试输出显示 `Found src: original.jpg` 和 CSS 属性

### 测试 3：多个属性

```xml
<img src="multi.jpg" width="200" height="150" alt="Test" id="myimg" />
```

**预期**：所有属性都可以在 specifiedValues 中找到  
**验证**：调试输出列出所有属性

---

## 编译验证

```
? EzUiStyleTreeBuilder.cpp - 编译成功
? EzUiLayoutBox.cpp - 编译成功
? EngineLayoutRender.cpp - 编译成功

错误：0
```

---

## 总结

| 问题 | 原因 | 解决 |
|------|------|------|
| src 属性无法识别 | HTML 属性没有添加到 specifiedValues | 在 Build() 中复制所有 HTML 属性 |
| 图片无法显示 | GetImageSource() 找不到 src | 现在可以找到了 ? |
| 优先级不清楚 | 没有定义优先级规则 | HTML 属性 < CSS 规则 |

---

## 相关文件

- **修改**：`EzUiStyleTreeBuilder.cpp` - Build() 方法
- **改进**：`EzUiLayoutBox.cpp` - GetImageSource() 调试输出
- **参考**：`EngineLayoutRender.cpp` - img 标签渲染逻辑

---

**版本**: 1.0  
**日期**: 2024  
**状态**: ? 完成  
**编译**: ? 成功  

现在 `<img src="...">` 标签可以正确识别和显示图片了！????
