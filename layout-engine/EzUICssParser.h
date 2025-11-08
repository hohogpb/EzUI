#pragma once

#include <string>
#include <cwctype>
#include <string_view>
#include "EzUIDocNode.h"
#include "EzUIParser.h"
#include "EzUICssNode.h"

class EzUICssParser : public EzUIParser {
public:
  EzUICssParser(const std::wstring_view& src) :EzUIParser(src) {}

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
