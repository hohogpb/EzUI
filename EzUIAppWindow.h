#pragma once

#define EZUI_APPWND_CLASS	L"EZUI_APPWND_CLASS"

class EzUIAppWindow {
public:
  EzUIAppWindow(HINSTANCE hInst);
  ~EzUIAppWindow();

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

protected:
  virtual void OnCreate();
  virtual void OnDestroy();
  virtual void OnSize(int width, int height);
  
public:
  void Create(int x = CW_USEDEFAULT, int y = 0, int width = CW_USEDEFAULT, int height = 0);

  HWND GetHwnd() const { return mWnd; }
protected:
  HWND mWnd;
  HINSTANCE mInst;
};

