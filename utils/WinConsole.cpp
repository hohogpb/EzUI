#include "pch.h"
#include "WinConsole.h"

class WinConsoleWBuf : public std::wstreambuf {
public:
  WinConsoleWBuf(HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE)) : hOut(h) {
    setp(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]) - 1);
  }

protected:
  // flush 缓冲区
  int sync() override {
    return flushBuffer() ? 0 : -1;
  }

  // 当缓冲区满或需要写入字符时调用
  int_type overflow(int_type ch = traits_type::eof()) override {
    if (!flushBuffer()) return traits_type::eof();
    if (ch != traits_type::eof()) {
      buffer[0] = static_cast<wchar_t>(ch);
      setp(buffer + 1, buffer + sizeof(buffer) / sizeof(buffer[0]) - 1);
    }
    return ch;
  }

private:
  bool flushBuffer() {
    std::ptrdiff_t n = pptr() - pbase();
    if (n == 0) return true;
    DWORD written = 0;
    BOOL ok = WriteConsoleW(hOut, pbase(), (DWORD)n, &written, nullptr);
    setp(buffer, buffer + sizeof(buffer) / sizeof(buffer[0]) - 1);
    return ok == TRUE;
  }

  HANDLE hOut;
  wchar_t buffer[256];
};

void WinConsole::Init() {
//#ifdef _DEBUG
  // 创建控制台（如果是 GUI 子系统）
  AllocConsole();

  FILE* fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);
  freopen_s(&fp, "CONIN$", "r", stdin);

  // 替换 std::wcout 缓冲区
  static WinConsoleWBuf winBuf(GetStdHandle(STD_OUTPUT_HANDLE));
  std::wcout.rdbuf(&winBuf);
  std::wcerr.rdbuf(&winBuf);

  std::ios::sync_with_stdio(true);

// 测试输出
//  std::wcout << L"直接绑定 WriteConsoleW 的 std::wcout 测试：" << std::endl;
//  std::wcout << L"中文测试：你好，世界！" << std::endl;
//  std::wcout << L"Emoji 也试试：🚀" << std::endl;
//#endif
}