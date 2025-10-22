#include "pch.h"
#include "EzUIDocParser.h"
#include <iostream>
#include <fstream>
#include "EzUITextLoader.h"
#include <common/wchar_utils.h>


std::unique_ptr<EzUIDocNode> EzUIDocParser::ParseFile(const std::wstring& docPath) {
  EzUITextLoader loader;
  auto text = loader.Load(docPath);
  return ParseText(text);
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



