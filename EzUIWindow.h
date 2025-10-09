#pragma once

#define EZUI_WND_CLASS	L"EZUI_WND_CLASS"

class EzUIWindow {
public:
  EzUIWindow(HINSTANCE hInst);
  ~EzUIWindow();

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

protected:
  virtual void OnCreate();
  virtual void OnDestroy();
  virtual void OnPaint(HDC hdc);
  virtual void OnSize(int width, int height);
  virtual void OnVScroll(int pos, int flags);
  virtual void OnHScroll(int pos, int flags);
  virtual void OnMouseWheel(int delta);
  virtual void OnKeyDown(UINT vKey);
  virtual void OnMouseMove(int x, int y);
  virtual void OnLButtonDown(int x, int y);
  virtual void OnLButtonUp(int x, int y);
  virtual void OnMouseLeave();

public:
  void Create(int x = CW_USEDEFAULT, int y = 0, int width = CW_USEDEFAULT, int height = 0, HWND parent = nullptr);

  HWND GetHwnd() const { return mWnd; }
protected:
  HWND mWnd;
  HINSTANCE mInst;
};

