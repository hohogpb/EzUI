#include "pch.h"
#include "EzUIWindow.h"
#include <WindowsX.h>
#include "EzUIAppWindow.h"

EzUIWindow::EzUIWindow(HINSTANCE hInst) {
  mInst = hInst;
  mWnd = NULL;

  WNDCLASS wc;
  if (!::GetClassInfo(mInst, EZUI_WND_CLASS, &wc)) {
    ZeroMemory(&wc, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)EzUIWindow::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    // wc.cbWndExtra = sizeof(LONG_PTR); // 为 GWLP_USERDATA 预留空间
    wc.hInstance = mInst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = EZUI_WND_CLASS;

    ::RegisterClass(&wc);
  }
}

EzUIWindow::~EzUIWindow() {
}

LRESULT CALLBACK EzUIWindow::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  EzUIWindow* pThis = NULL;
  if (::IsWindow(hWnd)) {
    pThis = (EzUIWindow*)GetProp(hWnd, TEXT("ezui_window_pointer"));
    // pThis = (EzUIWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (pThis && pThis->mWnd != hWnd) {
      pThis = NULL;
    }
  }

  if (uMessage == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
    pThis = (EzUIWindow*)(lpcs->lpCreateParams);
    SetProp(hWnd, TEXT("ezui_window_pointer"), (HANDLE)pThis);
    // SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
    pThis->mWnd = hWnd;
    pThis->OnCreate();
    return DefWindowProc(hWnd, uMessage, wParam, lParam);
  }

  if (!pThis)
    return DefWindowProc(hWnd, uMessage, wParam, lParam);

  switch (uMessage) {
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    pThis->OnPaint(hdc);

    EndPaint(hWnd, &ps);
  }
  return 0;
  case WM_SIZE:
    pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_DESTROY:
    RemoveProp(hWnd, TEXT("ezui_window_pointer"));
    // SetWindowLongPtr(hWnd, GWLP_USERDATA, 0); // 在销毁前清除指针
    pThis->OnDestroy();
    delete pThis; // 这里注意释放了
    return 0;
  case WM_VSCROLL:
    pThis->OnVScroll(HIWORD(wParam), LOWORD(wParam));
    return 0;
  case WM_HSCROLL:
    pThis->OnHScroll(HIWORD(wParam), LOWORD(wParam));
    return 0;
  case WM_MOUSEWHEEL:
    pThis->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
    return 0;
  case WM_KEYDOWN:
    pThis->OnKeyDown((UINT)wParam);
    return 0;
  case WM_MOUSEMOVE:
  {
    TRACKMOUSEEVENT tme;
    ZeroMemory(&tme, sizeof(TRACKMOUSEEVENT));
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_QUERY;
    tme.hwndTrack = hWnd;
    TrackMouseEvent(&tme);
    if (!(tme.dwFlags & TME_LEAVE)) {
      tme.dwFlags = TME_LEAVE;
      tme.hwndTrack = hWnd;
      TrackMouseEvent(&tme);
    }
    pThis->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
  }
  return 0;
  case WM_MOUSELEAVE:
    pThis->OnMouseLeave();
    return 0;
  case WM_LBUTTONDOWN:
    pThis->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
  case WM_LBUTTONUP:
    pThis->OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
  case WM_CHAR:
    pThis->OnChar((TCHAR)wParam);
    return 0;
  case WM_TIMER:
    pThis->OnTimer((UINT)wParam);
    return 0;
  }

  return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void EzUIWindow::OnCreate() {
  Created.emit(this);
}

void EzUIWindow::OnDestroy() {
  Destoryed.emit(this);
}

void EzUIWindow::OnPaint(HDC hdc) {
  // TextOut(hdc, 10, 10, L"Hello, EzUIWindow!", 18);
  Draw.emit(this, hdc);
}

void EzUIWindow::OnSize(int width, int height) {
  Resized.emit(this, width, height);
}

void EzUIWindow::OnVScroll(int pos, int flags) {
  VScrolled.emit(this, pos, flags);
}

void EzUIWindow::OnHScroll(int pos, int flags) {
  HScrolled.emit(this, pos, flags);
}

void EzUIWindow::OnMouseWheel(int delta) {
  MouseWheeled.emit(this, delta);
}

void EzUIWindow::OnKeyDown(UINT vKey) {
  KeyDown.emit(this, vKey);
}

void EzUIWindow::OnMouseMove(int x, int y) {
  MouseMoved.emit(this, x, y);
}

void EzUIWindow::OnLButtonDown(int x, int y) {
  LButtonDown.emit(this, x, y);
}

void EzUIWindow::OnLButtonUp(int x, int y) {
  LButtonUp.emit(this, x, y);
}

void EzUIWindow::OnMouseLeave() {
  MouseLeaved.emit(this, 0, 0);
}

void EzUIWindow::OnChar(UINT vChar) {
  CharInputed.emit(this, vChar);
}

void EzUIWindow::OnTimer(UINT timerId) {
  TimerCalled.emit(this, timerId);
}

void EzUIWindow::Create(int x, int y, int width, int height, HWND parent) {
  mWnd = CreateWindow(EZUI_WND_CLASS, L"ezui_window", WS_CHILD | WS_VISIBLE,
    x, y, width, height, parent, NULL, mInst, (LPVOID)this);
}

void EzUIWindow::Invalidate(bool erase) {
  if (mWnd) {
    InvalidateRect(mWnd, nullptr, erase ? TRUE : FALSE);
  }
}

RECT EzUIWindow::GetClientRect() {
  if (mWnd) {
    RECT rect;
    ::GetClientRect(mWnd, &rect);
    return rect;
  }
  return RECT();
}
