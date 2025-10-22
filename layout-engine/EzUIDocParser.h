#pragma once

#include <string>
#include <cwctype>
#include <string_view>
#include "EzUIDocNode.h"

class EzUIDocParser {
public:
  EzUIDocParser() : mSrc(), mPos(0) {}

  std::unique_ptr<EzUIDocNode> ParseFile(const std::wstring& docPath);
  std::unique_ptr<EzUIDocNode> ParseText(const std::wstring& docText);

private:
  std::wstring_view mSrc;
  size_t mPos;

  bool StartsWith(const std::wstring_view& s) const;

  wchar_t Peek() const;

  wchar_t Get();

  void SkipWhitespace();

  void SkipComment();

  std::wstring ParseIdentifier();

  std::wstring ParseQuotedString();

  std::unique_ptr<EzUIDocNode> ParseNode();

  std::unique_ptr<EzUIDocNode> Parse();
};

