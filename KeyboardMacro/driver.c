
#include "driver.h"

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	NTSTATUS status;

	WDFDRIVER hDriver;
	WDF_DRIVER_CONFIG driverConfig;

	WDF_DRIVER_CONFIG_INIT(&driverConfig, EvtDeviceAdd);

	status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &driverConfig, &hDriver);

	if(!NT_SUCCESS(status))
	{
		DbgPrint("KeyboardMacroDriver failed in DriverEntry");
	}

	return status;
}



NTSTATUS EvtDeviceAdd(
	_In_ WDFDRIVER Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
{
	NTSTATUS status;
	WDFDEVICE hDevice;
	WDF_IO_QUEUE_CONFIG ioQueueConfig;
	PDEVICE_EXTENSION deviceExtension;
	WDF_OBJECT_ATTRIBUTES deviceAttributes;

	UNREFERENCED_PARAMETER(Driver);

	PAGED_CODE();

	WdfFdoInitSetFilter(DeviceInit);

	WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_KEYBOARD);

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_EXTENSION);
	
	status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &hDevice);

	if(!NT_SUCCESS(status))
	{
		DbgPrint("KeyboardMacroDriver device creation failed");
		return status;
	}

	deviceExtension = GetDeviceExtension(hDevice);
	status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, PagedPool, 'Mdbk', OUTPUT_BUFFER_SIZE_BYTES, &deviceExtension->OUTPUT_BUFFER, NULL);

	if(!NT_SUCCESS(status))
	{
		DbgPrint("KeyboardMacroDriver OUTPUT_BUFFER memory allocation failed");
		return status;
	}

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchParallel);

	ioQueueConfig.EvtIoInternalDeviceControl = EvtIoDeviceControl;

	status = WdfIoQueueCreate(hDevice, &ioQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

	if(!NT_SUCCESS(status))
	{
		DbgPrint("KeyboardMacroDriver IO queue creation failed");
		return status;
	}

	return status;
}

void EvtIoDeviceControl(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request,
	_In_ size_t OutputBufferLength,
	_In_ size_t InputBufferLength,
	_In_ ULONG IoControlCode)
{
	WDFDEVICE hDevice;
	PDEVICE_EXTENSION deviceExtension;
	PCONNECT_DATA connectData;
	size_t length;
	NTSTATUS status = STATUS_SUCCESS;

	UNREFERENCED_PARAMETER(OutputBufferLength);
	UNREFERENCED_PARAMETER(InputBufferLength);

	PAGED_CODE();

	KdBreakPoint();

	hDevice = WdfIoQueueGetDevice(Queue);
	deviceExtension = GetDeviceExtension(hDevice);

	switch (IoControlCode) 
	{
		case IOCTL_INTERNAL_KEYBOARD_CONNECT:

			//Disallow multiple keyboards from being connected to the same port
			if(deviceExtension->KeyboardConnectData.ClassService != NULL)
			{
				status = STATUS_SHARING_VIOLATION;
				break;
			}

			status = WdfRequestRetrieveInputBuffer(Request, sizeof(CONNECT_DATA), &connectData, &length);

			if(!NT_SUCCESS(status))
			{
				break;
			}

			//Save the original connect data before overwriting it
			deviceExtension->KeyboardConnectData = *connectData;

			connectData->ClassDeviceObject = WdfDeviceWdmGetDeviceObject(hDevice);

#pragma warning(disable:4152)
			connectData->ClassService = KeyboardMacroServiceCallback;
#pragma warning(default:4152)

			break;
		case IOCTL_INTERNAL_KEYBOARD_DISCONNECT:
			deviceExtension->KeyboardConnectData.ClassDeviceObject = NULL;
			deviceExtension->KeyboardConnectData.ClassService = NULL;
			break;
	}

	if(!NT_SUCCESS(status))
	{
		WdfRequestComplete(Request, status);
		return;
	}

	KeyboardMacroDispatchPassThrough(Request, WdfDeviceGetIoTarget(hDevice));
}

VOID KeyboardMacroServiceCallback(
	IN PDEVICE_OBJECT DeviceObject,
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd,
	IN OUT PULONG InputDataConsumed
)
{
	KdBreakPoint();

	PDEVICE_EXTENSION deviceExtension;
	WDFDEVICE hDevice;

	hDevice = WdfWdmDeviceGetWdfDeviceHandle(DeviceObject);

	deviceExtension = GetDeviceExtension(hDevice);
	
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,"Service callback: %hu", InputDataStart->MakeCode));

	//This calls the original connect data's callback
	(*(PSERVICE_CALLBACK_ROUTINE)deviceExtension->KeyboardConnectData.ClassService)(
		deviceExtension->KeyboardConnectData.ClassDeviceObject,
		InputDataStart,
		InputDataEnd,
		InputDataConsumed
		);
}

VOID KeyboardMacroDispatchPassThrough(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target
)
{
	WDF_REQUEST_SEND_OPTIONS options;
	NTSTATUS status = STATUS_SUCCESS;
	BOOLEAN completed;

	WDF_REQUEST_SEND_OPTIONS_INIT(&options, WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

	completed = WdfRequestSend(Request, Target, &options);

	//KdBreakPoint();

	if (completed == FALSE) 
	{
		status = WdfRequestGetStatus(Request);
		DbgPrint("KeyboardMacroDriver dispatch pass through failed");
		WdfRequestComplete(Request, status);
	}
}
