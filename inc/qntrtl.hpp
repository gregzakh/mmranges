#pragma once

#ifndef _QNTRTL_HPP_
#define _QNTRTL_HPP_

constexpr auto SystemSuperfetchInformation = static_cast<SYSTEM_INFORMATION_CLASS>(0x4F);
constexpr auto STATUS_BUFFER_TOO_SMALL = static_cast<NTSTATUS>(0xC0000023);

struct PF_PHYSICAL_MEMORY_RANGE {
  ULONG_PTR BasePage;
  ULONG_PTR PageCount;
};

struct PF_PHYSICAL_MEMORY_RANGE_INFO_V1 {
  ULONG Version;
  ULONG RangeCount;
  PF_PHYSICAL_MEMORY_RANGE Ranges[ANYSIZE_ARRAY];
};

struct PF_PHYSICAL_MEMORY_RANGE_INFO_V2 {
  ULONG Version;
  ULONG Flags;
  ULONG RangeCount;
  PF_PHYSICAL_MEMORY_RANGE Ranges[ANYSIZE_ARRAY];
};

enum SUPERFETCH_INFORMATION_CLASS {
  SuperfetchRetrieveTrace = 1,
  SuperfetchSystemParameters,
  SuperfetchLogEvent,
  SuperfetchGenerateTrace,
  SuperfetchPrefetch,
  SuperfetchPfnQuery,
  SuperfetchPfnSetPriority,
  SuperfetchPrivSourceQuery,
  SuperfetchSequenceNumberQuery,
  SuperfetchScenarioPhase,
  SuperfetchWorkerPriority,
  SuperfetchScenarioQuery,
  SuperfetchScenarioPrefetch,
  SuperfetchRobustnessControl,
  SuperfetchTimeControl,
  SuperfetchMemoryListQuery,
  SuperfetchMemoryRangesQuery,
  SuperfetchTracingControl,
  SuperfetchTrimWhileAgingControl,
  SuperfetchRepurposedByPrefetch,
  SuperfetchChannelPowerRequest,
  SuperfetchMovePages,
  SuperfetchVirtualQuery,
  SuperfetchCombineStatsQuery,
  SuperfetchSetMinWsAgeRate,
  SuperfetchDeprioritizeOldPagesInWs,
  SuperfetchFileExtentsQuery,
  SuperfetchGpuUtilizationQuery,
  SuperfetchInformationMax
};

constexpr ULONG SUPERFETCH_INFORMATION_VERSION = 0x2D;
constexpr ULONG SUPERFETCH_INFORMATION_MAGIC   = 0x6B756843;

struct SUPERFETCH_INFORMATION {
  ULONG Version;
  ULONG Magic;
  SUPERFETCH_INFORMATION_CLASS SuperfetchInformationClass;
  PVOID SuperfetchInformation;
  ULONG SuperfetchInformationLength;
};
using PSUPERFETCH_INFORMATION = SUPERFETCH_INFORMATION*;

extern "C" {
  NTSYSAPI
  NTSTATUS
  NTAPI
  RtlAdjustPrivilege(
    _In_  ULONG Privilege,
    _In_  BOOLEAN Enable,
    _In_  BOOLEAN CurrentThread,
    _Out_ PBOOLEAN Enabled
  );

  NTSYSAPI
  VOID
  NTAPI
  RtlTimeToElapsedTimeFields(
    _In_  PLARGE_INTEGER Time,
    _Out_ PTIME_FIELDS TimeFields
  );
}

void SiInitData(PSUPERFETCH_INFORMATION inf, PVOID buf, ULONG len, SUPERFETCH_INFORMATION_CLASS sic) {
  inf->Version = SUPERFETCH_INFORMATION_VERSION;
  inf->Magic   = SUPERFETCH_INFORMATION_MAGIC;
  inf->SuperfetchInformationClass  = sic;
  inf->SuperfetchInformation       = buf;
  inf->SuperfetchInformationLength = len;
}

#endif
