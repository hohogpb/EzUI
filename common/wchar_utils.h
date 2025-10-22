#pragma once

#include <cwctype>

inline bool IsSpace(wchar_t c) { return std::iswspace(c); }
inline bool IsAlpha(wchar_t c) { return std::iswalpha(c); }
inline bool IsDigit(wchar_t c) { return std::iswdigit(c); }
inline bool IsAlnum(wchar_t c) { return std::iswalnum(c); }
inline bool IsPunct(wchar_t c) { return std::iswpunct(c); }
inline bool IsUpper(wchar_t c) { return std::iswupper(c); }
inline bool IsLower(wchar_t c) { return std::iswlower(c); }

inline std::wstring Trim(const std::wstring_view& s) {
  size_t start = 0;
  while (start < s.size() && IsSpace(s[start])) start++;
  size_t end = s.size();
  while (end > start && IsSpace(s[end - 1])) end--;
  return std::wstring(s.substr(start, end - start));
}