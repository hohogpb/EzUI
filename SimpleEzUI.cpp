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
    mainWindow->Created += EngineRenderInit;
    mainWindow->Resized += EngineRenderResize;
    mainWindow->CharInputed += EngineRenderCharInput;
    mainWindow->KeyDown += EngineRenderKeyDown;
    mainWindow->LButtonDown += EngineRenderLButtonDown;
    mainWindow->TimerCalled += EngineRenderTimer;

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

  appWindow->Activated += [&](EzUIAppWindow* appWnd, UINT status) {
    if (status != WA_INACTIVE && mainWindow) {
      SetFocus(mainWindow->GetHwnd());
    }
  };

  appWindow->Create();
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  HRESULT Hr = ::CoInitialize(NULL);
  if (FAILED(Hr))  return 0;

  CreateWindows(hInstance);

  EzUIApp app(hInstance);
  int nRet = app.Run();

  ::CoUninitialize();

  return nRet;
}





