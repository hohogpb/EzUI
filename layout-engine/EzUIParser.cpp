#include "pch.h"
#include "EzUIParser.h"
#include <iostream>
#include <fstream>
#include <common/wchar_utils.h>

void EzUIParser::ConsumeWhiteSpace() {
  ConsumeWhile(IsSpace);
}

std::wstring EzUIParser::ConsumeWhile(const std::function<bool(wchar_t)>& test) {
  std::wstring result;
  while (!Eof() && test(NextChar())) {
    result += ConsumeChar();
  }
  return result;
}

wchar_t EzUIParser::ConsumeChar() {
  auto c = NextChar();
  mPos++;
  return c;
}

wchar_t EzUIParser::NextChar() {
  return mPos < mSrc.size() ? mSrc[mPos] : '\0';
}

bool EzUIParser::StartsWith(const std::wstring_view& s) {
  return mSrc.substr(mPos).starts_with(s);
}

void EzUIParser::Expect(const std::wstring_view& s) {
  if (StartsWith(s)) {
    mPos += s.length();
  } else {
    throw std::runtime_error(std::format("Expected some at byte{} but it was not found", mPos));
  }
}

void EzUIParser::ExpectChar(wchar_t c) {
  if (ConsumeChar() != c)
    throw std::runtime_error(std::format("Expected char at byte{} but it was not found", mPos));
}

bool EzUIParser::Eof() {
  return mPos >= mSrc.length();
}
