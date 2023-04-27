#pragma once

#ifndef _QKTIME_HPP_
#define _QKTIME_HPP_

struct KSYSTEM_TIME {
  ULONG LowPart;
  LONG  HighPart1;
  LONG  HighPart2;

  auto asquad(void) {
    return (reinterpret_cast<PLARGE_INTEGER>(this))->QuadPart;
  }
};
using PKSYSTEM_TIME = KSYSTEM_TIME*;

using CSHORT = SHORT;
struct TIME_FIELDS {
  CSHORT Year;
  CSHORT Month;
  CSHORT Day;
  CSHORT Hour;
  CSHORT Minute;
  CSHORT Second;
  CSHORT Milliseconds;
  CSHORT Weekday;
};
using PTIME_FIELDS = TIME_FIELDS*;

auto getdelta(std::function<void(PLARGE_INTEGER, PTIME_FIELDS)> fn, FILETIME& ft) {
  TIME_FIELDS tf{};
  std::vector<ULONGLONG> vals{0x7FFE0008, 0x7FFE0014, 0x7FFE0020};
  std::transform(vals.begin(), vals.end(), vals.begin(), [](ULONGLONG const addr) {
    return (*reinterpret_cast<PKSYSTEM_TIME>(addr)).asquad();
  });

  auto delta = ( // registry key last modified timestamp minus boot time
    (static_cast<LONGLONG>(ft.dwHightDateTime) << 32 | ft.dwLowDateTime) - vals[2]
  ) - (vals[1] - vals[2] - vals[0]);
  fn(reinterpret_cast<PLARGE_INTEGER>(&delta), &ft);

  return tf.Second;
}

#endif
