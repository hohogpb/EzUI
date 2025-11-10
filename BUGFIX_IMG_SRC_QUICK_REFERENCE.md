# ? 快速修复：img src 属性识别问题

## 问题

```xml
<img src="image.png" width="200" height="150" />
<!-- ? src 属性无法识别，图片无法加载 -->
```

## 根本原因

`EzUIDocNode` 正确解析了 `src` 属性，但 `EzUiStyleTreeBuilder::Build()` **没有将 HTML 属性复制到 `specifiedValues`** 中。

## 解决方案

在 `EzUiStyleTreeBuilder::Build()` 中添加代码来复制 HTML 属性：

```cpp
// 修改前：只有 CSS 规则
styled->specifiedValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Normal);

// 修改后：HTML 属性 + CSS 规则
// 1. 首先添加 HTML 属性
for (const auto& [attrName, attrValue] : docNode->attributes) {
  styled->specifiedValues[attrName] = attrValue;
}

// 2. 然后添加 CSS 规则（会覆盖同名的 HTML 属性）
PropertyMap cssValues = SpecifiedValues(docNode, stylesheet, PseudoClassState::Normal);
for (const auto& [propName, propValue] : cssValues) {
  styled->specifiedValues[propName] = propValue;
}
```

## 文件修改

### 1. EzUiStyleTreeBuilder.cpp

```cpp
std::unique_ptr<EzUiStyledNode> EzUiStyleTreeBuilder::Build(EzUIDocNode* docNode, Stylesheet* stylesheet) {
  auto styled = std::make_unique<EzUiStyledNode>();
  styled->node = docNode;

  if (docNode) {
    // ? 首先添加 HTML 属性
    for (const auto& [attrName, attrValue] : docNode->attributes) {
      styled->specifiedValues[attrName] = attrValue;
    }

    // ? 然后添加 CSS 规则（覆盖同名属性）
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

## 工作原理

```
HTML: <img src="photo.jpg" width="200" />
  ↓
Parse: EzUIDocNode.attributes = {src: "photo.jpg", width: "200"}
  ↓
Build: specifiedValues 继承所有 HTML 属性 ?
  ↓
GetImageSource(): 在 specifiedValues 中找到 src ?
  ↓
Render: 加载并显示图片 ?
```

## 优先级

```
CSS 规则（高）
  ↓
HTML 属性（中）
  ↓
默认值（低）
```

CSS 规则可以覆盖 HTML 属性：

```xml
<!-- HTML -->
<img src="original.jpg" width="200" />

<!-- CSS 覆盖 width -->
<style>
  img { width: 400; }
</style>

<!-- 结果 -->
src: "original.jpg"  ← HTML 值（CSS 没覆盖）
width: 400           ← CSS 值（覆盖 HTML）
```

## 调试输出

```cpp
[GetImageSource] Found src: photo.jpg ?
[GetImageSource] Available specifiedValues:
  - src: photo.jpg
  - width: 200
  - height: 150
```

## 测试

```xml
<!-- ? 现在可以工作 -->
<img src="test.png" width="100" height="100" />
<img src="assets/photo.jpg" />
<img src="../../images/banner.png" class="responsive" />
```

---

**完整文档**: 查看 `BUGFIX_IMG_SRC_Attribute.md`  
**编译**: ? 成功  
**状态**: ? 完成
