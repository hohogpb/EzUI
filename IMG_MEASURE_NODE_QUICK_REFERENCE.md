# ? MeasureImgNode 快速参考

## 什么是 MeasureImgNode？

类似 `MeasureTextNode`，是一个 Yoga 测量函数，用于计算 img 标签的理想大小。

```cpp
// 两个测量函数的对比
YGNodeSetMeasureFunc(ygNode, MeasureTextNode);   // 用于文本
YGNodeSetMeasureFunc(ygNode, MeasureImgNode);    // 用于图片
```

## 核心功能

| 功能 | 说明 |
|------|------|
| **自动大小** | 使用图片原生大小 |
| **约束处理** | 响应 Yoga 的 MeasureMode |
| **比例保持** | 自动计算缺失的维度 |
| **布局集成** | 与 Yoga 系统完美配合 |

## 处理的场景

### 1?? 无约束（MeasureMode: Undefined）

```xml
<img src="photo.jpg" />
<!-- 原生 640x480 → 显示 640x480 -->
```

### 2?? 有约束（MeasureMode: AtMost）

```xml
<div style="width: 400;">
  <img src="photo.jpg" />
  <!-- 原生 640x480 → 显示 400x300 -->
</div>
```

### 3?? 严格约束（MeasureMode: Exactly）

```xml
<img src="photo.jpg" width="500" />
<!-- 原生 640x480 → 显示 500x375 -->
```

## 工作原理

```
img 标签被测量
  ↓
调用 MeasureImgNode()
  ├─ 获取原生大小（640x480）
  ├─ 检查 Yoga 约束
  │  ├─ widthMode: Exactly → width = 500
  │  └─ heightMode: Undefined
  ├─ 计算缺失维度
  │  └─ height = 480 * (500/640) = 375
  └─ 返回测量结果 (500, 375)
       ↓
Yoga 布局系统获得大小
  ↓
完成布局计算
```

## 代码对比

### ? 原方式

```cpp
if (!hasWidth) {
  YGNodeStyleSetWidth(ygNode, imgWidth);  // 直接设置
}
if (!hasHeight) {
  YGNodeStyleSetHeight(ygNode, imgHeight);
}
```

**问题**: 无法响应动态约束

### ? 新方式

```cpp
YGNodeSetMeasureFunc(ygNode, MeasureImgNode);  // 设置测量函数
```

**优势**: 完全动态，响应 Yoga 约束

## 支持的布局

### Flex 布局

```xml
<div style="display: flex;">
  <img src="photo1.jpg" style="flex-grow: 1;" />
  <img src="photo2.jpg" style="flex-grow: 1;" />
</div>
```

MeasureImgNode 自动计算每个 img 的大小

### 容器约束

```xml
<div style="max-width: 500; max-height: 400;">
  <img src="photo.jpg" />
</div>
```

MeasureImgNode 自动在约束内缩放

### 用户指定

```xml
<img src="photo.jpg" width="300" />
```

MeasureImgNode 保持宽高比计算高度

## 调试输出

```
[MeasureImgNode] Native: 640x480 | Measured: 500x375
[BuildNode] img tag with width only, height will be calculated
```

## 性能

| 方面 | 说明 |
|------|------|
| 调用频率 | 布局时调用（正常） |
| 计算量 | 最小（仅比例计算） |
| 缓存 | 图片大小已缓存 |
| 总开销 | 可忽略不计 |

## 与 MeasureTextNode 关系

都是 Yoga 的测量函数：

| 函数 | 元素 | 测量方式 |
|------|------|--------|
| MeasureTextNode | `<text>` | 文字渲染尺寸 |
| MeasureImgNode | `<img>` | 图片原生大小 |

## 现在支持的用法

```xml
<!-- ? 都能正常工作 -->
<img src="photo.jpg" />
<img src="photo.jpg" width="400" />
<img src="photo.jpg" height="300" />
<img src="photo.jpg" width="400" height="300" />

<div style="max-width: 500;">
  <img src="photo.jpg" />
</div>

<div style="display: flex;">
  <img src="photo.jpg" />
</div>
```

## 调用流程

```
Yoga 布局计算
  ↓
需要测量某个节点
  ↓
该节点设置了 MeasureFunc?
  ├─ 是 img → 调用 MeasureImgNode()
  ├─ 是 text → 调用 MeasureTextNode()
  └─ 否 → 使用静态大小
       ↓
测量函数返回大小
       ↓
继续布局计算
```

---

**完整文档**: `IMG_MEASURE_NODE_IMPLEMENTATION.md`
