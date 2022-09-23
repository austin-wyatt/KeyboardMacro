#pragma once

enum class DuplicateMacroEnum
{
	Default,
	//Stop assigning the macro
	Cancel,
	//Allow multiple macros to contain the same key combination
	Ignore,
};

enum class InternalMacroReadCode
{
	KeyData,
	KeyboardDevices
};

class MacroInterface
{
	public:
		static bool MainCallbackActive;

		static void KeyboardPacketRecieved(OUTPUT_BUFFER_STRUCT* buffer, int length);
		static void WriteToDevice(HANDLE deviceHandle, int code, long long data);
		static void ReadFromDevice(HANDLE deviceHandle, InternalMacroReadCode readCode);
		static HANDLE CreateDeviceHandle();

		static void BeginMainCallback(HANDLE callbackHandle);
		static void RunCommand(char* command);
};

