#include "pch.h"
#include "SimpleEzUIConsole.h"
#include "layout-engine/EzUIDocParser.h"


int main() {
  EzUIDocParser parser;
  auto dom = parser.ParseFile(L"SimpleEzUI.ezui");

  return 0;
}