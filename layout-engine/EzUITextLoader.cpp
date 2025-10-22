#include "pch.h"
#include "EzUITextLoader.h"

std::wstring EzUITextLoader::Load(const std::wstring& filepath) {
  // 打开文件二进制读取
  std::ifstream file(filepath, std::ios::binary);
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
