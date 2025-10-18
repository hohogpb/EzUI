#include "pch.h"
#include "EzUIAppWindow.h"

static bool IsWindowMaximized(HWND hwnd) {
  WINDOWPLACEMENT placement;
  if (!::GetWindowPlacement(hwnd, &placement)) {
    return false;
  }
  return placement.showCmd == SW_MAXIMIZE;
}

/* Adjust client rect to not spill over monitor edges when maximized.
 * rect(in/out): in: proposed window rect, out: calculated client rect
 * Does nothing if the window is not maximized.
 */
static void AdjustMaximizedClientRect(HWND window, RECT& rect) {
  if (!IsWindowMaximized(window)) return;

  auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
  if (!monitor) return;

  MONITORINFO monitorInfo{};
  monitorInfo.cbSize = sizeof(monitorInfo);
  if (!::GetMonitorInfoW(monitor, &monitorInfo)) return;

  // when maximized, make the client area fill just the monitor (without task bar) rect,
  // not the whole window rect which extends beyond the monitor.
  rect = monitorInfo.rcWork;
}

static bool IsCompositionEnabled() {
  BOOL compositionEnabled = FALSE;
  bool success = ::DwmIsCompositionEnabled(&compositionEnabled) == S_OK;
  return compositionEnabled && success;
}

static EzUIAppWindow::Style SelectBorderlessStyle() {
  if (IsCompositionEnabled())
    return EzUIAppWindow::Style::aero_borderless;
  return EzUIAppWindow::Style::basic_borderless;
}

static void SetShadow(HWND handle, bool enabled) {
  if (IsCompositionEnabled()) {
    static const MARGINS shadowState[2] = {
      { 0,0,0,0 }, { 1,1,1,1 },
    };
    ::DwmExtendFrameIntoClientArea(handle, &shadowState[enabled]);
  }
}

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

LRESULT EzUIAppWindow::HitTest(POINT cursor) {
  // identify borders and corners to allow resizing the window.
  // Note: On Windows 10, windows behave differently and
  // allow resizing outside the visible window frame.
  // This implementation does not replicate that behavior.
  const POINT border{
    ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
    ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
  };

  RECT window;
  if (!::GetWindowRect(mWnd, &window)) {
    return HTNOWHERE;
  }

  const auto drag = borderless_drag ? HTCAPTION : HTCLIENT;

  enum region_mask {
    client = 0b0000,
    left = 0b0001,
    right = 0b0010,
    top = 0b0100,
    bottom = 0b1000,
  };

  const auto result =
    left * (cursor.x < (window.left + border.x)) |
    right * (cursor.x >= (window.right - border.x)) |
    top * (cursor.y < (window.top + border.y)) |
    bottom * (cursor.y >= (window.bottom - border.y));

  switch (result) {
  case left: return borderless_resize ? HTLEFT : drag;
  case right: return borderless_resize ? HTRIGHT : drag;
  case top: return borderless_resize ? HTTOP : drag;
  case bottom: return borderless_resize ? HTBOTTOM : drag;
  case top | left: return borderless_resize ? HTTOPLEFT : drag;
  case top | right: return borderless_resize ? HTTOPRIGHT : drag;
  case bottom | left: return borderless_resize ? HTBOTTOMLEFT : drag;
  case bottom | right: return borderless_resize ? HTBOTTOMRIGHT : drag;
  case client: return drag;
  default: return HTNOWHERE;
  }
}

void EzUIAppWindow::SetBorderless(bool enabled) {
  Style newStyle = (enabled) ? SelectBorderlessStyle() : Style::windowed;
  Style oldStyle = static_cast<Style>(::GetWindowLongPtrW(mWnd, GWL_STYLE));

  if (newStyle == oldStyle) return;

  borderless = enabled;

  ::SetWindowLongPtrW(mWnd, GWL_STYLE, static_cast<LONG>(newStyle));

  // when switching between borderless and windowed, restore appropriate shadow state
  SetShadow(mWnd, borderless_shadow && (newStyle != Style::windowed));

  // redraw frame
  ::SetWindowPos(mWnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
  ::ShowWindow(mWnd, SW_SHOW);
}

void EzUIAppWindow::SetBorderlessShadow(bool enabled) {
  if (!borderless) return;
  borderless_shadow = enabled;
  SetShadow(mWnd, enabled);
}

LRESULT CALLBACK EzUIAppWindow::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  EzUIAppWindow* pThis = NULL;
  if (IsWindow(hWnd)) {
    pThis = (EzUIAppWindow*)GetProp(hWnd, TEXT("browser_this"));
    if (pThis && pThis->mWnd != hWnd) {
      pThis = NULL;
    }
  }

  if (uMessage == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
    pThis = (EzUIAppWindow*)(lpcs->lpCreateParams);
    SetProp(hWnd, TEXT("browser_this"), (HANDLE)pThis);
    pThis->mWnd = hWnd;
    pThis->OnCreate();
    return DefWindowProc(hWnd, uMessage, wParam, lParam);
  }

  if (!pThis)
    return DefWindowProc(hWnd, uMessage, wParam, lParam);

  switch (uMessage) {
  case WM_SIZE:
    pThis->OnSize(LOWORD(lParam), HIWORD(lParam));
    return 1;
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
  case WM_NCCALCSIZE: {
    if (wParam == TRUE && pThis->borderless) {
      auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
      AdjustMaximizedClientRect(pThis->mWnd, params.rgrc[0]);
      return 0;
    }
    break;
  }
  case WM_NCHITTEST: {
    // When we have no border or title bar, we need to perform our
    // own hit testing to allow resizing and moving.
    if (pThis->borderless) {
      POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
      return pThis->HitTest(pt);
    }
    break;
  }
  case WM_NCACTIVATE: {
    if (!IsCompositionEnabled()) {
      // Prevents window frame reappearing on window activation
      // in "basic" theme, where no aero shadow is present.
      return 1;
    }
    break;
  }
  }

  return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

void EzUIAppWindow::OnCreate() {
  Created.emit(this);
}

void EzUIAppWindow::OnSize(int width, int height) {
  Resized.emit(this);
}

void EzUIAppWindow::OnActivate(UINT status) {
  this->Activated.emit(this, status);
}

void EzUIAppWindow::OnPaint(HDC hdc) {
  // TextOut(hdc, 10, 10, L"Hello, EzUIWindow!", 18);
  Draw.emit(this, hdc);
}

void EzUIAppWindow::OnDestroy() {

}

void EzUIAppWindow::Create(int x, int y, int width, int height) {


  DWORD exStyle = WS_EX_APPWINDOW | WS_EX_CONTROLPARENT;
  DWORD style = WS_BORDER | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU
    | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
  // style &= ~WS_CAPTION;

  exStyle = 0;
  style = (DWORD)Style::aero_borderless;

  mWnd = CreateWindowEx(exStyle, EZUI_APPWND_CLASS, L"EzUIAppWnd", style,
    x, y, width, height, NULL, NULL, mInst, (LPVOID)this);
 
  SetBorderless(borderless);
  SetBorderlessShadow(borderless_shadow);

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

LONG EzUIAppWindow::GetLong(int idx) {
  if (mWnd) {
    return GetWindowLong(mWnd, GWL_STYLE);
  }
  return -1;
}
