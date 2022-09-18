#pragma once

#include <initguid.h>

#include <ntddk.h>
#include <wdf.h>
#include <wdmsec.h>
#include <kbdmou.h>
#include <ntdd8042.h>
#include "../MacroGUI/public.h"
#include "keyboardInterface.h"

#define KEY_MAP_SIZE 256
#define MACRO_KEY_INVALID 255

typedef struct _DEVICE_EXTENSION {
	PVOID OUTPUT_BUFFER;
	ULONG OutputBufferLength;

	CONNECT_DATA KeyboardConnectData;

	PI8042_KEYBOARD_INITIALIZATION_ROUTINE UpperInitializationRoutine;
	PI8042_KEYBOARD_ISR UpperIsrHook;
	PVOID UpperContext;

	KEYBOARD_ATTRIBUTES KeyboardAttributes;

	WDFSPINLOCK KeypressSpinLock;

	//A request that can be completed when input is available for the user app to read
	WDFREQUEST UserAppCallbackRequest;

	int KEYS_DOWN;
	UCHAR KEY_MAP[KEY_MAP_SIZE];

	WDFDEVICE KeyboardInterface;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, GetDeviceExtension)

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD EvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_DESTROY EvtDeviceContextDestroy;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtIoDeviceControl;

VOID KeyboardMacroServiceCallback(
	IN PDEVICE_OBJECT DeviceObject,
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd,
	IN OUT PULONG InputDataConsumed
);

VOID KeyboardMacroDispatchPassThrough(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target
);

BOOLEAN KeyboardMacro_IsrHook(
	_In_ PVOID                   IsrContext,
	_In_ PKEYBOARD_INPUT_DATA    CurrentInput,
	_In_ POUTPUT_PACKET          CurrentOutput,
	_In_ UCHAR                   StatusByte,
	_In_ PUCHAR                  Byte,
	_Out_ PBOOLEAN                ContinueProcessing,
	_In_ PKEYBOARD_SCAN_STATE    ScanState
);

NTSTATUS KeyboardMacro_InitializationRoutine(
	IN PVOID                           InitializationContext,
	IN PVOID                           SynchFuncContext,
	IN PI8042_SYNCH_READ_PORT          ReadPort,
	IN PI8042_SYNCH_WRITE_PORT         WritePort,
	OUT PBOOLEAN                       TurnTranslationOn
);

VOID KeyboardMacroRequestCompletionRoutine(
	WDFREQUEST                  Request,
	WDFIOTARGET                 Target,
	PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
	WDFCONTEXT                  Context
);

VOID KeyboardMacroEvtDestroy(
	_In_ WDFOBJECT Object
);

VOID PrepareKeyMapArray(UCHAR* keyMap);
VOID InitializeKeyData(PDEVICE_EXTENSION deviceExtension);