# EzUiLayoutBox Getter 方法 Hover 支持快速参考

## ? 新增 Hover 支持的 Getter 方法

### 1?? GetOpacity() - 透明度

```cpp
float GetOpacity() const;
```

**作用**: 获取当前透明度（考虑 hover 状态）

**范围**: 0.0 - 1.0
- `0.0` = 完全透明
- `1.0` = 完全不透明

**CSS 示例**:
```css
button {
  opacity: 0.8;
}

button:hover {
  opacity: 1;  /* ? hover 时完全不透明 */
}
```

---

### 2?? GetBackgroundColor() - 背景色

```cpp
std::optional<EzUI::Color> GetBackgroundColor() const;
```

**作用**: 获取当前背景颜色（考虑 hover 状态）

**返回**: 可选的 Color 对象

**CSS 示例**:
```css
.card {
  background-color: white;
}

.card:hover {
  background-color: #f5f5f5;  /* ? hover 时变灰 */
}
```

---

### 3?? GetBorderColor() - 边框色

```cpp
std::optional<EzUI::Color> GetBorderColor() const;
```

**作用**: 获取当前边框颜色（考虑 hover 状态）

**返回**: 可选的 Color 对象

**CSS 示例**:
```css
.element {
  border-color: #ccc;
}

.element:hover {
  border-color: #333;  /* ? hover 时变深 */
}
```

---

### 4?? GetBorder() - 边框宽度

```cpp
EzUI::EdgeSizes GetBorder() const;
```

**作用**: 获取当前边框宽度（考虑 hover 状态）

**返回**: EdgeSizes 结构体（所有边宽度相同）

**CSS 示例**:
```css
.box {
  border-width: 1;
}

.box:hover {
  border-width: 2;  /* ? hover 时边框加粗 */
}
```

---

## ?? 完整示例

### CSS

```css
.interactive-box {
  background-color: white;
  opacity: 0.8;
  border-color: #ddd;
  border-width: 1;
}

.interactive-box:hover {
  background-color: #f0f0f0;
  opacity: 1;
  border-color: #333;
  border-width: 2;
}
```

### 效果

| 属性 | 正常状态 | Hover 状态 |
|------|---------|-----------|
| 背景色 | white | #f0f0f0 |
| 透明度 | 0.8 | 1 |
| 边框色 | #ddd | #333 |
| 边框宽 | 1 | 2 |

---

## ?? 工作原理

所有方法都遵循这个模式：

```
1. 检查 mIsHover 标志
   ↓
2. 是 → 从 hoverValues 查找属性
   ↓
3. 找到 → 解析并返回 hover 值
   ↓
4. 未找到 → 返回正常状态值
```

---

## ?? 使用场景

### 场景 1：按钮悬停效果

```css
button {
  background-color: #007AFF;
  opacity: 0.95;
}

button:hover {
  background-color: #0051D5;
  opacity: 1;
}
```

### 场景 2：卡片悬停效果

```css
.card {
  background-color: white;
  border-color: #e0e0e0;
  border-width: 1;
  opacity: 0.9;
}

.card:hover {
  background-color: #fafafa;
  border-color: #999;
  border-width: 2;
  opacity: 1;
}
```

### 场景 3：链接悬停效果

```css
.link {
  opacity: 0.7;
}

.link:hover {
  opacity: 1;
}
```

---

## ?? 注意事项

1. **透明度范围**: 值必须在 0.0 到 1.0 之间
2. **颜色格式**: 支持 #RRGGBB、rgb() 和颜色名
3. **数值解析**: 会自动处理字符串到数值的转换
4. **边框宽度**: 四条边的宽度相同

---

## ?? 支持的 CSS 属性

在 `:hover` 伪类中支持以下属性：

| 属性 | 方法 | 类型 |
|------|------|------|
| `background-color` | GetBackgroundColor() | Color |
| `opacity` | GetOpacity() | float |
| `border-color` | GetBorderColor() | Color |
| `border-width` | GetBorder() | EdgeSizes |

---

## ? 编译状态

? **编译成功** - 0 错误，0 警告

---

## ?? 下一步

- 使用这些方法创建交互式 UI
- 在 CSS 中定义 `:hover` 伪类样式
- 享受流畅的悬停效果！

---

**版本**: 1.0  
**状态**: ? 就绪  
**最后更新**: 2024
