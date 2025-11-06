#pragma once

#include <string>
#include <cwctype>
#include <string_view>
#include "EzUIDocNode.h"
#include "EzUIParser.h"


class EzUIDocParser : public EzUIParser {
public:
  static std::unique_ptr<EzUIDocNode> Parse(const std::wstring& src);

  EzUIDocParser(const std::wstring_view& src) :EzUIParser(src) {}

protected:
  std::vector<std::unique_ptr<EzUIDocNode>> ParseNodes();

  std::unique_ptr<EzUIDocNode> ParseNode();

  std::unique_ptr<EzUIDocNode> ParseComment();

  std::unique_ptr<EzUIDocNode> ParseElement();
  std::wstring ParseName();
  std::unordered_map<std::wstring, std::wstring> ParseAttributes();
  std::pair<std::wstring, std::wstring> ParseAttr();
  std::wstring ParseAttrValue();

  std::unique_ptr<EzUIDocNode> ParseText();

};
