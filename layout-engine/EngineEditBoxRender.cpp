#include "pch.h"
#include "EngineEditBoxRender.h" 

static ID2D1Factory* g_pD2DFactory = nullptr;
static ID2D1HwndRenderTarget* g_pRenderTarget = nullptr;
static ID2D1SolidColorBrush* g_pBrush = nullptr;
static IDWriteFactory* g_pDWriteFactory = nullptr;
static IDWriteTextFormat* g_pTextFormat = nullptr;
static IDWriteTextLayout* g_pTextLayout = nullptr;

static std::wstring g_text = L"你好大哥\n啊手动阀手动阀;♥阿喀琉斯的风景啊啊是发生发生 heeloo world\n";

static int g_textBoxWidth = 100; // 文本框宽度
static int g_textBoxHeight = 80;

static UINT32 g_cursorPos = 0;
static bool g_showCursor = true;

static bool   g_isSelecting = false;     // 是否正在拖拽选择
static UINT32 g_selectionStart = 0;      // 选区起始字符索引
static UINT32 g_selectionEnd = 0;      // 选区结束字符索引

void EngineEditBoxInit(EzUIWindow* wnd) {
  HWND hwnd = wnd->GetHwnd();

  // D2D
  D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);

  RECT rc;
  GetClientRect(hwnd, &rc);

  g_pD2DFactory->CreateHwndRenderTarget(
    D2D1::RenderTargetProperties(),
    D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right, rc.bottom)),
    &g_pRenderTarget
  );
  g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &g_pBrush);

  // DWrite
  DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_pDWriteFactory);


  g_pDWriteFactory->CreateTextFormat(
    L"Segoe UI", nullptr,
    DWRITE_FONT_WEIGHT_REGULAR,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    25.0f,
    L"zh-cn",
    &g_pTextFormat
  );

  // 初始布局
  g_pDWriteFactory->CreateTextLayout(
    g_text.c_str(), (UINT32)g_text.length(),
    g_pTextFormat, 100, 80.0f,
    &g_pTextLayout
  );

  SetTimer(wnd->GetHwnd(), 1, 500, nullptr); // 光标闪烁
}



// 假设：你用 DWrite 的 HitTestTextPosition 得到 caret 的 (x,y)（相对于 TextLayout 原点）
// textLayoutOffsetX/Y 是你绘制 TextLayout 时的偏移（例如 DrawTextLayout(D2D1::Point2F(offsetX, offsetY), ...)）
void UpdateIMEWindowPosition(HWND hwnd, IDWriteTextLayout* textLayout, UINT32 caretIndex,
  float textLayoutOffsetX = 0.0f, float textLayoutOffsetY = 0.0f) {
  if (!textLayout) return;

  DWRITE_HIT_TEST_METRICS hit;
  FLOAT cx = 0.0f, cy = 0.0f;
  HRESULT hr = textLayout->HitTestTextPosition(caretIndex, FALSE, &cx, &cy, &hit);
  if (FAILED(hr)) return;

  // caret 相对于 textLayout 原点的坐标是 (cx, cy).
  // 加上你绘制时的偏移（margin/滚动）
  FLOAT caretClientX = cx + textLayoutOffsetX;
  FLOAT caretClientY = cy + textLayoutOffsetY;

  // 把客户端坐标转换为屏幕坐标
  POINT pt = { (LONG)round(caretClientX), (LONG)round(caretClientY) };
  // 如果 D2D 使用了 DPI 或缩放变换，先把 DIPs 转实际像素（按你的实现处理）
  // e.g., 

  //ClientToScreen(hwnd, &pt);


  // 设置合成窗口位置
  HIMC hImc = ImmGetContext(hwnd);
  if (hImc) {
    COMPOSITIONFORM cf = {};
    cf.dwStyle = CFS_POINT; // 使用点位置（也可用 CFS_FORCE_POSITION）
    cf.ptCurrentPos.x = pt.x;
    cf.ptCurrentPos.y = pt.y;
    ImmSetCompositionWindow(hImc, &cf);

    // 可选：设置候选窗口（若需要）
    // CANDIDATEFORM cand = {};
    // cand.dwIndex = 0;
    // cand.dwStyle = CFS_CANDIDATEPOS;
    // cand.ptCurrentPos = cf.ptCurrentPos;
    // ImmSetCandidateWindow(hImc, &cand);

    ImmReleaseContext(hwnd, hImc);
  }
}



// -------------------- 更新文本布局 --------------------
static void UpdateTextLayout(RECT rc) {
  if (g_pTextLayout)
    g_pTextLayout->Release();

  g_pDWriteFactory->CreateTextLayout(
    g_text.c_str(), (UINT32)g_text.length(),
    g_pTextFormat,
    (FLOAT)(rc.right - rc.left),
    (FLOAT)(rc.bottom - rc.top),
    &g_pTextLayout
  );
}

void EngineEditBoxRender(EzUIWindow* wnd, HDC hdc) {
#if 0
  // 填充背景
  RECT clientRect;
  GetClientRect(wnd->GetHwnd(), &clientRect);
  FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

  // 绘制一些文本
  const wchar_t* text = L"Hello, EzUI!";
  SetTextColor(hdc, RGB(0, 0, 0));
  SetBkMode(hdc, TRANSPARENT);
  DrawText(hdc, text, -1, &clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  // 显示客户区大小
  wchar_t sizeText[100];
  int width = clientRect.right - clientRect.left;
  int height = clientRect.bottom - clientRect.top;
  swprintf_s(sizeText, L"Size: %d x %d", width, height);
  DrawText(hdc, sizeText, -1, &clientRect, DT_LEFT | DT_TOP | DT_SINGLELINE);
#endif

  g_pRenderTarget->BeginDraw();
  g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


  if (g_selectionStart != g_selectionEnd) {
    UINT32 selStart = min(g_selectionStart, g_selectionEnd);
    UINT32 selEnd = max(g_selectionStart, g_selectionEnd);
    UINT32 selLen = selEnd - selStart;

    std::vector<DWRITE_HIT_TEST_METRICS> metrics(selLen);
    UINT32 actualCount = 0;

    // 获取选区的每个 glyph 的矩形
    g_pTextLayout->HitTestTextRange(
      selStart, selLen,
      0, .0f, // 你 layout 的起点偏移
      metrics.data(), (UINT32)metrics.size(),
      &actualCount
    );

    ID2D1SolidColorBrush* selBrush = nullptr;
    g_pRenderTarget->CreateSolidColorBrush(
      D2D1::ColorF(D2D1::ColorF::SkyBlue, 0.5f),
      &selBrush
    );

    for (UINT32 i = 0; i < actualCount; ++i) {
      const auto& m = metrics[i];
      D2D1_RECT_F rect = D2D1::RectF(
        m.left, m.top,
        m.left + m.width,
        m.top + m.height
      );
      g_pRenderTarget->FillRectangle(rect, selBrush);
    }
    if (selBrush) selBrush->Release();
  }

  // 绘制文本
  g_pRenderTarget->DrawTextLayout(D2D1::Point2F(0, 0), g_pTextLayout, g_pBrush);

  // 绘制光标

  if (g_showCursor && g_cursorPos <= g_text.length()) {
    DWRITE_HIT_TEST_METRICS hit;
    FLOAT x = 0, y = 0;
    g_pTextLayout->HitTestTextPosition(g_cursorPos, FALSE, &x, &y, &hit);
    g_pRenderTarget->DrawLine(
      D2D1::Point2F(x, y),
      D2D1::Point2F(x, y + hit.height),
      g_pBrush, 1.0f
    );
  }

  g_pRenderTarget->EndDraw();

}

// -------------------- 鼠标点击移动光标 --------------------
static void MoveCursorByClick(int mouseX, int mouseY) {
  if (!g_pTextLayout) return;

  BOOL trailing;
  BOOL isInside;
  DWRITE_HIT_TEST_METRICS hit;
  g_pTextLayout->HitTestPoint((FLOAT)mouseX, (FLOAT)mouseY, &trailing, &isInside, &hit);

  UINT pos = hit.textPosition;
  g_cursorPos = trailing ? pos + 1 : pos;
}


void EngineEditBoxResize(EzUIWindow* wnd, int width, int height) {
  // 调整 RenderTarget 大小
  if (g_pRenderTarget) {
    D2D1_SIZE_U size = D2D1::SizeU(width, height);
    g_pRenderTarget->Resize(size);
  }

  UpdateTextLayout({ 0,0,width, height });
  UpdateIMEWindowPosition(wnd->GetHwnd(), g_pTextLayout, g_cursorPos, 0.0f, 0.0f);
  wnd->Invalidate(true);
}


void EngineEditBoxCharInput(EzUIWindow* wnd, UINT vChar) {
  if (vChar == VK_BACK && !g_text.empty() && g_cursorPos > 0) {
    g_text.erase(g_cursorPos - 1, 1);
    g_cursorPos--;
  } else if (vChar == VK_RETURN || vChar == L'\r' || vChar == L'\n') {
    // 插入换行符
    g_text.insert(g_cursorPos, 1, L'\n');
    g_cursorPos++;
  } else if (vChar >= 32) {
    g_text.insert(g_cursorPos, 1, (wchar_t)vChar);
    g_cursorPos++;
  }
  UpdateTextLayout(wnd->GetClientRect());
  UpdateIMEWindowPosition(wnd->GetHwnd(), g_pTextLayout, g_cursorPos, 0.0f, 0.0f);
  wnd->Invalidate(false);
}

void EngineEditBoxKeyDown(EzUIWindow* wnd, UINT vKey) {
  if (vKey == VK_LEFT && g_cursorPos > 0) g_cursorPos--;
  if (vKey == VK_RIGHT && g_cursorPos < g_text.length()) g_cursorPos++;
  if (vKey == VK_DELETE) {
    if (g_cursorPos < g_text.size()) {
      g_text.erase(g_cursorPos, 1);
      UpdateTextLayout(wnd->GetClientRect());
      UpdateIMEWindowPosition(wnd->GetHwnd(), g_pTextLayout, g_cursorPos, 0.0f, 0.0f);
    }
  }
  if (vKey == VK_HOME) {
#if 0
    if (g_pTextLayout) {
      UINT32 start, end;
      // 假设你在 DrawTextLayout 时是从 (5,5) 开始绘制
      float offsetX = 0.0f, offsetY = 0.0f;
      RECT rc = wnd->GetClientRect();
      float layoutWidth = (float)(rc.right - rc.left) - offsetX * 2; // 调整为你实际 layout 宽度
      GetLineStartEndFromCaret(g_pTextLayout, (UINT32)g_cursorPos, offsetX, offsetY, layoutWidth, start, end);
      g_cursorPos = std::min<UINT32>(start, (UINT32)g_text.size());
      // 更新
      UpdateTextLayout(wnd->GetClientRect());
      UpdateIMEWindowPosition(wnd->GetHwnd(), g_pTextLayout, g_cursorPos, 0.0f, 0.0f);
    }
#endif
  }
  if (vKey == VK_END) {
#if 0
    if (g_pTextLayout) {
      UINT32 start, end;
      float offsetX = .0f, offsetY = .0f;
      RECT rc = wnd->GetClientRect();
      float layoutWidth = (float)(rc.right - rc.left) - offsetX * 2;
      GetLineStartEndFromCaret(g_pTextLayout, (UINT32)g_cursorPos, offsetX, offsetY, layoutWidth, start, end);
      g_cursorPos = std::min<UINT32>(end, (UINT32)g_text.size());

      UpdateTextLayout(wnd->GetClientRect());
      UpdateIMEWindowPosition(wnd->GetHwnd(), g_pTextLayout, g_cursorPos, 0.0f, 0.0f);
    }
#endif
  }


  wnd->Invalidate(false);
}

void EngineEditBoxLButtonDown(EzUIWindow* wnd, int x, int y) {

  SetCapture(wnd->GetHwnd());
  g_isSelecting = true;

  // 将点击坐标转换成字符索引
  DWRITE_HIT_TEST_METRICS metrics = {};
  BOOL isTrailing = FALSE;
  BOOL isInside = FALSE;
  g_pTextLayout->HitTestPoint(x, y, &isTrailing, &isInside, &metrics);

  g_selectionStart = g_selectionEnd = metrics.textPosition + (isTrailing ? 1 : 0);
  g_cursorPos = g_selectionEnd;

  MoveCursorByClick(x, y);
  wnd->Invalidate(false);

}

void EngineEditBoxMouseMove(EzUIWindow* wnd, int x, int y) {

  // && (wParam & MK_LBUTTON)
  if (g_isSelecting) {
    POINT pt = { x, y };
    DWRITE_HIT_TEST_METRICS metrics = {};
    BOOL isTrailing = FALSE, isInside = FALSE;

    g_pTextLayout->HitTestPoint(pt.x, pt.y, &isTrailing, &isInside, &metrics);

    g_selectionEnd = metrics.textPosition + (isTrailing ? 1 : 0);
    g_cursorPos = g_selectionEnd;

    wnd->Invalidate(false);
  }

}

void EngineEditBoxLButtonUp(EzUIWindow* wnd, int x, int y) {
  ReleaseCapture();
  g_isSelecting = false;

  wnd->Invalidate(false);
}

void EngineEditBoxTimer(EzUIWindow* wnd, UINT timerId) {
  g_showCursor = !g_showCursor;
  wnd->Invalidate(false);
}

