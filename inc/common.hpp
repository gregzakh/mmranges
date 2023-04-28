#pragma once

#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#ifndef UNICODE
  #define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winternl.h>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <vector>
#include <locale>

template<typename... ArgTypes>
auto err(ArgTypes... args) { fprintf(stderr, args...); }

void getlasterror(const DWORD ecode = ::GetLastError()) {
  HLOCAL loc{};
  auto size = ::FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
    nullptr, ecode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPWSTR>(&loc), 0, nullptr
  );
  if (!size)
    err("[?] Unknown error has been occured.\n");
  else
    err("[!] %.*ws\n", size - 1, reinterpret_cast<LPWSTR>(loc));

  if (nullptr != ::LocalFree(loc))
    err("[!] LocalFree (%d) fatal error.\n", ::GetLastError());
}

#ifndef NT_PROCESSOR_GROUPS
  #define NT_PROCESSOR_GROUPS 1
#endif

#pragma comment (lib, "advapi32.lib")
#pragma comment (lib, "ntdll.lib")

#include "qktime.hpp"
#include "qntrtl.hpp"
#include "qrekey.hpp"

constexpr auto win10rhi(void) {
  return 0xA <= *reinterpret_cast<PULONG>(0x7FFE026C);
}
// This value specifies which API version to use when requesting
// Superfetch data. Must be greater or equal to 0x3F65 for V2,
// otherwise вЂ” V1 (see ntapi.hpp for details).
static const auto winbuild = *reinterpret_cast<PULONG>(0x7FFE0260);
//--------------------------------------------------//
//                    | Page size |                 //
//        CPU         |-----------|   Reasonable    //
//                    | Fit | Lrg |                 //
//--------------------|-----------|-----------------//
// x86_32 without PAE | 4KB | 4MB |    4KB only     //
//    x86_32 with PAE | 4KB | 2MB |    4KB only     //
//             x86_64 | 4KB | 2MB |    4KB only     //
//                SH4 | 4KB |     |    1KB, 4KB     //
//               MIPS | 4KB |     |    1KB, 4KB     //
//            PowerPC | 4KB |     |    4KB only     //
//          Alpha AXP | 8KB |     | 8KB, 16KB, 32KB //
//       Alpha AXP_64 | 8KB |     | 8KB, 16KB, 32KB //
//            Itanium | 8KB |     |    4KB, 8KB     //
//            AArch32 | 4KB | n/a |    1KB, 4KB     //
//            AArch64 | 4KB | 2MB |    4KB onky     //
//--------------------------------------------------//
constexpr auto PageSize = 0x1000;
constexpr auto KBytesPerPage = 0x4;
constexpr auto Kb = 0x400;

#endif
