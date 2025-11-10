// SimpleEzUI.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "SimpleEzUI.h"
#include "core/EzUIApp.h"
#include "core/EzUIAppWindow.h"
#include "core/EzUIWindow.h"
#include "layout-engine/EngineEditBoxRender.h"
#include "layout-engine/EngineLayoutRender.h"
#include "common/file_watch.h"
#include <io.h>
#include <fcntl.h>
#include <locale>
#include <utils/WinConsole.h>
#include <utils/GdiPlusScope.h>
#include <utils/ComScope.h>
#include "layout-engine/EzUiLayoutBox.h"

EzUIAppWindow* appWindow = nullptr;
EzUIWindow* mainWindow = nullptr;

// extern UIElement* uiRoot;
extern EzUiLayoutBox* gLayoutRoot;

static void CreateWindows(HINSTANCE hInstance) {
  appWindow = new EzUIAppWindow(hInstance);

  appWindow->Created += [&](EzUIAppWindow* appWnd) {
    RECT rect = appWnd->GetClientRect();

    mainWindow = new EzUIWindow(hInstance);
#if 0
    mainWindow->Draw += EngineEditBoxRender;
    mainWindow->Created += EngineEditBoxInit;
    mainWindow->Resized += EngineEditBoxResize;
    mainWindow->CharInputed += EngineEditBoxCharInput;
    mainWindow->KeyDown += EngineEditBoxKeyDown;
    mainWindow->TimerCalled += EngineEditBoxTimer;
    mainWindow->LButtonDown += EngineEditBoxLButtonDown;
    mainWindow->LButtonUp += EngineEditBoxLButtonUp;
    mainWindow->MouseMoved += EngineEditBoxMouseMove;
#endif
    mainWindow->Created += [](auto wnd) {
      EngineLayout_InitUILayout(wnd);

      auto width = YGNodeStyleGetWidth(gLayoutRoot->ygNode);
      auto height = YGNodeStyleGetHeight(gLayoutRoot->ygNode);

      if (width.unit != YGUnitAuto && height.unit != YGUnitAuto) {
#if 1
        RECT rc;
        GetWindowRect(appWindow->GetHwnd(), &rc);
        rc.right = rc.left + (LONG)width.value;
        rc.bottom = rc.top + (LONG)height.value;

        ::SetWindowPos(appWindow->GetHwnd(), NULL, rc.left, rc.top, width.value, height.value, SWP_FRAMECHANGED);
#endif
      }
    };
    mainWindow->Resized += EngineLayout_Resize;
    mainWindow->Draw += EngineLayout_RenderUI;

    mainWindow->Create(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, appWnd->GetHwnd());

    SetFocus(mainWindow->GetHwnd());
  };

  appWindow->Resized += [&](EzUIAppWindow* appWnd, int width, int height) {
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

  appWindow->MouseMove += [&](EzUIAppWindow* appWnd, int x, int y) {
    EngineLayout_HitTest(appWnd, x, y);
  };

  appWindow->LButtonDown += [&](EzUIAppWindow* appWnd, int x, int y) {
    EngineLayout_LButtonDown(appWnd, x, y);
  };

  appWindow->LButtonUp += [&](EzUIAppWindow* appWnd, int x, int y) {
    EngineLayout_LButtonUp(appWnd, x, y);
  };

  appWindow->QueryCursor += [&](EzUIAppWindow* appWnd) {
    // 告诉系统我们会自己处理鼠标形状
    EngineLayout_QueryCursor(appWnd);
    return true;
  };

  appWindow->Create();

}

static void HotReloadInit() {
  static FileWatcher watch(L"./SimpleEzUI.ezui", []() {
    std::wcout << L"文件已修改！" << std::endl;
    // 这里可以安全调用你的热重载函数
    EngineLayout_InitUILayout(mainWindow);

    auto rc = mainWindow->GetClientRect();
    EngineLayout_Resize(mainWindow, rc.right - rc.left, rc.bottom - rc.top);

    // DumpYogaTree(uiRoot);
  });
}

HINSTANCE gInstance;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
  gInstance = hInstance;

  ComScope com;
  GdiPlusScope gdip;
  WinConsole console;

  CreateWindows(hInstance);

  HotReloadInit();

  EzUIApp app(hInstance);
  int nRet = app.Run();

  return nRet;
}





