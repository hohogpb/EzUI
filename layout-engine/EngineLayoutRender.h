#pragma once


#include "core/EzUIWindow.h"
#include <core/EzUIAppWindow.h>

void EngineLayout_InitGDIPlus();

void EngineLayout_InitUILayout(EzUIWindow* wnd);

void EngineLayout_RenderUI(EzUIWindow* wnd, HDC hdc);

void EngineLayout_Resize(EzUIWindow* wnd, int width, int height);

void EngineLayout_HitTest(EzUIAppWindow* appWnd, int x, int y);

void EngineLayout_LButtonDown(EzUIAppWindow* appWnd, int x, int y) ;

void EngineLayout_LButtonUp(EzUIAppWindow* appWnd, int x, int y);

void EngineLayout_QueryCursor(EzUIAppWindow* appWnd) ;