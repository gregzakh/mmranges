#include "inc\common.hpp"

void ntqueryranges(void);
void regqueryranges(void);

int main(void) {
  std::locale::global(std::locale(""));
  if (!win10rhi()) {
    ::SetLastError(1);
    getlasterror();
    return 1;
  }

  regqueryranges();

  return 0;
}

void regqueryranges(void) {
  AutoKey rk;
  auto size{0ul};
  FILETIME ft{};
  std::vector<BYTE> buf(0);

  try {
    check(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
      L"HARDWARE\\RESOURCEMAP\\System Resources\\Physical Memory", 0,
      KEY_QUERY_VALUE | KEY_READ, &rk
    ));
    check(queryfmkey(rk, ft)); // for duration calculation
    check(queryvalue(rk, L".Translated", nullptr, &size));
    buf.resize(size);
    check(queryvalue(rk, L".Translated", &buf[0], &size));

    auto list = reinterpret_cast<PCM_RESOURCE_LIST>(&buf[0]);
    printf("%16s %16s %16s\n%16s %16s %16s\n", "Start", "End", "Size", "------", "----", "-----");
    for (auto i = 0; i < list->Count; i++) {
      for (auto j = 0; j < list->List[i].PartialResourceList.Count; j++) {
        auto tmp = list->List[i].PartialResourceList.PartialDescriptors[j].u.Memory;
        printf("%#16llx %#16llx %16d K\n", tmp.Start.QuadPart, tmp.Start.QuadPart + tmp.Length, tmp.Length / 0x400);
      }
    }
    printf("\n[*] Duration of setting data: %hu sec.\n", getdelta(::RtlTimeToElapsedTimeFields, ft));
  }
  catch (...) {}

  std::vector<BYTE> ().swap(buf);
}
