# ? SimpleEzUI 背景图片使用完全指南

## ?? 问题诊断

你的代码：
```xml
<UI background-image="sample.jpg">
```

### 可能的问题

1. **语法问题** - 应该使用 `url()` 包装
2. **路径问题** - 相对路径可能不正确
3. **解析问题** - 属性可能没被正确识别

---

## ? 正确的用法

### 方式 1：标准 CSS url() 语法（推荐）

```xml
<!-- ? 正确 -->
<UI background-image="url('sample.jpg')" />

<!-- ? 也正确 -->
<UI background-image='url("sample.jpg")' />

<!-- ? 也支持 -->
<UI background-image="url(sample.jpg)" />
```

### 方式 2：在 style 属性中

```xml
<!-- ? 推荐：使用 style 属性 -->
<UI name="control-layer" 
    position="absolute" 
    left="0" top="0" 
    width="100%" height="100%"
    style="background-image: url('sample.jpg'); 
           background-color: #000000ff;"
    justify-content="space-between">
</UI>
```

### 方式 3：直接属性（如果支持）

```xml
<!-- ?? 可能不工作 - 依赖于解析器 -->
<UI background-image="sample.jpg" />

<!-- ? 改为 -->
<UI background-image="url('sample.jpg')" />
```

---

## ?? 路径问题诊断

### 检查清单

- [ ] 图片文件是否存在？
- [ ] 路径是否正确？
- [ ] 是否在项目根目录？
- [ ] 是否需要完整路径？

### 常见路径格式

```xml
<!-- 1. 相对于项目根目录 -->
<UI background-image="url('sample.jpg')" />

<!-- 2. 子目录 -->
<UI background-image="url('assets/sample.jpg')" />

<!-- 3. 完整路径 -->
<UI background-image="url('C:/Users/YourName/Projects/sample.jpg')" />

<!-- 4. 使用正斜杠 -->
<UI background-image="url('D:/Images/sample.jpg')" />
```

### 推荐的项目结构

```
Project/
├── SimpleEzUI.exe
├── sample.jpg           ← 放在这里
├── assets/
│   ├── bg.png
│   ├── icons/
│   │   └── icon.png
│   └── ...
├── SimpleEzUI.ezui      ← UI 文件
└── ...
```

---

## ?? 完整工作示例

### 你的代码改进版本

```xml
<!-- 根节点 -->
<Window name="window" padding="10" width="800" height="600">
  <Video name="video" position="absolute" left="0" top="0" width="100%" height="100%"></Video>

  <!-- ? 背景图片正确用法 -->
  <UI name="control-layer" 
      position="absolute" 
      left="0" top="0" 
      width="100%" height="100%"
      background-color="#000000ff" 
      justify-content="space-between"
      background-image="url('sample.jpg')">
   
    <!-- 左边栏 -->
    <TitleBar name="Titlebar" height="40" background-color="#FFFFFF22">
    </TitleBar>

    <div name="bottom" height="50" padding-top="3" padding-left="12" padding-right="12" margin-bottom="3">
      <!-- 其他内容 -->
    </div>
  </UI>
</Window>
```

### 使用 style 属性的版本

```xml
<UI name="control-layer" 
    position="absolute" 
    left="0" top="0" 
    width="100%" height="100%"
    justify-content="space-between"
    style="background-image: url('sample.jpg'); 
           background-color: #000000ff;">
  <!-- 内容 -->
</UI>
```

---

## ?? 高级用法

### 1. 背景图片 + 备用颜色

```xml
<UI style="
  background-image: url('sample.jpg');
  background-color: #333333;
" />
```

### 2. Hover 状态切换背景

```xml
<!-- HTML -->
<div name="card" class="interactive-card" 
     style="background-image: url('card-normal.jpg');" />

<!-- CSS -->
<style>
  .interactive-card {
    background-image: url('card-normal.jpg');
    background-color: #f5f5f5;
  }
  
  .interactive-card:hover {
    background-image: url('card-hover.jpg');
  }
</style>
```

### 3. 透明背景图片

```xml
<UI style="
  background-image: url('transparent-bg.png');
  background-color: rgba(0, 0, 0, 0.5);
  opacity: 0.8;
" />
```

### 4. 多个背景元素

```xml
<div style="
  position: relative;
  width: 100%;
  height: 100%;
  background-image: url('bg-pattern.png');
  background-color: #white;
">
  <!-- 前景内容 -->
</div>
```

---

## ?? 调试步骤

### 步骤 1：检查文件是否存在

```bash
# 在项目目录运行
dir sample.jpg
ls sample.jpg

# 如果没有找到，检查 assets 目录
dir assets/sample.jpg
```

### 步骤 2：检查语法

```xml
<!-- ? 正确的语法 -->
background-image="url('sample.jpg')"

<!-- ? 错误的语法 -->
background-image="sample.jpg"
background-image="url(sample.jpg"
background-image="url('sample.jpg"
```

### 步骤 3：检查路径

```xml
<!-- 检查相对路径 -->
<UI background-image="url('sample.jpg')" />          <!-- 项目根目录 -->
<UI background-image="url('assets/sample.jpg')" />   <!-- assets 子目录 -->
<UI background-image="url('../sample.jpg')" />       <!-- 上级目录 -->

<!-- 检查绝对路径 -->
<UI background-image="url('D:/Projects/sample.jpg')" />
```

### 步骤 4：启用调试输出

查看编译输出是否有相关错误信息：
- 图片加载失败信息
- 路径错误信息
- 解析错误信息

---

## ?? 完整可用的示例代码

### Example 1: 简单背景

```xml
<Window width="800" height="600">
  <UI width="100%" height="100%" 
      background-image="url('sample.jpg')"
      background-color="#000000ff">
    <!-- 内容 -->
  </UI>
</Window>
```

### Example 2: 带半透明的背景

```xml
<UI width="100%" height="100%"
    background-image="url('sample.jpg')"
    background-color="rgba(0, 0, 0, 0.3)"
    opacity="0.9">
  <!-- 内容 -->
</UI>
```

### Example 3: 你的完整代码修正版

```xml
<!-- 根节点 -->
<Window name="window" padding="10" width="800" height="600">
  <Video name="video" position="absolute" left="0" top="0" width="100%" height="100%"></Video>

  <!-- 控制层 - 带背景图片 -->
  <UI name="control-layer" 
      position="absolute" 
      left="0" top="0" 
      width="100%" height="100%"
      background-color="#000000ff" 
      background-image="url('sample.jpg')"
      justify-content="space-between">
   
    <!-- 标题栏 -->
    <TitleBar name="Titlebar" height="40" background-color="#FFFFFF22">
    </TitleBar>

    <!-- 底部控制 -->
    <div name="bottom" height="50" padding-top="3" padding-left="12" padding-right="12" margin-bottom="3">
      <!-- 进度条 -->
      <div name="progress-bar-container" height="3">
        <div name="progress-bar" height="100%" background="#ffffff55">
          <!-- ... 其他内容保持不变 ... -->
        </div>
      </div>

      <!-- 控制按钮 -->
      <div name="controls" height="100%" flex-direction="row">
        <!-- ... 按钮内容保持不变 ... -->
      </div>
    </div>
  </UI>
</Window>
```

---

## ? 常见问题

### Q1: 背景图片不显示？

**A:** 检查以下几点：

1. ? 是否使用了 `url()` 包装？
   ```xml
   <!-- ? 错误 -->
   background-image="sample.jpg"
   
   <!-- ? 正确 -->
   background-image="url('sample.jpg')"
   ```

2. ? 文件是否存在？
   ```bash
   # 检查文件
   ls sample.jpg
   ```

3. ? 路径是否正确？
   ```xml
   <!-- 相对路径 -->
   background-image="url('assets/sample.jpg')"
   
   <!-- 或绝对路径 -->
   background-image="url('C:/path/to/sample.jpg')"
   ```

4. ? 是否指定了大小？
   ```xml
   <UI width="400" height="300" 
       background-image="url('sample.jpg')" />
   ```

### Q2: 背景图片显示不清楚？

**A:** 尝试这些方法：

```xml
<!-- 方法 1：使用 opacity 调整 -->
<UI background-image="url('sample.jpg')" opacity="0.8" />

<!-- 方法 2：使用 background-color 混合 -->
<UI background-image="url('sample.jpg')"
    background-color="rgba(255, 255, 255, 0.5)" />
```

### Q3: 能否多个背景图片？

**A:** 暂不支持多个背景图片。使用单个图片或改用 CSS 渐变。

### Q4: 支持网络 URL 吗？

**A:** 不支持，只支持本地文件路径。

### Q5: 图片格式有限制吗？

**A:** 支持以下格式：
- ? PNG
- ? JPEG / JPG
- ? BMP
- ? GIF
- ? TIFF

---

## ?? 功能支持矩阵

| 功能 | 支持 | 说明 |
|------|------|------|
| `background-image` 属性 | ? | 可用 |
| `url()` 语法 | ? | 必需 |
| 相对路径 | ? | 推荐 |
| 绝对路径 | ? | 可用 |
| 透明度 | ? | 配合 opacity |
| Hover 状态 | ? | CSS 支持 |
| 多背景 | ? | 不支持 |
| 网络 URL | ? | 本地只 |
| `background-size` | ? | 待开发 |
| `background-position` | ? | 待开发 |
| `background-repeat` | ? | 待开发 |

---

## ?? 立即使用

### 快速修复你的代码

```xml
<!-- 原始代码 -->
<UI background-image="sample.jpg">

<!-- 修改为 -->
<UI background-image="url('sample.jpg')">

<!-- 或使用 style -->
<UI style="background-image: url('sample.jpg');">
```

### 验证步骤

1. 修改 XML 文件
2. 确保 `sample.jpg` 在项目目录
3. 重新编译
4. 运行程序
5. 检查是否显示背景图片

---

## ?? 参考文档

- IMAGE_SUPPORT_README.md - 完整功能说明
- IMAGE_SUPPORT_IMPLEMENTATION.md - 实现细节
- IMAGE_SUPPORT_QUICK_REFERENCE.md - 快速参考

---

## ? 总结

**关键点：**

1. ? 使用 `url()` 包装路径
2. ? 检查文件是否存在
3. ? 验证相对/绝对路径
4. ? 指定元素大小
5. ? 提供备用背景颜色

**推荐做法：**

```xml
<UI width="100%" height="100%"
    background-image="url('sample.jpg')"
    background-color="#333333">
</UI>
```

现在你的背景图片应该能正常显示了！??
