#pragma once

#include <string>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>

class FileWatcher {
public:
  // debounceMs �����ڶ���޸�ֻ����һ�λص�
  FileWatcher(const std::wstring& filePath, std::function<void()> callback, int debounceMs = 200)
    : callback(std::move(callback)), running(true), debounceMs(debounceMs) {
    // ��������·��
    wchar_t fullPath[MAX_PATH];
    if (GetFullPathNameW(filePath.c_str(), MAX_PATH, fullPath, nullptr) == 0) {
      std::wcerr << L"�޷������ļ�·��: " << filePath << std::endl;
      return;
    }

    std::wstring absPath = fullPath;
    size_t pos = absPath.find_last_of(L"\\/");
    dir = (pos == std::wstring::npos) ? L"." : absPath.substr(0, pos);
    filename = (pos == std::wstring::npos) ? absPath : absPath.substr(pos + 1);

    // ��Ŀ¼���첽 I/O
    dirHandle = CreateFileW(
      dir.c_str(),
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
      nullptr,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
      nullptr);

    if (dirHandle == INVALID_HANDLE_VALUE) {
      std::wcerr << L"�޷���Ŀ¼: " << dir << std::endl;
      return;
    }

    watcher = std::thread([this]() { watch(); });
  }

  ~FileWatcher() {
    stop();
  }

  void stop() {
    running = false;
    if (dirHandle != INVALID_HANDLE_VALUE)
      CancelIoEx(dirHandle, nullptr);
    if (watcher.joinable())
      watcher.join();
    if (dirHandle != INVALID_HANDLE_VALUE) {
      CloseHandle(dirHandle);
      dirHandle = INVALID_HANDLE_VALUE;
    }
  }

private:
  void watch() {
    char buffer[4096];
    OVERLAPPED ov = {};
    DWORD bytesReturned;

    auto lastTrigger = std::chrono::steady_clock::now() - std::chrono::milliseconds(debounceMs);

    while (running) {
      BOOL ok = ReadDirectoryChangesW(
        dirHandle,
        buffer,
        sizeof(buffer),
        FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_ATTRIBUTES |
        FILE_NOTIFY_CHANGE_SIZE |
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_LAST_ACCESS |
        FILE_NOTIFY_CHANGE_CREATION |
        FILE_NOTIFY_CHANGE_SECURITY,
        &bytesReturned,
        &ov,
        nullptr);

      if (!ok) break;

      // �첽�ȴ���ɻ� CancelIoEx �ж�
      DWORD dw = WaitForSingleObjectEx(dirHandle, INFINITE, TRUE);
      if (!running) break;

      //FILE_ACTION_MODIFIED
      FILE_NOTIFY_INFORMATION* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
      do {
        std::wstring changed(info->FileName, info->FileNameLength / sizeof(WCHAR));
        if (_wcsicmp(changed.c_str(), filename.c_str()) == 0) {
          if (info->Action == FILE_ACTION_MODIFIED)
            callback(); 
        }

        if (info->NextEntryOffset == 0)
          break;
        info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(info) + info->NextEntryOffset);
      } while (true);
    }
  }

private:
  std::wstring dir;
  std::wstring filename;
  std::function<void()> callback;
  std::thread watcher;
  std::atomic<bool> running;
  HANDLE dirHandle = INVALID_HANDLE_VALUE;
  int debounceMs;
};
