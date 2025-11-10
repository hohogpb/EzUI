# CSS 伪类 :hover 快速参考

## ?? 快速开始

在你的 CSS 文件中使用 `:hover` 伪类：

```css
/* 定义正常状态 */
button {
  background-color: blue;
}

/* 定义 hover 状态 */
button:hover {
  background-color: red;
}
```

**就这么简单！** 当鼠标进入元素时，自动应用 `:hover` 样式。

---

## ?? 常见用法

### 改变背景色

```css
.box {
  background-color: white;
}

.box:hover {
  background-color: #f0f0f0;
}
```

### 改变文字颜色

```css
.link {
  color: blue;
}

.link:hover {
  color: red;
}
```

### 改变透明度

```css
.element {
  opacity: 0.8;
}

.element:hover {
  opacity: 1;
}
```

### 多个属性变化

```css
.card {
  background-color: white;
  opacity: 0.9;
}

.card:hover {
  background-color: #f5f5f5;
  opacity: 1;
}
```

---

## ?? 支持的属性

当前在 `:hover` 伪类中支持以下 CSS 属性：

- ? `background-color` - 背景颜色
- ? `color` - 文字颜色
- ? `opacity` - 透明度
- ? `border-color` - 边框颜色
- ?? 其他属性可根据需要添加

---

## ?? 实现原理

```
1. 鼠标进入元素
   ↓
2. 系统识别元素并标记为 hover 状态
   ↓
3. 应用 :hover 伪类的样式
   ↓
4. 重新渲染元素
   ↓
5. 用户看到新的样式效果
```

---

## ? 完整示例

```css
/* SimpleEzUI.css */

/* === 按钮样式 === */
button {
  background-color: #0066CC;
  color: white;
  padding: 10px 20px;
  opacity: 1;
}

button:hover {
  background-color: #0052A3;
  opacity: 0.9;
}

/* === 卡片样式 === */
.card {
  background-color: white;
  border: 1px solid #ccc;
}

.card:hover {
  background-color: #f0f0f0;
  border: 1px solid #333;
}

/* === 文本链接 === */
a {
  color: #0066CC;
}

a:hover {
  color: #FF6600;
}
```

---

## ?? 工作机制

### 加载时

CSS 解析器识别 `:hover` 伪类选择器：
```css
button:hover { background-color: red; }
```

样式树构建时，为每个 `button` 元素存储：
- **正常样式** - `background-color: blue`
- **hover 样式** - `background-color: red`

### 交互时

当鼠标进入元素：
1. 触发 `OnMouseEnter()` 回调
2. 标记元素状态为 `mIsHover = true`
3. 调用 `mainWindow->Invalidate()` 重新渲染
4. 绘制时使用 `GetCurrentBackgroundColor()` 获取 hover 颜色

当鼠标离开元素：
1. 触发 `OnMouseLeave()` 回调
2. 标记元素状态为 `mIsHover = false`
3. 调用 `mainWindow->Invalidate()` 重新渲染
4. 恢复正常样式

---

## ?? 示例代码

### HTML 示例（SimpleEzUI.ezui）

```xml
<button id="myButton" class="primary">Click Me</button>
```

### CSS 样式（SimpleEzUI.css）

```css
button {
  background-color: #007AFF;
  color: white;
}

button.primary:hover {
  background-color: #0051D5;
}

button#myButton:hover {
  background-color: #003AA3;
}
```

---

## ?? 特殊情况

### 带 ID 的元素

```css
#submit-btn:hover {
  background-color: green;
}
```

### 带 Class 的元素

```css
.btn-primary:hover {
  background-color: blue;
}
```

### 标签选择器

```css
div:hover {
  opacity: 0.9;
}
```

---

## ?? 注意事项

1. **嵌套元素**：子元素的 `:hover` 样式独立应用
   ```css
   .parent:hover { background: blue; }
   .child:hover { background: red; }
   ```

2. **性能**：`:hover` 样式在状态改变时才应用，不影响性能

3. **优先级**：`:hover` 样式覆盖正常状态，但遵循 CSS 优先级规则

---

## ?? 相关功能

- `:focus` - 元素获得焦点时的样式（已实现框架）
- `opacity` - 透明度属性
- 动画和过渡 - 配合 `:hover` 创建平滑效果

---

## ? 常见问题

**Q: :hover 样式不生效？**
A: 检查：
1. CSS 语法是否正确
2. 选择器是否匹配元素
3. 属性名是否支持（目前支持 background-color, color, opacity 等）

**Q: 可以用 :hover 改变大小吗？**
A: 目前主要支持颜色和透明度，其他属性可通过扩展添加

**Q: 子元素的 :hover 会影响父元素吗？**
A: 不会，每个元素的 :hover 样式独立应用

---

## ?? 获取帮助

参考完整文档：
- [PSEUDO_CLASS_HOVER_IMPLEMENTATION.md](PSEUDO_CLASS_HOVER_IMPLEMENTATION.md)

---

**版本**: 1.0  
**状态**: ? 生产就绪  
**最后更新**: 2024
