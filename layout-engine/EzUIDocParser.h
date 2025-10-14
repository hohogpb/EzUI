#pragma once

#include <string>
#include <cwctype>
#include <string_view>

struct XmlNode {
  std::wstring name;
  std::unordered_map<std::wstring, std::wstring> attributes;
  std::vector<std::unique_ptr<XmlNode>> children;
  std::wstring text;

  XmlNode* FindChild(const std::wstring& tag) {
    for (auto& c : children)
      if (c->name == tag)
        return c.get();
    return nullptr;
  }
};

class EzUIDocParser {
public:
  EzUIDocParser() : mSrc(), mPos(0) {}

  static std::wstring ReadFile(const std::wstring& filename);
  static std::wstring Trim(const std::wstring_view& s);

  static bool IsSpace(wchar_t c) { return std::iswspace(c); }
  static bool IsAlpha(wchar_t c) { return std::iswalpha(c); }
  static bool IsDigit(wchar_t c) { return std::iswdigit(c); }
  static bool IsAlnum(wchar_t c) { return std::iswalnum(c); }
  static bool IsPunct(wchar_t c) { return std::iswpunct(c); }
  static bool IsUpper(wchar_t c) { return std::iswupper(c); }
  static bool IsLower(wchar_t c) { return std::iswlower(c); }

  std::unique_ptr<XmlNode> ParseFile(const std::wstring& docPath);
  std::unique_ptr<XmlNode> ParseText(const std::wstring& docText);

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

  std::unique_ptr<XmlNode> ParseNode();

  std::unique_ptr<XmlNode> Parse();
};

