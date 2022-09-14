#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <kbdmou.h>


#define OUTPUT_BUFFER_SIZE 10
#define OUTPUT_BUFFER_SIZE_BYTES sizeof(KEYBOARD_INPUT_DATA) * OUTPUT_BUFFER_SIZE

typedef struct _DEVICE_EXTENSION {
	WDFMEMORY OUTPUT_BUFFER;
	ULONG OutputBufferLength;

	CONNECT_DATA KeyboardConnectData;
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
