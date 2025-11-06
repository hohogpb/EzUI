#include "pch.h"
#include "test_css_parser.h"
#include "layout-engine/EzUIDocParser.h"
#include "layout-engine/EzUIElement.h"
#include "layout-engine/EzUITreeBuilder.h"
#include "layout-engine/EzUITextLoader.h"
#include <utils/DumpInfo.h>
#include <fcntl.h>
#include <io.h>
#include <layout-engine/EzUICssParser.h>

#if 1

int main() {
  _setmode(_fileno(stdout), _O_U16TEXT);

  EzUITextLoader loader;
  auto source = loader.Load(L"SimpleEzUI.css");

  auto stylesheet = EzUICssParser::Parse(source);

  for (const auto& rule : stylesheet->rules) {
    std::wcout << "Selectors: ";
    for (const auto& selector : rule.selectors) {
      if (selector.tagName)
        std::wcout << *selector.tagName;
      if (selector.id)
        std::wcout << "#" << *selector.id;
      for (const auto& cls : selector.classes) {
        std::wcout << "." << cls;
      }
      std::wcout << " ";
    }

    for (const auto& decl : rule.declarations) {
      std::wcout << decl.name << " -> ";
      if (auto kw = std::get_if<std::wstring>(&decl.value))
        std::wcout << *kw;
      else if (auto len = std::get_if<std::pair<float, Unit>>(&decl.value))
        std::wcout << len->first << "px";
      else if (auto color = std::get_if<Color>(&decl.value))
        std::wcout << "rgba(" << int(color->r) << "," << int(color->g) << "," << int(color->b) << ")";
      std::wcout << std::endl;
    }
  }
}

#endif