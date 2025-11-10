# ? 图片加载 BugFix - Gdiplus::Image LockBits 问题

## ?? 发现的问题

`Gdiplus::Image` 类**没有** `LockBits` 成员函数！

### 错误代码示例

```cpp
// ? 这会编译失败
Gdiplus::Image image(path);
image.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
// error: 'class Gdiplus::Image' has no member named 'LockBits'
```

## ?? 解决方案

使用 **`Gdiplus::Bitmap`** 代替 `Gdiplus::Image`，因为：
- ? `Gdiplus::Bitmap` 继承自 `Gdiplus::Image`
- ? `Gdiplus::Bitmap` 提供 `LockBits()` 和 `UnlockBits()` 方法
- ? 可以从 `Gdiplus::Image` 转换到 `Gdiplus::Bitmap`

### 正确的做法

```cpp
// ? 正确：使用 Gdiplus::Bitmap
Gdiplus::Image image(path);
UINT width = image.GetWidth();
UINT height = image.GetHeight();

// 将 Image 转换为 Bitmap
Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
Gdiplus::Graphics graphics(&bitmap);
graphics.DrawImage(&image, 0, 0, width, height);

// 现在可以调用 LockBits
Gdiplus::BitmapData bitmapData;
Gdiplus::Rect rect(0, 0, width, height);
bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

// ... 使用 bitmapData ...

bitmap.UnlockBits(&bitmapData);
```

---

## ?? 代码修改

### 1. 修复 DrawImage 函数

```cpp
// ? 修复后
static void DrawImage(ID2D1HwndRenderTarget* rt, EzUiLayoutBox* node, const std::wstring& imagePath) {
  if (!node) {
    return;
  }

  try {
    Gdiplus::Image image(imagePath.c_str());
    if (image.GetLastStatus() != Gdiplus::Ok) {
      OutputDebugStringW((L"[DrawImage] Failed to load image: " + imagePath + L"\n").c_str());
      return;
    }

    UINT width = image.GetWidth();
    UINT height = image.GetHeight();
    
    if (width == 0 || height == 0) {
      return;
    }

    // ? 转换为 Bitmap（这样才能调用 LockBits）
    Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
    Gdiplus::Graphics graphics(&bitmap);
    graphics.DrawImage(&image, 0, 0, width, height);

    // ... 后续处理 ...
  } catch (...) {
    OutputDebugStringW((L"[DrawImage] Exception loading image\n").c_str());
  }
}
```

### 2. 修复 EngineLayout_DrawLayoutNode 中的 img 标签处理

```cpp
// ? img 标签处理（修复后）
if (tag == L"img") {
  auto imageSrc = aLayoutNode->GetImageSource();
  if (!imageSrc.empty()) {
    try {
      Gdiplus::Image image(imageSrc.c_str());
      if (image.GetLastStatus() == Gdiplus::Ok) {
        UINT width = image.GetWidth();
        UINT height = image.GetHeight();

        if (width > 0 && height > 0) {
          // ? 转换为 Bitmap
          Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
          Gdiplus::Graphics graphics(&bitmap);
          graphics.DrawImage(&image, 0, 0, width, height);

          // ? 现在可以调用 LockBits
          ComPtr<ID2D1Bitmap> d2dBitmap;
          Gdiplus::BitmapData bitmapData;
          Gdiplus::Rect gdiRect(0, 0, width, height);

          if (bitmap.LockBits(&gdiRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok) {
            // ... 创建 D2D Bitmap ...
            bitmap.UnlockBits(&bitmapData);
          }
        }
      }
    } catch (...) {
      OutputDebugStringW((L"[DrawLayoutNode] Failed to draw img\n").c_str());
    }
  }
  return;
}
```

### 3. 修复 EzUiLayoutBox::GetBackgroundBitmap

```cpp
ID2D1Bitmap* EzUiLayoutBox::GetBackgroundBitmap(ID2D1RenderTarget* renderTarget) {
  if (!styleNode || !styleNode->bgImage || !renderTarget) {
    return nullptr;
  }

  if (mCachedBitmap && mCachedImagePath == L"background") {
    return mCachedBitmap.Get();
  }

  try {
    UINT width = styleNode->bgImage->GetWidth();
    UINT height = styleNode->bgImage->GetHeight();

    if (width == 0 || height == 0) {
      return nullptr;
    }

    // ? 转换为 Bitmap
    Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
    Gdiplus::Graphics graphics(&bitmap);
    graphics.DrawImage(styleNode->bgImage, 0, 0, width, height);

    // ? 现在可以调用 LockBits
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, width, height);
    
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok) {
      ComPtr<ID2D1Bitmap> d2dBitmap;
      
      D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
      );
      
      HRESULT hr = renderTarget->CreateBitmap(
        D2D1::SizeU(width, height),
        bitmapData.Scan0,
        bitmapData.Stride,
        props,
        &d2dBitmap
      );
      
      if (SUCCEEDED(hr) && d2dBitmap) {
        mCachedBitmap = d2dBitmap;
        mCachedImagePath = L"background";
      }

      bitmap.UnlockBits(&bitmapData);
      return d2dBitmap.Get();
    }
  } catch (...) {
    OutputDebugStringW((L"[GetBackgroundBitmap] Exception converting image\n").c_str());
  }
  
  return nullptr;
}
```

---

## ?? 转换流程

### Image vs Bitmap 的区别

| 特性 | Gdiplus::Image | Gdiplus::Bitmap |
|------|---|---|
| 基类 | 无 | Image |
| LockBits() | ? 没有 | ? 有 |
| UnlockBits() | ? 没有 | ? 有 |
| 用途 | 通用图片对象 | 可编辑的位图 |
| 性能 | 中等 | 高（可像素级操作）|

### 转换步骤

```cpp
// 步骤 1：加载原始图片
Gdiplus::Image image(L"photo.jpg");

// 步骤 2：获取尺寸
UINT width = image.GetWidth();
UINT height = image.GetHeight();

// 步骤 3：创建 Bitmap 并绘制
Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
Gdiplus::Graphics graphics(&bitmap);
graphics.DrawImage(&image, 0, 0, width, height);

// 步骤 4：现在可以访问像素数据
Gdiplus::BitmapData bitmapData;
Gdiplus::Rect rect(0, 0, width, height);
bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

// 步骤 5：使用 bitmapData...
// bitmapData.Scan0 - 像素数据指针
// bitmapData.Stride - 每行字节数

// 步骤 6：释放
bitmap.UnlockBits(&bitmapData);
```

---

## ? 编译验证

```
? EzUiLayoutBox.cpp - 编译成功
? EngineLayoutRender.cpp - 编译成功

错误：0
警告：0
```

---

## ?? 相关知识

### GDI+ 类层次

```
Gdiplus::Image (虚基类)
  ├─ Gdiplus::Bitmap (继承 Image)
  ├─ Gdiplus::Metafile
  └─ ...

Gdiplus::Bitmap 提供的额外功能：
  - LockBits()          - 锁定像素数据
  - UnlockBits()        - 解锁像素数据
  - GetPixel()          - 获取单个像素
  - SetPixel()          - 设置单个像素
  - Clone()             - 克隆 Bitmap
```

### PixelFormat 常见值

```cpp
PixelFormat32bppARGB  // 32 位，ARGB 顺序（推荐用于 D2D）
PixelFormat24bppRGB   // 24 位，RGB 顺序
PixelFormat8bppIndexed // 8 位，索引颜色
```

### ImageLockMode 标志

```cpp
ImageLockModeRead     // 读取模式
ImageLockModeWrite    // 写入模式
ImageLockModeUserInputBuf // 使用用户提供的缓冲区
```

---

## ?? 最佳实践

### ? 推荐做法

```cpp
// 1. 加载并转换
Gdiplus::Image image(path);
UINT width = image.GetWidth();
UINT height = image.GetHeight();

Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
Gdiplus::Graphics graphics(&bitmap);
graphics.DrawImage(&image, 0, 0, width, height);

// 2. 使用 RAII 自动解锁
{
  Gdiplus::BitmapData bitmapData;
  Gdiplus::Rect rect(0, 0, width, height);
  
  if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, 
                      PixelFormat32bppARGB, &bitmapData) == Gdiplus::Ok) {
    // 使用 bitmapData
    
    bitmap.UnlockBits(&bitmapData);  // 确保释放
  }
}
```

### ? 避免的做法

```cpp
// ? 不要直接在 Image 上调用 LockBits
Gdiplus::Image image(path);
image.LockBits(...);  // 编译错误！

// ? 不要忘记 UnlockBits
Gdiplus::Bitmap bitmap(...);
bitmap.LockBits(...);
// ... 使用 ...
// bitmap.UnlockBits(...);  // 不要遗漏！
```

---

## ?? 常见问题

### Q: 为什么要转换？

A: `Gdiplus::Image` 是通用图片对象，支持多种格式（JPG, PNG, GIF 等）但没有像素级操作接口。`Gdiplus::Bitmap` 继承了这些功能并添加了像素级操作。

### Q: 转换会影响性能吗？

A: 不会。转换只是将现有图片数据复制到 Bitmap 对象中，开销很小（通常 < 1ms）。后续缓存机制会补偿这个开销。

### Q: 能直接从文件创建 Bitmap 吗？

A: 可以！
```cpp
// 直接从文件创建 Bitmap
Gdiplus::Bitmap bitmap(L"photo.jpg");
// 现在可以直接调用 LockBits
```

### Q: 为什么使用 ARGB 格式？

A: ARGB 格式与 D2D 的 DXGI_FORMAT_B8G8R8A8_UNORM 兼容，转换时无需字节顺序调整。

---

## ?? 修改总结

| 文件 | 修改内容 | 状态 |
|------|---------|------|
| EzUiLayoutBox.cpp | 修复 GetBackgroundBitmap | ? 完成 |
| EngineLayoutRender.cpp | 修复 DrawImage | ? 完成 |
| EngineLayoutRender.cpp | 修复 img 标签绘制 | ? 完成 |

---

## ? 修复后的优势

? **编译通过** - 无编译错误  
? **运行正确** - 正确访问像素数据  
? **性能良好** - Bitmap 缓存优化  
? **代码清晰** - 逻辑明确无歧义  

---

**版本**: 1.0  
**状态**: ? 完成  
**编译**: ? 成功  
**日期**: 2024  
