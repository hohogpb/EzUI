#pragma once

#include <common/signal.h>

#define EZUI_APPWND_CLASS	L"EZUI_APPWND_CLASS"

class EzUIAppWindow {
public:
  EzUIAppWindow(HINSTANCE hInst);
  ~EzUIAppWindow();

  void Create(int x = CW_USEDEFAULT, int y = 0, int width = CW_USEDEFAULT, int height = 0);
  HWND GetHwnd() const { return mWnd; }
  RECT GetClientRect();
  void Invalidate(bool erase = true);
  LONG GetLong(int idx);

  Signal<EzUIAppWindow*, UINT> Activated;
  Signal<EzUIAppWindow*> Created;
  Signal<EzUIAppWindow*> Destoryed;
  Signal<EzUIAppWindow*, HDC> Draw;
  Signal<EzUIAppWindow*, int, int> Resized;
  Signal<EzUIAppWindow*, short> MouseWheel;
  Signal<EzUIAppWindow*, int, int> MouseMove;
  Signal<EzUIAppWindow*> MouseLeave;
  Signal<EzUIAppWindow*, UINT> KeyDown;
  Signal<EzUIAppWindow*, int, int> LButtonDown;
  Signal<EzUIAppWindow*, int, int> LButtonUp;
  Signal<EzUIAppWindow*, UINT> Input;
  Signal<EzUIAppWindow*, UINT> Tick;

  bool borderless = true; // is the window currently borderless
  bool borderless_resize = true; // should the window allow resizing by dragging the borders while borderless
  bool borderless_drag = true; // should the window allow moving my dragging the client area
  bool borderless_shadow = true; // should the window display a native aero shadow while borderless

protected:
  virtual LRESULT OnMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

  void SetBorderless(bool enabled);
  void SetBorderlessShadow(bool enabled);

private:
  void CheckkRegisterClass();

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

protected:
  HWND mWnd;
  HINSTANCE mInst;
};

