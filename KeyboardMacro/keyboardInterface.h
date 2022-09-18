#pragma once

#define KEYBOARD_DEVICES_MAX_LENGTH 10

typedef struct _KEYBOARD_CONTEXT {
	WDFDEVICE KeyboardDevices[KEYBOARD_DEVICES_MAX_LENGTH];
	int CurrentDevices;

	int RegisteredKeyboard;
} KEYBOARD_CONTEXT, *PKEYBOARD_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(KEYBOARD_CONTEXT, GetKeyboardExtension)


WDFDEVICE CreateKeyboardInterface(WDFDRIVER Driver);

VOID KeyboardInterfaceEvtIoRead(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request,
	_In_ size_t Length
);

VOID KeyboardInterfaceEvtIoWrite(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request,
	_In_ size_t Length
);

void KeyboardInterfaceRemoveDevice(WDFDEVICE keyboardInterface, WDFOBJECT object);