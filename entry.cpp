#include "inc\common.hpp"

void ntqueryranges(void);
void regqueryranges(void);

int main(void) {
  NTSTATUS nts{};
  BOOLEAN did{};

  std::locale::global(std::locale(""));
  if (!win10rhi()) {
    ::SetLastError(1);
    getlasterror();
    return 1;
  }

  nts = ::RtlAdjustPrivilege(13 /* SE_PROF_SINGLE_PROCESS_PRIVILEGE */, TRUE, FALSE, &did);
  if (!NT_SUCCESS(nts)) {
    printf("[*] Trying to retrieve required data from registry...\n\n");
    regqueryranges();
  }
  else ntqueryranges();

  return 0;
}

template<typename T>
auto wrapper(T mr, ULONG ver) {
  SUPERFETCH_INFORMATION si{};
  auto req{0ul};
  NTSTATUS nts{};

  mr.Version = ver;
  SiInitData(&si, &mr, sizeof(mr), SuperfetchMemoryRangesQuery);

  nts = ::NtQuerySystemInformation(SystemSuperfetchInformation, &si, sizeof(si), &req);
  if (STATUS_BUFFER_TOO_SMALL != nts) {
    getlasterror(::RtlNtStatusToDosError(nts));
    return 1;
  }

  std::vector<T> buf(req);
  buf[0].Version = ver;
  SiInitData(&si, &buf[0], req, SuperfetchMemoryRangesQuery);
  nts = ::NtQuerySystemInformation(SystemSuperfetchInformation, &si, sizeof(si), &req);
  if (!NT_SUCCESS(nts)) {
    getlasterror(::RtlNtStatusToDosError(nts));
    return 1;
  }
  printf("%16s %16s %16s\n%16s %16s %16s\n", "Start", "End", "Size", "------", "----", "-----");
  for (auto i = 0; i < buf[0].RangeCount; i++) {
    printf("%#16llx %#16llx %16lld K\n", buf[0].Ranges[i].BasePage * PageSize,
      (buf[0].Ranges[i].BasePage + buf[0].Ranges[i].PageCount) * PageSize,
      buf[0].Ranges[i].PageCount * KBytesPerPage
    );
  }

  return 0;
}

void ntqueryranges(void) {
  PF_PHYSICAL_MEMORY_RANGE_INFO_V1 m1{};
  PF_PHYSICAL_MEMORY_RANGE_INFO_V2 m2{};

  0x3F65 <= winbuild ? wrapper(m2, 2) : wrapper(m1, 1);
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
        printf("%#16llx %#16llx %16d K\n", tmp.Start.QuadPart, tmp.Start.QuadPart + tmp.Length, tmp.Length / Kb);
      }
    }
    printf("\n[*] Duration of setting data: %hu sec.\n", getdelta(::RtlTimeToElapsedTimeFields, ft));
  }
  catch (...) {}

  std::vector<BYTE> ().swap(buf);
}
