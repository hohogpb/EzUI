#pragma once

#include "core/EzUIWindow.h"

void EngineRenderInit(EzUIWindow* wnd);

void EngineRenderResize(EzUIWindow* wnd, int width, int height);

void EngineRender(EzUIWindow* wnd, HDC hdc);

void EngineRenderCharInput(EzUIWindow* wnd, UINT vChar);

void EngineRenderKeyDown(EzUIWindow* wnd, UINT vKey);

void EngineRenderLButtonDown(EzUIWindow* wnd, int x, int y);

void EngineRenderTimer(EzUIWindow* wnd, UINT timerId);

