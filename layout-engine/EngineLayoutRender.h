#pragma once


#include "core/EzUIWindow.h"

void EngineLayout_InitGDIPlus();

void EngineLayout_InitUILayout(EzUIWindow* wnd);

void EngineLayout_RenderUI(EzUIWindow* wnd, HDC hdc);

void EngineLayout_Resize(EzUIWindow* wnd, int width, int height);

