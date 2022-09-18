#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <cfgmgr32.h>
#include <Fileapi.h>
#include <thread>

#include <initguid.h>
#include "public.h"

void WriteToDevice(HANDLE deviceHandle, int code, int data);
void ReadFromDevice(HANDLE deviceHandle);

HANDLE CreateDeviceHandle();