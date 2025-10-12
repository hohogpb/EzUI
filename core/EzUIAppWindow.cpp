#include "pch.h"
#include "EzUIAppWindow.h"

EzUIAppWindow::EzUIAppWindow(HINSTANCE hInst) {
  mInst = hInst;
  mWnd = NULL;

  //	m_view		= new CHTMLViewWnd(hInst, this);
  //	m_toolbar	= new CToolbarWnd(hInst, this);

  WNDCLASS wc;
  if (!GetClassInfo(mInst, EZUI_APPWND_CLASS, &wc)) {
    ZeroMemory(&wc, sizeof(wc));
    wc.style = CS_DBLCLKS /*| CS_HREDRAW | CS_VREDRAW*/;
    wc.lpfnWndProc = (WNDPROC)EzUIAppWindow::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = mInst;
    wc.hIcon = NULL;
    // wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEEZUI));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    // wc.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEEZUI);
    wc.lpszClassName = EZUI_APPWND_CLASS;
    // wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClass(&wc);
  }
}

#if 0
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_DBLCLKS /*| CS_HREDRAW | CS_VREDRAW*/;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEEZUI));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEEZUI);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}
#endif

EzUIAppWindow::~EzUIAppWindow() {
  // if (m_view)		delete m_view;
}

LRESULT CALLBACK EzUIAppWindow::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  EzUIAppWindow* pThis = NULL;
  if (IsWindow(hWnd)) {
    pThis = (EzUIAppWindow*)GetProp(hWnd, TEXT("browser_this"));
    if (pThis && pThis->mWnd != hWnd) {
      pThis = NULL;
    }
  }

  if (pThis || uMessage == WM_CREATE) {
    switch (uMessage) {
    case WM_ERASEBKGND:
      return TRUE;
    case WM_CREATE:
    {
      LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
      pThis = (EzUIAppWindow*)(lpcs->lpCreateParams);
      SetProp(hWnd, TEXT("browser_this"), (HANDLE)pThis);
      pThis->mWnd = hWnd;
      pThis->OnCreate();
    }
    break;
    case WM_SIZE:
      pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
      return 0;
    case WM_DESTROY:
      RemoveProp(hWnd, TEXT("browser_this"));
      pThis->OnDestroy();
      //delete pThis;
      return 0;
    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;
    case WM_COMMAND:
    {
#if 0
      int wmId = LOWORD(wParam);
      // 分析菜单选择:
      switch (wmId) {
      case IDM_EXIT:
        DestroyWindow(hWnd);
        break;
      default:
        return DefWindowProc(hWnd, message, wParam, lParam);
      }
#endif
    }
    break;
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // TODO: 在此处添加使用 hdc 的任何绘图代码...
      EndPaint(hWnd, &ps);
    }
    return 0;
    case WM_ACTIVATE:
      pThis->OnActivate(LOWORD(wParam));
      return 0;
    }
  }

  return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void EzUIAppWindow::OnCreate() {
  Created.emit(this);
#if 0
  RECT rect;
  GetClientRect(hWnd, &rect);

  if (mainWindow == nullptr) {
    mainWindow = new EzUIWindow(hInst);
    mainWindow->Create(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hWnd);

    SetFocus(mainWindow->GetHwnd());
  }
#endif

#if 0
  RECT rcClient;
  GetClientRect(m_hWnd, &rcClient);
#ifndef NO_TOOLBAR
  m_toolbar->create(rcClient.left, rcClient.top, rcClient.right - rcClient.left, m_hWnd);
  m_view->create(rcClient.left, rcClient.top + m_toolbar->height(), rcClient.right - rcClient.left, rcClient.bottom - rcClient.top - m_toolbar->height(), m_hWnd);
#else
  m_view->create(rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, m_hWnd);
#endif
  SetFocus(m_view->wnd());
#endif
}

void EzUIAppWindow::OnSize(int width, int height) {
  Resized.emit(this);
#if 0
  RECT rcClient;
  GetClientRect(m_hWnd, &rcClient);
#ifndef NO_TOOLBAR
  int toolbar_height = m_toolbar->set_width(rcClient.right - rcClient.left);
#else
  int toolbar_height = 0;
#endif
  SetWindowPos(m_view->wnd(), NULL, rcClient.left, rcClient.top + toolbar_height, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top - toolbar_height, SWP_NOZORDER);
  UpdateWindow(m_view->wnd());
#ifndef NO_TOOLBAR
  SetWindowPos(m_toolbar->wnd(), NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, toolbar_height, SWP_NOZORDER);
  UpdateWindow(m_toolbar->wnd());
#endif
#endif

#if 0
  if (mainWindow) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    SetWindowPos(mainWindow->GetHwnd(), NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    UpdateWindow(mainWindow->GetHwnd());
  }
#endif
}

void EzUIAppWindow::OnActivate(UINT status) {
  this->Activated.emit(this, status);
}

void EzUIAppWindow::OnDestroy() {

}

void EzUIAppWindow::Create(int x, int y, int width, int height) {
  mWnd = CreateWindow(EZUI_APPWND_CLASS, L"EzUIAppWnd", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    x, y, width, height, NULL, NULL, mInst, (LPVOID)this);
  ShowWindow(mWnd, SW_SHOW);
  UpdateWindow(mWnd);
}

RECT EzUIAppWindow::GetClientRect() {
  if (mWnd) {
    RECT rect;
    ::GetClientRect(mWnd, &rect);
    return rect;
  }
  return RECT();
}
