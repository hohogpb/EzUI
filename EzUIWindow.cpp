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
    wc.hCursor = NULL/*LoadCursor(NULL, IDC_ARROW)*/;
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
  }

  return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void EzUIWindow::OnCreate() {
}

void EzUIWindow::OnDestroy() {
}

void EzUIWindow::OnPaint(HDC hdc) {
  TextOut(hdc, 10, 10, L"Hello, EzUIWindow!", 18);
}

void EzUIWindow::OnSize(int width, int height) {
}

void EzUIWindow::OnVScroll(int pos, int flags) {
}

void EzUIWindow::OnHScroll(int pos, int flags) {
}

void EzUIWindow::OnMouseWheel(int delta) {
}

void EzUIWindow::OnKeyDown(UINT vKey) {
}

void EzUIWindow::OnMouseMove(int x, int y) {
}

void EzUIWindow::OnLButtonDown(int x, int y) {
}

void EzUIWindow::OnLButtonUp(int x, int y) {
}

void EzUIWindow::OnMouseLeave() {

}

void EzUIWindow::Create(int x, int y, int width, int height, HWND parent) {
  mWnd = CreateWindow(EZUI_WND_CLASS, L"ezui_window", WS_CHILD | WS_VISIBLE,
    x, y, width, height, parent, NULL, mInst, (LPVOID)this);
}
