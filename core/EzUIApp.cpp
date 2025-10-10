#include "pch.h"
#include "EzUIApp.h"
#include "Resource.h"

EzUIApp::EzUIApp(HINSTANCE hInstance) :mInst(hInstance) {

}

int EzUIApp::Run() {
  HACCEL hAccelTable = LoadAccelerators(mInst, MAKEINTRESOURCE(IDC_SIMPLEEZUI));

  MSG msg;

  // 主消息循环:
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}
