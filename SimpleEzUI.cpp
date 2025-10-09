// SimpleEzUI.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "SimpleEzUI.h"
#include "EzUIAppWindow.h"
#include "EzUIWindow.h"

// 全局变量:
HINSTANCE hInst;

EzUIWindow* mainWindow = nullptr;

int APIENTRY wWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPWSTR lpCmdLine,
  int nCmdShow) {

  EzUIAppWindow wnd(hInstance);
  wnd.Create();

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEEZUI));

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



