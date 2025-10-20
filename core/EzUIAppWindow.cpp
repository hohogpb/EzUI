#include "pch.h"
#include "EzUIAppWindow.h"

using std::unordered_map;
using std::function;
using std::optional;

// we cannot just use WS_POPUP style
// WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
// WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the task bar item)
// WS_CAPTION: enables aero minimize animation/transition
// WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable minimize/maximize
static enum class Style : DWORD {
  windowed = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
  aero_borderless = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
  basic_borderless = WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
};

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

static Style SelectBorderlessStyle() {
  if (IsCompositionEnabled())
    return Style::aero_borderless;
  return Style::basic_borderless;
}

static void SetShadow(HWND handle, bool enabled) {
  if (IsCompositionEnabled()) {
    static const MARGINS shadowState[2] = {
      { 0,0,0,0 }, { 1,1,1,1 },
    };
    ::DwmExtendFrameIntoClientArea(handle, &shadowState[enabled]);
  }
}

EzUIAppWindow::EzUIAppWindow(HINSTANCE hInst) :mInst(hInst), mWnd(NULL) {
  CheckkRegisterClass();
}

EzUIAppWindow::~EzUIAppWindow() {}

void EzUIAppWindow::CheckkRegisterClass() {
  WNDCLASS wc;
  if (!GetClassInfo(mInst, EZUI_APPWND_CLASS, &wc)) {
    ZeroMemory(&wc, sizeof(wc));
    wc.style = CS_DBLCLKS /*| CS_HREDRAW | CS_VREDRAW*/;
    wc.lpfnWndProc = (WNDPROC)EzUIAppWindow::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    // wc.cbWndExtra = sizeof(LONG_PTR); // 为 GWLP_USERDATA 预留空间
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

LRESULT CALLBACK EzUIAppWindow::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  EzUIAppWindow* pThis = NULL;
  if (::IsWindow(hWnd)) {
    pThis = (EzUIAppWindow*)GetProp(hWnd, TEXT("browser_this"));
    // pThis = (EzUIWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (pThis && pThis->mWnd != hWnd) {
      pThis = NULL;
    }
  }

  if (uMessage == WM_CREATE) {
    LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
    pThis = (EzUIAppWindow*)(lpcs->lpCreateParams);
    SetProp(hWnd, TEXT("browser_this"), (HANDLE)pThis);
    // SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
    pThis->mWnd = hWnd;
  }

  if (!pThis)
    return DefWindowProc(hWnd, uMessage, wParam, lParam);

  return pThis->OnMessage(uMessage, wParam, lParam);
}

using MsgHandler = function<LRESULT(EzUIAppWindow*, HWND, UINT, WPARAM, LPARAM)>;

static LRESULT ON_WM_CREATE(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->Created.emit(win);
  return DefWindowProc(hwnd, msg, w, l);
};

static LRESULT ON_WM_PAINT(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
#if 1
  win->Draw.emit(win, NULL);
  return 0;
#else
  // 下面的代码会导致闪烁，
  // 因为我们当前是在子窗口上绘制的 子窗口绘制之前，父窗口这次绘制会清空背景
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);
  win->Draw.emit(win, hdc);
  EndPaint(hwnd, &ps);
  return 0;
  return DefWindowProc(hwnd, msg, w, l);
#endif
};

static LRESULT ON_WM_SIZE(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  int width = LOWORD(l);
  int height = HIWORD(l);
  win->Resized.emit(win, width, height);
  return 0;
};

static LRESULT ON_WM_MOUSEWHEEL(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  short delta = GET_WHEEL_DELTA_WPARAM(w);
  win->MouseWheel.emit(win, delta);
  return 0;
};

static LRESULT ON_WM_MOUSEMOVE(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  TRACKMOUSEEVENT tme;
  ZeroMemory(&tme, sizeof(TRACKMOUSEEVENT));
  tme.cbSize = sizeof(TRACKMOUSEEVENT);
  tme.dwFlags = TME_QUERY;
  tme.hwndTrack = hwnd;
  TrackMouseEvent(&tme);
  if (!(tme.dwFlags & TME_LEAVE)) {
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
  }
  win->MouseMove.emit(win, GET_X_LPARAM(l), GET_Y_LPARAM(l));
  return 0;
};

static LRESULT ON_WM_MOUSELEAVE(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->MouseLeave.emit(win);
  return 0;
};

static LRESULT ON_WM_KEYDOWN(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->KeyDown.emit(win, (UINT)w);
  return 0;
};

static LRESULT ON_WM_LBUTTONDOWN(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->LButtonDown.emit(win, GET_X_LPARAM(l), GET_Y_LPARAM(l));
  return 0;
}

static LRESULT ON_WM_LBUTTONUP(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->LButtonUp.emit(win, GET_X_LPARAM(l), GET_Y_LPARAM(l));
  return 0;
}

static LRESULT ON_WM_CHAR(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->Input.emit(win, (TCHAR)w);
  return 0;
}

static LRESULT ON_WM_TIMER(EzUIAppWindow* win, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  win->Tick.emit(win, (UINT)w); // timerId
  return 0;
}

static LRESULT ON_WM_DESTROY(EzUIAppWindow* window, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  RemoveProp(hwnd, TEXT("browser_this"));
  // SetWindowLongPtr(hWnd, GWLP_USERDATA, 0); 
  // delete pThis;
  return 0;
};

static LRESULT ON_WM_CLOSE(EzUIAppWindow* window, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  PostQuitMessage(0);
  return 0;
};

static LRESULT ON_WM_COMMAND(EzUIAppWindow* window, HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  int wmId = LOWORD(w);
#if 0
  // 分析菜单选择:
  switch (wmId) {
  case IDM_EXIT:
    DestroyWindow(hWnd);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
#endif
  return DefWindowProc(hwnd, msg, w, l);
};

static LRESULT ON_WM_ACTIVATE(EzUIAppWindow* window, HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  int status = LOWORD(wParam);
  window->Activated.emit(window, status);
  return 0;
}

static LRESULT ON_WM_NCCALCSIZE(EzUIAppWindow* window, HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  if (wParam == TRUE && window->borderless) {
    auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
    AdjustMaximizedClientRect(hwnd, params.rgrc[0]);
    return 0;
  }
  return DefWindowProc(hwnd, uMessage, wParam, lParam);
}

static LRESULT ON_WM_NCHITTEST(EzUIAppWindow* win, HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  if (!win->borderless)
    return DefWindowProc(hwnd, uMessage, wParam, lParam);

  // When we have no border or title bar, we need to perform our
  // own hit testing to allow resizing and moving.
  POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };

  // identify borders and corners to allow resizing the window.
  // Note: On Windows 10, windows behave differently and
  // allow resizing outside the visible window frame.
  // This implementation does not replicate that behavior.
  const POINT border{
    ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
    ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
  };

  // notice: get window position
  RECT rect;
  if (!::GetWindowRect(hwnd, &rect)) {
    return HTNOWHERE;
  }

  POINT ptClient = pt;
  ::ScreenToClient(hwnd, &ptClient);

  const auto borderless_drag = win->borderless_drag;
  const auto borderless_resize = win->borderless_resize;

  const auto drag = borderless_drag ? HTCAPTION : HTCLIENT;

  enum region_mask {
    client = 0b0000,
    left = 0b0001,
    right = 0b0010,
    top = 0b0100,
    bottom = 0b1000,
  };

  const auto result =
    left * (pt.x < (rect.left + border.x)) |
    right * (pt.x >= (rect.right - border.x)) |
    top * (pt.y < (rect.top + border.y)) |
    bottom * (pt.y >= (rect.bottom - border.y));

  switch (result) {
  case left: return borderless_resize ? HTLEFT : drag;
  case right: return borderless_resize ? HTRIGHT : drag;
  case top: return borderless_resize ? HTTOP : drag;
  case bottom: return borderless_resize ? HTBOTTOM : drag;
  case top | left: return borderless_resize ? HTTOPLEFT : drag;
  case top | right: return borderless_resize ? HTTOPRIGHT : drag;
  case bottom | left: return borderless_resize ? HTBOTTOMLEFT : drag;
  case bottom | right: return borderless_resize ? HTBOTTOMRIGHT : drag;
  case client: {
    ON_WM_MOUSEMOVE(win, hwnd, uMessage, wParam, MAKELPARAM(ptClient.x, ptClient.y));
    return drag;
  }
  default: return HTNOWHERE;
  }
}

static LRESULT ON_WM_NCACTIVATE(EzUIAppWindow* window, HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  if (!IsCompositionEnabled()) {
    // Prevents window frame reappearing on window activation
    // in "basic" theme, where no aero shadow is present.
    return 1;
  }
  return DefWindowProc(hwnd, uMessage, wParam, lParam);
}

static LRESULT ON_WM_ERASEBKGND(EzUIAppWindow* window, HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  return false;
}

static unordered_map<UINT, MsgHandler> msgHandlers = {
  {WM_CREATE, ON_WM_CREATE},
  {WM_PAINT, ON_WM_PAINT},
  {WM_SIZE, ON_WM_SIZE},
  {WM_MOUSEWHEEL, ON_WM_MOUSEWHEEL},
  {WM_MOUSEMOVE, ON_WM_MOUSEMOVE},
  {WM_MOUSELEAVE, ON_WM_MOUSELEAVE},
  {WM_KEYDOWN, ON_WM_KEYDOWN},
  {WM_LBUTTONDOWN, ON_WM_LBUTTONDOWN},
  {WM_LBUTTONUP, ON_WM_LBUTTONUP},
  {WM_CHAR, ON_WM_CHAR},
  {WM_TIMER, ON_WM_TIMER},
  {WM_DESTROY, ON_WM_DESTROY},
  {WM_CLOSE, ON_WM_CLOSE},
  {WM_COMMAND, ON_WM_COMMAND},
  {WM_ACTIVATE, ON_WM_ACTIVATE},
  {WM_NCCALCSIZE, ON_WM_NCCALCSIZE},
  {WM_NCHITTEST, ON_WM_NCHITTEST},
  {WM_NCACTIVATE, ON_WM_NCACTIVATE},
  {WM_ERASEBKGND, ON_WM_ERASEBKGND},
};

LRESULT EzUIAppWindow::OnMessage(UINT uMessage, WPARAM wParam, LPARAM lParam) {
  auto handlerItr = msgHandlers.find(uMessage);
  if (handlerItr != msgHandlers.end()) {
    auto result = handlerItr->second(this, mWnd, uMessage, wParam, lParam);
    return result;
  }

  return DefWindowProc(mWnd, uMessage, wParam, lParam);
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

void EzUIAppWindow::Invalidate(bool erase) {
  if (mWnd) {
    InvalidateRect(mWnd, nullptr, erase ? TRUE : FALSE);
  }
}

LONG EzUIAppWindow::GetLong(int idx) {
  if (mWnd) {
    return GetWindowLong(mWnd, GWL_STYLE);
  }
  return -1;
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
