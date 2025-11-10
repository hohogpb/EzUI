# ?? BugFix: Gdiplus::Image LockBits 问题 - 快速解决方案

## ? 问题

`Gdiplus::Image` 类**没有** `LockBits()` 方法！

```cpp
Gdiplus::Image image(path);
image.LockBits(...);  // ? 编译错误：no member named 'LockBits'
```

## ? 解决方案

使用 **`Gdiplus::Bitmap`** 代替：

```cpp
// ? 正确做法
Gdiplus::Image image(path);
UINT width = image.GetWidth();
UINT height = image.GetHeight();

// 转换为 Bitmap
Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
Gdiplus::Graphics graphics(&bitmap);
graphics.DrawImage(&image, 0, 0, width, height);

// 现在可以调用 LockBits
Gdiplus::BitmapData bitmapData;
Gdiplus::Rect rect(0, 0, width, height);
bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

// ... 使用 bitmapData.Scan0 访问像素 ...

bitmap.UnlockBits(&bitmapData);
```

## ?? 改动的三个地方

### 1?? DrawImage 函数
```cpp
static void DrawImage(ID2D1HwndRenderTarget* rt, EzUiLayoutBox* node, const std::wstring& imagePath) {
  Gdiplus::Image image(imagePath.c_str());
  
  // ? 转换为 Bitmap
  UINT width = image.GetWidth();
  UINT height = image.GetHeight();
  Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
  Gdiplus::Graphics graphics(&bitmap);
  graphics.DrawImage(&image, 0, 0, width, height);
  
  // ? 使用 Bitmap.LockBits()
  // ...
}
```

### 2?? EngineLayout_DrawLayoutNode (img 标签)
```cpp
if (tag == L"img") {
  Gdiplus::Image image(imageSrc.c_str());
  
  // ? 转换为 Bitmap
  Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
  Gdiplus::Graphics graphics(&bitmap);
  graphics.DrawImage(&image, 0, 0, width, height);
  
  // ? 现在可以调用 LockBits
  Gdiplus::BitmapData bitmapData;
  bitmap.LockBits(&gdiRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
  // ...
  bitmap.UnlockBits(&bitmapData);
}
```

### 3?? GetBackgroundBitmap 方法
```cpp
ID2D1Bitmap* EzUiLayoutBox::GetBackgroundBitmap(ID2D1RenderTarget* renderTarget) {
  Gdiplus::Image image = styleNode->bgImage;
  
  // ? 转换为 Bitmap
  Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppARGB);
  Gdiplus::Graphics graphics(&bitmap);
  graphics.DrawImage(&image, 0, 0, width, height);
  
  // ? 使用 LockBits
  bitmap.LockBits(...);
  // ...
  bitmap.UnlockBits(...);
}
```

## ?? 关键点

| 类 | 有 LockBits? | 说明 |
|---|---|---|
| `Gdiplus::Image` | ? 否 | 通用图片对象 |
| `Gdiplus::Bitmap` | ? 是 | Image 的子类，有像素级操作 |

## ? 编译状态

```
? EzUiLayoutBox.cpp - 编译成功
? EngineLayoutRender.cpp - 编译成功
错误：0
```

---

**详细说明**: 查看 `BUGFIX_Gdiplus_Image_LockBits.md`
