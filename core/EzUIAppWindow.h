#pragma once

#include <common/signal.h>

#define EZUI_APPWND_CLASS	L"EZUI_APPWND_CLASS"

class EzUIAppWindow {
public:
  // we cannot just use WS_POPUP style
  // WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
  // WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the task bar item)
  // WS_CAPTION: enables aero minimize animation/transition
  // WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable minimize/maximize
  enum class Style : DWORD {
    windowed = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    aero_borderless = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    basic_borderless = WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
  };

  EzUIAppWindow(HINSTANCE hInst);
  ~EzUIAppWindow();

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

protected:
  virtual void OnCreate();
  virtual void OnDestroy();
  virtual void OnSize(int width, int height);
  virtual void OnActivate(UINT status);
  virtual void OnPaint(HDC hdc);

  // for borderless window hit test
  LRESULT HitTest(POINT cursor);

  void SetBorderless(bool enabled);
  void SetBorderlessShadow(bool enabled);

public:
  void Create(int x = CW_USEDEFAULT, int y = 0, int width = CW_USEDEFAULT, int height = 0);

  HWND GetHwnd() const { return mWnd; }
  RECT GetClientRect();
  LONG GetLong(int idx);

  Signal<EzUIAppWindow*> Created;
  Signal<EzUIAppWindow*> Resized;
  Signal<EzUIAppWindow*, UINT> Activated;
  Signal<EzUIAppWindow*, HDC> Draw;

protected:
  HWND mWnd;
  HINSTANCE mInst;

  bool borderless = true; // is the window currently borderless
  bool borderless_resize = true; // should the window allow resizing by dragging the borders while borderless
  bool borderless_drag = true; // should the window allow moving my dragging the client area
  bool borderless_shadow = true; // should the window display a native aero shadow while borderless

};

