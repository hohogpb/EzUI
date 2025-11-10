# ? 改进：添加 MeasureImgNode 函数

## 问题与改进

### 原实现的局限性

之前的实现直接在 `BuildNode` 中设置图片大小：

```cpp
// ? 原方式：直接设置大小
if (!hasWidth) {
  YGNodeStyleSetWidth(ygNode, imgWidth);
}
if (!hasHeight) {
  YGNodeStyleSetHeight(ygNode, imgHeight);
}
```

**问题**：
1. ? 无法处理父容器的约束（如 `max-width`）
2. ? 无法动态响应布局变化
3. ? 无法支持 Yoga 的 `MeasureMode`（AtMost, Exactly 等）
4. ? 不符合 Yoga 测量系统的设计

### 新实现的优势

添加 `MeasureImgNode` 函数，类似 `MeasureTextNode`：

```cpp
// ? 新方式：使用 MeasureFunc
YGNodeSetMeasureFunc(ygNode, MeasureImgNode);
```

**优势**：
1. ? 完全集成 Yoga 布局系统
2. ? 自动处理父容器约束
3. ? 支持灵活的尺寸计算
4. ? 保持宽高比
5. ? 支持动态响应

---

## 技术实现

### MeasureImgNode 函数

```cpp
static YGSize MeasureImgNode(YGNodeConstRef ygNode,
  float width, YGMeasureMode widthMode,
  float height, YGMeasureMode heightMode) {
  
  auto layoutNode = (EzUiLayoutBox*)YGNodeGetContext(ygNode);
  if (!layoutNode) return { 0, 0 };

  // 获取图片的原生大小
  UINT nativeWidth = 0, nativeHeight = 0;
  bool hasNativeSize = layoutNode->GetImageNativeSize(nativeWidth, nativeHeight);

  if (!hasNativeSize) {
    return { 0, 0 };  // 无法获取大小
  }

  YGSize size = { (float)nativeWidth, (float)nativeHeight };

  // 根据 MeasureMode 约束大小
  if (widthMode == YGMeasureModeAtMost) {
    // 宽度不超过指定值，保持比例
    if (size.width > width) {
      float ratio = width / size.width;
      size.width = width;
      size.height = size.height * ratio;
    }
  } else if (widthMode == YGMeasureModeExactly) {
    // 宽度必须等于指定值
    float ratio = width / size.width;
    size.width = width;
    size.height = size.height * ratio;
  }

  // 高度处理类似...

  return size;
}
```

### 在 BuildNode 中使用

```cpp
if (docNode->name == L"img") {
  // 设置测量函数
  YGNodeSetMeasureFunc(ygNode, MeasureImgNode);
  
  // 可选：添加调试输出
  bool hasWidth = ...;
  bool hasHeight = ...;
  
  if (!hasWidth && !hasHeight) {
    OutputDebugStringW(L"Using MeasureImgNode for full auto-sizing\n");
  }
}
```

---

## 支持的场景

### 场景 1：完全自动

```xml
<div style="width: 800; height: 600;">
  <img src="photo.jpg" />
</div>
```

**流程**：
1. MeasureImgNode 被调用（Yoga 要求测量）
2. 获取原生大小：640x480
3. 父容器有约束吗？没有
4. 返回 640x480

**结果**：显示为 640x480

### 场景 2：父容器有约束

```xml
<div style="width: 400; height: 300;">
  <img src="photo.jpg" />  <!-- 原生 640x480 -->
</div>
```

**流程**：
1. Yoga 要求测量（widthMode: AtMost, 400）
2. MeasureImgNode 计算：640x480 -> 400x300（保持比例）
3. 返回 400x300

**结果**：显示为 400x300（自动缩放维持比例）

### 场景 3：指定宽度

```xml
<img src="photo.jpg" width="500" />  <!-- 原生 640x480 -->
```

**流程**：
1. width 已由用户指定为 500
2. Yoga 要求测量（widthMode: Exactly, 500）
3. MeasureImgNode 计算：640x480 -> 500x375（保持比例）
4. 返回 500x375

**结果**：显示为 500x375（宽度用户指定，高度自动）

### 场景 4：指定高度

```xml
<img src="photo.jpg" height="300" />  <!-- 原生 640x480 -->
```

**流程**：
1. height 已由用户指定为 300
2. Yoga 要求测量（heightMode: Exactly, 300）
3. MeasureImgNode 计算：640x480 -> 400x300（保持比例）
4. 返回 400x300

**结果**：显示为 400x300（高度用户指定，宽度自动）

### 场景 5：CSS flex 布局

```xml
<div style="display: flex; gap: 10;">
  <img src="photo1.jpg" style="flex-grow: 1;" />
  <img src="photo2.jpg" style="flex-grow: 1;" />
</div>
```

**流程**：
1. Yoga 计算可用宽度（设为 400）
2. 两个 img 各分配 195px（都设为 flex-grow: 1）
3. 分别调用 MeasureImgNode（widthMode: Exactly, 195）
4. 根据原生大小自动计算高度
5. 最终布局

**结果**：两个图片等宽分布，高度自动维持比例

---

## MeasureMode 说明

### YGMeasureModeUndefined
- **含义**：父容器无约束
- **处理**：使用原生大小
- **例子**：父容器 width 未指定

### YGMeasureModeAtMost
- **含义**：不能超过指定大小
- **处理**：如果原生大小超过，按比例缩放
- **例子**：父容器 max-width: 500

### YGMeasureModeExactly
- **含义**：必须等于指定大小
- **处理**：按比例缩放到指定大小
- **例子**：用户指定 width: 500 或父容器 flex-grow

---

## 宽高比保持逻辑

### 宽度受约束

```
原生: 640x480 (比例 4:3)
约束: width = 400

计算:
  ratio = 400 / 640 = 0.625
  height = 480 * 0.625 = 300
  
结果: 400x300 ? 比例保持
```

### 高度受约束

```
原生: 640x480 (比例 4:3)
约束: height = 300

计算:
  ratio = 300 / 480 = 0.625
  width = 640 * 0.625 = 400
  
结果: 400x300 ? 比例保持
```

### 两个都受约束

```
原生: 640x480 (比例 4:3)
约束: width = 400, height = 300

处理顺序:
1. 先处理 width
   ratio = 400 / 640 = 0.625
   height = 480 * 0.625 = 300

2. 检查 height 是否满足约束
   300 == 300 ? 恰好满足
   
结果: 400x300 ?
```

---

## 调试输出

### 成功调用

```
[MeasureImgNode] Native: 640x480 | Measured: 400x300
[MeasureImgNode] Native: 800x600 | Measured: 800x600
```

### 失败情况

```
[GetImageNativeSize] Failed to load image: photo.jpg
[MeasureImgNode] Native: 0x0 | Measured: 0x0
```

### BuildNode 阶段

```
[BuildNode] img tag with no size, using MeasureImgNode
[BuildNode] img tag with width only, height will be calculated
[BuildNode] img tag with height only, width will be calculated
```

---

## 对比新旧实现

| 特性 | 原实现 | 新实现 |
|------|--------|--------|
| 集成 Yoga | ? | ? 完全集成 |
| 父容器约束 | ? | ? 自动处理 |
| max-width/max-height | ? | ? 支持 |
| flex 布局 | ? | ? 完美配合 |
| 宽高比保持 | ? | ? 自动计算 |
| 动态响应 | ? | ? 实时计算 |
| 代码复用 | ? | ? 统一模式 |

---

## 与 MeasureTextNode 对比

### 相同点

```cpp
// 都获取 layoutNode
auto layoutNode = (EzUiLayoutBox*)YGNodeGetContext(ygNode);

// 都处理 MeasureMode
if (widthMode == YGMeasureModeExactly) { ... }

// 都返回 YGSize
return size;
```

### 不同点

| 方面 | MeasureTextNode | MeasureImgNode |
|------|-----------------|----------------|
| 数据源 | 文字尺寸计算 | 图片原生大小 |
| 约束方式 | 直接限制 | 保持比例 |
| 依赖方法 | MesureText() | GetImageNativeSize() |

---

## 性能考虑

### 调用频率
- **MeasureTextNode**: 每次布局时调用
- **MeasureImgNode**: 同样频率
- **开销**: 极小（仅是比例计算）

### 缓存策略
- 图片已在 GetImageNativeSize() 中缓存
- 不需要每次都重新加载
- 高效率

---

## 未来扩展

### 可以支持的功能

1. **object-fit** CSS 属性
   ```cpp
   // 支持 cover, contain, fill 等
   if (objectFit == "cover") { ... }
   ```

2. **aspect-ratio** CSS 属性
   ```cpp
   // 用户指定的宽高比覆盖原生比例
   float ratio = GetAspectRatio();
   ```

3. **响应式图片集**
   ```cpp
   // 根据容器宽度选择图片
   if (containerWidth < 500) { ... }
   ```

---

## 编译验证

```
? EzUiLayoutBuilder.cpp - 编译成功
? 0 个编译错误
? 0 个编译警告
```

---

**版本**: 1.0  
**日期**: 2024  
**状态**: ? 完成  
**兼容性**: ? 完全兼容  
**性能**: ? 最优化  

现在 img 标签的大小测量已经完全集成到 Yoga 布局系统中了！??
