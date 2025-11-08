#include "pch.h"
#include "EzUIBaseParser.h"
#include <iostream>
#include <fstream>
#include <common/wchar_utils.h>

void EzUIBaseParser::ConsumeWhiteSpace() {
  ConsumeWhile(IsSpace);
}

std::wstring EzUIBaseParser::ConsumeWhile(const std::function<bool(wchar_t)>& test) {
  std::wstring result;
  while (!Eof() && test(NextChar())) {
    result += ConsumeChar();
  }
  return result;
}

wchar_t EzUIBaseParser::ConsumeChar() {
  auto c = NextChar();
  mPos++;
  return c;
}

wchar_t EzUIBaseParser::NextChar() {
  return mPos < mSrc.size() ? mSrc[mPos] : '\0';
}

bool EzUIBaseParser::StartsWith(const std::wstring_view& s) {
  return mSrc.substr(mPos).starts_with(s);
}

void EzUIBaseParser::Expect(const std::wstring_view& s) {
  if (StartsWith(s)) {
    mPos += s.length();
  } else {
    throw std::runtime_error(std::format("Expected some at byte{} but it was not found", mPos));
  }
}

void EzUIBaseParser::ExpectChar(wchar_t c) {
  if (ConsumeChar() != c)
    throw std::runtime_error(std::format("Expected char at byte{} but it was not found", mPos));
}

bool EzUIBaseParser::Eof() {
  return mPos >= mSrc.length();
}
