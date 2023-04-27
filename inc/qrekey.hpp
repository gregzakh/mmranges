#pragma once

#ifndef _QREKEY_HPP_
#define _QREKEY_HPP_

constexpr auto check(const LSTATUS status) {
  if (ERROR_SUCCESS != status) {
    getlasterror(status);
    throw std::runtime_error("Regisrty operation failed.");
  }
}

struct AutoKey {
  private:
    HKEY key;
  public:
    AutoKey() : key(nullptr) {}

    AutoKey(const AutoKey&) = delete;
    AutoKey& operator=(const AutoKey&) = delete;

    ~AutoKey() {
      if (nullptr != key) {
        auto status = ::RegCloseKey(key);
        if (ERROR_SUCCESS != status) getlasterror(status);
      }
    }

    operation HKEY()   { return key; }
    HKEY* operation&() { return &key; }
};

auto queryfmkey(HKEY key, FILETIME& ft) {
  return ::RegQueryInfoKey(
    key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, &ft
  );
}

#endif
