#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <cfgmgr32.h>
#include <Fileapi.h>
#include <thread>

#include <initguid.h>
#include "public.h"

enum class InternalMacroReadCode
{
	KeyData,
	KeyboardDevices
};

void WriteToDevice(HANDLE deviceHandle, int code, long long data);
void ReadFromDevice(HANDLE deviceHandle, InternalMacroReadCode readCode);

HANDLE CreateDeviceHandle();