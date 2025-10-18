#include "pch.h"
#include "EzUIDocParser.h"
#include <iostream>
#include <fstream>

std::wstring EzUIDocParser::ReadFile(const std::wstring& filename) {
  // 打开文件二进制读取
  std::ifstream file(filename, std::ios::binary);
  if (!file)
    return L"";

  // 读取内容到缓冲区
  std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  if (buffer.empty())
    return L"";

  // 检测 BOM
  if (buffer.size() >= 3 &&
    (unsigned char)buffer[0] == 0xEF &&
    (unsigned char)buffer[1] == 0xBB &&
    (unsigned char)buffer[2] == 0xBF) {
    // UTF-8 BOM
    std::string utf8str(buffer.begin() + 3, buffer.end());
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), (int)utf8str.size(), nullptr, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), (int)utf8str.size(), &wstr[0], len);
    return wstr;
  }

  if (buffer.size() >= 2 &&
    (unsigned char)buffer[0] == 0xFF &&
    (unsigned char)buffer[1] == 0xFE) {
    // UTF-16 LE
    std::wstring wstr((wchar_t*)&buffer[2], (wchar_t*)&buffer[0] + buffer.size() / 2);
    return wstr;
  }

  if (buffer.size() >= 2 &&
    (unsigned char)buffer[0] == 0xFE &&
    (unsigned char)buffer[1] == 0xFF) {
    // UTF-16 BE -> 需转换
    std::wstring wstr;
    for (size_t i = 2; i + 1 < buffer.size(); i += 2) {
      wchar_t ch = (buffer[i] << 8) | (unsigned char)buffer[i + 1];
      wstr.push_back(ch);
    }
    return wstr;
  }

  // 无 BOM，假定 UTF-8 或 ANSI
  // 可以先尝试 UTF-8，如果失败再用系统代码页
  std::string s(buffer.begin(), buffer.end());
  int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(), (int)s.size(), nullptr, 0);
  if (len > 0) {
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &wstr[0], len);
    return wstr;
  }

  // fallback ANSI -> Unicode
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), nullptr, 0);
  std::wstring wstr(len, 0);
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), (int)s.size(), &wstr[0], len);
  return wstr;
}

std::wstring EzUIDocParser::Trim(const std::wstring_view& s) {
  size_t start = 0;
  while (start < s.size() && IsSpace(s[start])) start++;
  size_t end = s.size();
  while (end > start && IsSpace(s[end - 1])) end--;
  return std::wstring(s.substr(start, end - start));
}

std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseFile(const std::wstring& docPath) {
  auto docText = ReadFile(docPath);
  return ParseText(docText);
}

std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseText(const std::wstring& docText) {
  mSrc = docText;
  return Parse();
}

std::unique_ptr<EzUIDocNode> EzUIDocParser::Parse() {
  SkipWhitespace();
  if (!StartsWith(L"<"))
    return nullptr;
  return ParseNode();
}

bool EzUIDocParser::StartsWith(const std::wstring_view& s) const {
  return mSrc.substr(mPos, s.size()) == s;
}

wchar_t EzUIDocParser::Peek() const {
  return mPos < mSrc.size() ? mSrc[mPos] : '\0';
}

wchar_t EzUIDocParser::Get() {
  return mPos < mSrc.size() ? mSrc[mPos++] : '\0';
}

void EzUIDocParser::SkipWhitespace() {
  while (mPos < mSrc.size() && IsSpace(mSrc[mPos]))
    ++mPos;
}

void EzUIDocParser::SkipComment() {
  const std::wstring commentStart = L"<!--";
  const std::wstring commentEnd = L"-->";

  if (StartsWith(commentStart)) {
    mPos += 4;
    while (mPos + 3 < mSrc.size() && !StartsWith(commentEnd))
      ++mPos;
    mPos += 3;
  }
}

std::wstring EzUIDocParser::ParseIdentifier() {
  std::wstring id;
  while (mPos < mSrc.size()) {
    auto c = Peek();
    if (IsAlnum(c) || c == '_' || c == '-' || c == ':')
      id.push_back(Get());
    else
      break;
  }
  return id;
}

std::wstring EzUIDocParser::ParseQuotedString() {
  auto quote = Get(); // ' or "
  std::wstring val;
  while (mPos < mSrc.size() && Peek() != quote)
    val.push_back(Get());
  if (Peek() == quote)
    Get();
  return val;
}

std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseNode() {
  if (Peek() != '<') return nullptr;
  Get(); // '<'
  if (StartsWith(L"!--")) {
    mPos--;
    SkipComment();
    SkipWhitespace();
    return ParseNode(); // continue
  }

  // 节点名称
  auto name = ParseIdentifier();

  auto node = std::make_unique<EzUIDocNode>();
  node->name = name;

  // 属性
  while (true) {
    SkipWhitespace();
    if (Peek() == '/' || Peek() == '>' || Peek() == '\0')
      break;

    auto attrName = ParseIdentifier();
    SkipWhitespace();
    if (Get() != '=') break;
    SkipWhitespace();
    auto attrValue = ParseQuotedString();
    node->attributes[attrName] = attrValue;
  }

  SkipWhitespace();
  if (StartsWith(L"/>")) {
    mPos += 2; // 自闭合
    return node;
  }

  if (Get() != '>') return node;

  // 子节点或文本
  while (true) {
    SkipWhitespace();
    if (StartsWith(L"</")) {
      mPos += 2;
      ParseIdentifier(); // 跳过结束标签名
      if (Peek() == '>') Get();
      break;
    }
    if (StartsWith(L"<")) {
      auto child = ParseNode();
      if (child) node->children.push_back(std::move(child));
    } else {
      // 文本节点
      std::wstring text;
      while (mPos < mSrc.size() && Peek() != '<')
        text.push_back(Get());
      node->text = Trim(text);
    }
  }
  return node;
}



