#pragma once

class ComScope {
public:
  explicit ComScope(DWORD dwCoInit = COINIT_APARTMENTTHREADED)
    : initialized_(false) {
    HRESULT hr = ::CoInitializeEx(nullptr, dwCoInit);
    if (SUCCEEDED(hr))
      initialized_ = true;
  }

  ~ComScope() {
    if (initialized_)
      ::CoUninitialize();
  }

  // ½ûÖ¹¿½±´£¨·ÀÖ¹Îö¹¹¶à´Î CoUninitialize£©
  ComScope(const ComScope&) = delete;
  ComScope& operator=(const ComScope&) = delete;

private:
  bool initialized_;
};