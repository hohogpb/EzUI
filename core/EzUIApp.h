#pragma once

class EzUIApp {
public:
  EzUIApp(HINSTANCE hInstance);

  int Run();

protected:
  HINSTANCE mInst;
};

