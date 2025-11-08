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
      if (selector.pseudo)
        std::wcout << ":" << * selector.pseudo;
      std::wcout << " ";
    }

    for (const auto& decl : rule.declarations) {
      std::wcout << decl.name << " -> ";
      for (const auto& prop : decl.value) {
        std::wcout << prop << " ";
      }
      std::wcout << std::endl;
    }
  }
}

#endif