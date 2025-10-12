#pragma once

#include "core/EzUIWindow.h"

void EngineEditBoxInit(EzUIWindow* wnd);

void EngineEditBoxResize(EzUIWindow* wnd, int width, int height);

void EngineEditBoxRender(EzUIWindow* wnd, HDC hdc);

void EngineEditBoxCharInput(EzUIWindow* wnd, UINT vChar);

void EngineEditBoxKeyDown(EzUIWindow* wnd, UINT vKey);

void EngineEditBoxLButtonDown(EzUIWindow* wnd, int x, int y);

void EngineEditBoxLButtonUp(EzUIWindow* wnd, int x, int y);

void EngineEditBoxMouseMove(EzUIWindow* wnd, int x, int y);

void EngineEditBoxTimer(EzUIWindow* wnd, UINT timerId);

