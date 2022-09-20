#pragma once

typedef enum _INTERFACE_READ_MODE {
	MACRO_KeyBuffer = 0,
	MACRO_KeyboardDevices = 1
} INTERFACE_READ_MODE;


typedef struct _KEYBOARD_CONTEXT {
	WDFDEVICE KeyboardDevices[KEYBOARD_DEVICES_MAX_LENGTH];
	int CurrentDevices;

	WDFDEVICE RegisteredKeyboard;

	WDFREQUEST OrphanedCallback;

	INTERFACE_READ_MODE READ_MODE;
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
void KeyboardInterfaceRegisterDevice(WDFDEVICE keyboardInterface, WDFOBJECT object);

void KeyboardInterfaceUnregisterDevice(WDFDEVICE keyboardInterface);

void MacroRequestCancelCallback(WDFDEVICE keyboardInterface, WDFOBJECT device, ULONG reponse);
