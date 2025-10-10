// SimpleEzUI.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "SimpleEzUI.h"
#include "core/EzUIApp.h"
#include "core/EzUIAppWindow.h"
#include "core/EzUIWindow.h"
#include "layout-engine/EngineRender.h"

EzUIAppWindow* appWindow = nullptr;
EzUIWindow* mainWindow = nullptr;

static void CreateWindows(HINSTANCE hInstance) {
  appWindow = new EzUIAppWindow(hInstance);

  appWindow->Created += [&](EzUIAppWindow* appWnd) {
    RECT rect = appWnd->GetClientRect();

    mainWindow = new EzUIWindow(hInstance);
    mainWindow->Draw += EngineRender;

    mainWindow->Create(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, appWnd->GetHwnd());

    SetFocus(mainWindow->GetHwnd());
  };

  appWindow->Resized += [&](EzUIAppWindow* appWnd) {
    if (mainWindow) {
      RECT rect = appWnd->GetClientRect();
      SetWindowPos(mainWindow->GetHwnd(), NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
      UpdateWindow(mainWindow->GetHwnd());
    }
  };

  appWindow->Create();
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  CreateWindows(hInstance);

  EzUIApp app(hInstance);
  return app.Run();
}





