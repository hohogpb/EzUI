#include "pch.h"
#include "SimpleEzUIConsole.h"
#include "layout-engine/EzUIDocParser.h"
#include <format>
#include <io.h>
#include <fcntl.h>

void PrintXmlTree(const XmlNode* node, int indent = 0) {
  std::wstring pad(indent, L' ');
  std::wcout << pad << L"<" << node->name;

  // 打印属性
  for (auto& [k, v] : node->attributes)
    std::wcout << L" " << k << L"=\"" << v << L"\"";

  if (node->children.empty() && node->text.empty()) {
    std::wcout << L"/>" << std::endl;
    return;
  }

  std::wcout << L">";
  if (!node->text.empty()) std::wcout << node->text;
  std::wcout << std::endl;

  for (auto& c : node->children)
    PrintXmlTree(c.get(), indent + 2);

  std::wcout << pad << L"</" << node->name << L">" << std::endl;
}

int main() {
  //SetConsoleOutputCP(CP_UTF8);        // 或者 CP_UTF16
  _setmode(_fileno(stdout), _O_U16TEXT);

  EzUIDocParser parser;
  auto dom = parser.ParseFile(L"SimpleEzUI.ezui");

  PrintXmlTree(dom.get());

  return 0;
}