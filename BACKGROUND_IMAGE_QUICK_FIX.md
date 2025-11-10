# ?? 背景图片快速修复指南

## 你的问题

```xml
<!-- 当前代码 -->
<UI background-image="sample.jpg">
```

## 解决方案

### ? 方案 1：使用正确的语法（推荐）

```xml
<!-- 修改为 -->
<UI background-image="url('sample.jpg')">
```

### ? 方案 2：使用 style 属性

```xml
<!-- 或 -->
<UI style="background-image: url('sample.jpg');">
```

### ? 方案 3：完整示例

```xml
<UI name="control-layer" 
    position="absolute" 
    left="0" top="0" 
    width="100%" height="100%"
    background-color="#000000ff" 
    background-image="url('sample.jpg')"
    justify-content="space-between">
</UI>
```

---

## ? 快速检查清单

- [ ] 使用了 `url()` 包装？
- [ ] 文件 `sample.jpg` 存在？
- [ ] 路径正确？
- [ ] 元素有指定宽高？
- [ ] 重新编译？

---

## ?? 文件位置建议

```
Project/
├── sample.jpg           ← 放在这里（项目根目录）
├── SimpleEzUI.exe
├── SimpleEzUI.ezui      ← 你的 UI 文件
└── ...
```

或

```
Project/
├── assets/
│   └── sample.jpg       ← 或放在 assets 目录
├── SimpleEzUI.exe
├── SimpleEzUI.ezui
└── ...
```

对应的代码：
```xml
<!-- 项目根目录 -->
background-image="url('sample.jpg')"

<!-- assets 目录 -->
background-image="url('assets/sample.jpg')"
```

---

## 完整修正代码

你的原始代码：
```xml
<UI name="control-layer" position="absolute" left="0" top="0" width="100%" height="100%"
    background-color="#000000ff" justify-content="space-between"
    background-image="sample.jpg">
```

修正后：
```xml
<UI name="control-layer" position="absolute" left="0" top="0" width="100%" height="100%"
    background-color="#000000ff" justify-content="space-between"
    background-image="url('sample.jpg')">
```

---

## 验证步骤

1. **修改 XML**：添加 `url()` 包装
2. **检查文件**：确保 `sample.jpg` 存在
3. **编译**：重新编译项目
4. **运行**：检查背景是否显示

---

## 常见错误

```xml
<!-- ? 错误 1：缺少 url() -->
background-image="sample.jpg"

<!-- ? 修正 -->
background-image="url('sample.jpg')"

<!-- ? 错误 2：缺少引号 -->
background-image="url(sample.jpg)"

<!-- ? 修正 -->
background-image="url('sample.jpg')"

<!-- ? 错误 3：路径错误 -->
background-image="url('C:\temp\sample.jpg')"

<!-- ? 修正 -->
background-image="url('C:/temp/sample.jpg')"  <!-- 使用正斜杠 -->
```

---

## 现在就试试吧！

?? 只需一个小改动，你的背景图片就能工作！

```xml
background-image="sample.jpg"    → background-image="url('sample.jpg')"
```

详细文档：`BACKGROUND_IMAGE_USAGE_COMPLETE_GUIDE.md`
