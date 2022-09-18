#pragma once

#define DOS_DEVICE_NAME L"\\DosDevices\\KeyboardMacroInterface"
#define DOS_DEVICE_PATH L"\\\\.\\KeyboardMacroInterface"

#define OUTPUT_BUFFER_SIZE 10
#define OUTPUT_BUFFER_INDEX_SIZE sizeof(OUTPUT_BUFFER_STRUCT)
#define OUTPUT_BUFFER_SIZE_BYTES OUTPUT_BUFFER_INDEX_SIZE * OUTPUT_BUFFER_SIZE //TODO
#define OUTPUT_BUFFER_STRUCT KEYBOARD_INPUT_DATA


typedef struct _MACRO_REQUEST
{
	int Code;
	int Data;
} MACRO_REQUEST, *PMACRO_REQUEST;

//The next read request will return a list of int IDs terminated with -1
#define MACRO_REQUEST_GET_KEYBOARDS 0x1 
//pass the ID of the keyboard to register in the Data field
#define MACRO_REQUEST_REGISTER_KEYBOARD 0x2 
#define MACRO_REQUEST_CANCEL_CALLBACK 0x3

//the driver will hold this request until data becomes available
//when completed, the next read will be the available data
#define MACRO_REQUEST_SET_CALLBACK 0x123456 

#define MACRO_RESPONSE_EMPTY 0x0
#define MACRO_RESPONSE_KEY_DATA_AVAILABLE 0x1
#define MACRO_RESPONSE_FAILURE 0x2
#define MACRO_RESPONSE_NO_REGISTERED_KEYBOARD 0x3
#define MACRO_RESPONSE_CALLBACK_CANCELED 0x4


DEFINE_GUID(
	KEYBOARD_MACRO_GUID,
	0xC4833AD3, 0xEDC7, 0x4320, 0x8E, 0x5E, 0xCC, 0xD2, 0x6A, 0x6F, 0x06, 0x8A);