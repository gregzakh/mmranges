#pragma once

#ifndef _QREKEY_HPP_
#define _QREKEY_HPP_

#pragma pack(push, 4)
struct CM_PARTIAL_RESOURCE_DESCRIPTOR {
  UCHAR  Type;
  UCHAR  ShareDisposition;
  USHORT Flags;
  union {
    struct {
      LARGE_INTEGER Start;
      ULONG Length;
    } Generic;
    struct {
      LARGE_INTEGER Start;
      ULONG Length;
    } Port;
    struct {
#if defined(NT_PROCESSOR_GROUPS)
      USHORT Level;
      USHORT Group;
#else
      ULONG Level;
#endif
      ULONG Vector;
      KAFFINITY Affinity;
    } Interrupt;
    struct {
      union {
        struct {
#if defined(NT_PROCESSOR_GROUPS)
          USHORT Group;
#else
          USHORT Reserved;
#endif
          USHORT MessageCount;
          ULONG  Vector;
          KAFFINITY Affinity;
        } Raw;
        struct {
#if defined(NT_PROCESSOR_GROUPS)
          USHORT Level;
          USHORT Group;
#else
          ULONG Level;
#endif
          ULONG Vector;
          KAFFINITY Affinity;
        } Translated;
      } mi;
    } MessageInterrupt;
    struct {
      LARGE_INTEGER Start;
      ULONG Length;
    } Memory;
    struct {
      ULONG Channel;
      ULONG Port;
      ULONG Reserved1;
    } Dma;
    struct {
      ULONG Channel;
      ULONG RequestLine;
      UCHAR TransferWidth;
      UCHAR Reserved1;
      UCHAR Reserved2;
      UCHAR Reserved3;
    } DmaV3;
    struct {
      ULONG Data[3];
    } DevicePrivate;
    struct {
      ULONG Start;
      ULONG Length;
      ULONG Reserved;
    } BusNumber;
    struct {
      ULONG DataSize;
      ULONG Reserved1;
      ULONG Reserved2;
    } DeviceSpecificData;
    struct {
      LARGE_INTEGER Start;
      ULONG Length40;
    } Memory40;
    struct {
      LARGE_INTEGER Start;
      ULONG Length48;
    } Memory48;
    struct {
      LARGE_INTEGER Start;
      ULONG Length64;
    } Memory64;
    struct {
      UCHAR Class;
      UCHAR Type;
      UCHAR Reserved1;
      UCHAR Reserved2;
      ULONG IdLowPart;
      ULONG IdHighPart;
    } Connection;
  } u;
};
#pragma pack(pop, 4)

struct CM_PARTIAL_RESOURCE_LIST {
  USHORT Version;
  USHORT Revision;
  ULONG  Count;
  CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[ANYSIZE_ARRAY];
};

enum INTERFACE_TYPE {
   InterfaceTypeUndefined = -1, Internal = 0, Isa, Eisa,
   MicroChannel, TurboChannel, PCIBus, VMEBus, NuBus,
   PCMCIABus, CBus, MPIBus, MPSABus, ProcessorInternal,
   InternalPowerBus, PNPISABus, PNPBus, Vmcs, ACPIBus,
   MaximumInterfaceType
};

struct CM_FULL_RESOURCE_DESCRIPTOR {
  INTERFACE_TYPE InteractiveType;
  ULONG BusNumber;
  CM_PARTIAL_RESOURCE_LIST PartialResourceList;
};

struct CM_RESOURCE_LIST {
  ULONG Count;
  CM_FULL_RESOURCE_DESCRIPTOR List[ANYSIZE_ARRAY];
};
using PCM_RESOURCE_LIST = CM_RESOURCE_LIST*;

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

template<typename... ArgTypes>
auto queryvalue(HKEY key, PWSTR val, ArgTypes... args) {
  return ::RegQueryValueEx(key, val, nullptr, nullptr, args...);
}

#endif
