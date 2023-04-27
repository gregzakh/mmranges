#pragma once

#ifndef _QNTRTL_HPP_
#define _QNTRTL_HPP_

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

#endif
