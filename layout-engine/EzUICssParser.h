#pragma once

#include <string>
#include <cwctype>
#include <string_view>
#include "EzUIDocNode.h"
#include "EzUIBaseParser.h"
#include "EzUICssNode.h"

class EzUICssParser : public EzUIBaseParser {
public:
  EzUICssParser(const std::wstring_view& src) :EzUIBaseParser(src) {}

  static std::unique_ptr<Stylesheet> Parse(const std::wstring& src);

protected:
  std::vector<Rule> ParseRules();
  Rule ParseRule();

  std::vector<Selector> ParseSelectors();
  SimpleSelector ParseSimpleSelector();

  std::vector<Declaration> ParseDeclarations();
  Declaration ParseDeclaration();

  std::wstring ParseIdentifier();

  std::vector<std::wstring> ParsePropertys();
  std::wstring ParseProperty();



};
