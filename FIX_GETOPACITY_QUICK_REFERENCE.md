# GetOpacity() 修复快速参考

## ?? 问题

```cpp
// ? 之前：CSS 中的 opacity 值被忽略
float EzUiLayoutBox::GetOpacity() const {
  if (!styleNode) return 1.0f;
  
  if (mIsHover && !styleNode->hoverValues.empty()) {
    auto opacityIt = styleNode->hoverValues.find(L"opacity");
    if (opacityIt != styleNode->hoverValues.end()) {
      try {
        float hoverOpacity = std::stof(opacityIt->second);
        return (std::max)(0.0f, (std::min)(1.0f, hoverOpacity));
      } catch (...) {}
    }
  }
  
  return styleNode->opacity;  // ? 直接返回，没查询 specifiedValues
}
```

**问题**：CSS 中的 `opacity: 0.8;` 被完全忽略

---

## ?? 修复

```cpp
// ? 之后：正确查询 CSS 中的 opacity 值
float EzUiLayoutBox::GetOpacity() const {
  if (!styleNode) return 1.0f;
  
  // Level 1: Hover 状态
  if (mIsHover && !styleNode->hoverValues.empty()) {
    auto opacityIt = styleNode->hoverValues.find(L"opacity");
    if (opacityIt != styleNode->hoverValues.end()) {
      try {
        float hoverOpacity = std::stof(opacityIt->second);
        return (std::max)(0.0f, (std::min)(1.0f, hoverOpacity));
      } catch (...) {}
    }
  }
  
  // Level 2: 后代 Hover（优雅设计）
  if (hasHoverDescendants && IsAnyDirectChildHovering()) {
    auto opacityIt = styleNode->descendantHoverAppliedValues.find(L"opacity");
    if (opacityIt != styleNode->descendantHoverAppliedValues.end()) {
      try {
        float opacity = std::stof(opacityIt->second);
        return (std::max)(0.0f, (std::min)(1.0f, opacity));
      } catch (...) {}
    }
  }
  
  // Level 3: CSS 中定义的值 ? 修复关键
  auto opacityIt = styleNode->specifiedValues.find(L"opacity");
  if (opacityIt != styleNode->specifiedValues.end()) {
    try {
      float opacity = std::stof(opacityIt->second);
      return (std::max)(0.0f, (std::min)(1.0f, opacity));
    } catch (...) {}
  }
  
  // Level 4: 兜底
  return styleNode->opacity;
}
```

**改进**：现在正确查询 CSS 中的 opacity 值

---

## ?? 对比

| CSS 代码 | 之前 | 之后 |
|---------|------|------|
| 无 opacity | 1.0 | 1.0 ? |
| `opacity: 0.5;` | 1.0 ? | 0.5 ? |
| `opacity: 0.8;` | 1.0 ? | 0.8 ? |
| `:hover { opacity: 1; }` | 1.0 ? | 1.0 ? |

---

## ?? 优先级

```
自己 :hover
  ↓
有后代 :hover
  ↓
CSS 定义的值 ← ? 新增
  ↓
默认值 (1.0f)
```

---

## ? 编译状态

? 成功 - 0 错误，0 警告

---

## ?? 关键改动

**一句话总结**：在返回 `styleNode->opacity` 之前，先检查 `specifiedValues["opacity"]` 中是否有 CSS 定义的值。
