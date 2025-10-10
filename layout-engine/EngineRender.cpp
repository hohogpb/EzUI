#include "pch.h"
#include "EngineRender.h"

void EngineRender(EzUIWindow* wnd, HDC hdc) {
  // ��䱳��
  RECT clientRect;
  GetClientRect(wnd->GetHwnd(), &clientRect);
  FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

  // ����һЩ�ı�
  const wchar_t* text = L"Hello, EzUI!";
  SetTextColor(hdc, RGB(0, 0, 0));
  SetBkMode(hdc, TRANSPARENT);
  DrawText(hdc, text, -1, &clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  // ��ʾ�ͻ�����С
  wchar_t sizeText[100];
  int width = clientRect.right - clientRect.left;
  int height = clientRect.bottom - clientRect.top;
  swprintf_s(sizeText, L"Size: %d x %d", width, height);
  DrawText(hdc, sizeText, -1, &clientRect, DT_LEFT | DT_TOP | DT_SINGLELINE);
}
