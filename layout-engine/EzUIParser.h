#pragma once

class EzUIParser {
public:
  EzUIParser(const std::wstring_view& src) : mSrc(src), mPos(0) {}

  /// Consume and discard zero or more whitespace characters.
  void ConsumeWhiteSpace();

  // 消费字符直到 test 返回 false
  std::wstring ConsumeWhile(const std::function<bool(wchar_t)>& test);

  /// Return the current character, and advance self.pos to the next character.
  wchar_t ConsumeChar();

  /// Read the current character without consuming it.
  wchar_t NextChar();

  /// Does the current input start with the given string?
  bool StartsWith(const std::wstring_view& s);

  /// If the exact string `s` is found at the current position, consume it.
  /// Otherwise, panic.
  void Expect(const std::wstring_view& str);

  /// If the exact string `s` is found at the current position, consume it.
 /// Otherwise, panic.
  void ExpectChar(wchar_t c);

  /// Return true if all input is consumed.
  bool Eof();

protected:
  std::wstring_view mSrc;
  size_t mPos;
};


