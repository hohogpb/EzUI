#pragma once

#include <gdiplus.h>

class GdiPlusScope {
public:
  explicit GdiPlusScope() {
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&token_, &input, nullptr);
  }

  ~GdiPlusScope() {
    Gdiplus::GdiplusShutdown(token_);
  }

  GdiPlusScope(const GdiPlusScope&) = delete;
  GdiPlusScope& operator=(const GdiPlusScope&) = delete;

private:
  ULONG_PTR token_{};
};