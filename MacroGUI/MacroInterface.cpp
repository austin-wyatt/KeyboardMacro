#include "MacroGUI.h"

bool MacroInterface::MainCallbackActive = false;

void MacroInterface::KeyboardPacketRecieved(OUTPUT_BUFFER_STRUCT* buffer, int length) 
{
    MacroGuiWindowSettings::_lastPacketLock.lock();
    MacroGuiWindowSettings::LastPacket.MakeCodes.clear();

    for (int i = 0; i < length; i++)
    {
        MakeCode code = MakeCode();

        code.Code = (buffer + i)->MakeCode;
        code.Flag = (buffer + i)->Flags;
        cout << "Key: " << hex << code.Code << endl;
        cout << "Flags: " << hex << code.Flag << dec << endl;

        MacroGuiWindowSettings::LastPacket.MakeCodes.push_back(code);
    }

    MacroGuiWindowSettings::LastPacket.CodeString = MacroGuiWindowSettings::LastPacket.BuildCodeString();
    MacroGuiWindowSettings::LastPacket.CodeString.push_back('\0');

    MacroGuiWindowSettings::_lastPacketLock.unlock();

	MacroCommands* macroCommands = &MacroGuiWindowSettings::CurrMacroCommands;

	if (macroCommands->GetMacroBeingChanged() != nullptr)
	{
		MacroCommand* command = macroCommands->GetMacroBeingChanged();

		//check for duplicate make code
		for (int i = 0; i < macroCommands->CommandList.size(); i++) 
		{
			if (&macroCommands->CommandList[i] != command && macroCommands->CommandList[i].MakeCodes.size() == length)
			{
				bool dupe = true;

                //check for dupes
				for (int j = 0; j < length; j++) 
				{
					int k;
					for (k = 0; k < length; k++) 
					{
						if((buffer + j)->MakeCode == macroCommands->CommandList[i].MakeCodes[k].Code && 
							(buffer + j)->Flags == macroCommands->CommandList[i].MakeCodes[k].Flag)
						{
							break;
						}
					}

					if (k == length) 
					{
						dupe = false;
						break;
					}
				}

				if (dupe) 
				{
                    auto waitFunc = []()
                    {
                        while (MacroGuiWindowSettings::_resolveDuplicate == DuplicateMacroEnum::Default)
                        {
                            Sleep(5);
                        }
                    };

					MacroGuiWindowSettings::DuplicateMacro = true;

                    thread waitThread(waitFunc);
                    waitThread.join();

                    DuplicateMacroEnum val = MacroGuiWindowSettings::_resolveDuplicate;
                    MacroGuiWindowSettings::_resolveDuplicate = DuplicateMacroEnum::Default;

                    switch (val)
                    {
                        case DuplicateMacroEnum::Cancel:
                            return;
                    }

                    break;
				}
			}
		}

		//assign new make code
		
        cout << "Assigned macro" << endl;

		command->MakeCodes.clear();

		MakeCode code = MakeCode();
		for (int i = 0; i < length; i++) 
		{
			code.Code = (buffer + i)->MakeCode;
			code.Flag = (buffer + i)->Flags;

			command->MakeCodes.push_back(code);
		}

        command->CodeString = command->BuildCodeString();

        macroCommands->ChangeMacro();
	}
    else 
    {
        MacroCommand* command;

        for (int i = 0; i < macroCommands->CommandList.size(); i++) 
        {
            command = &macroCommands->CommandList[i];

            if (command->MakeCodes.size() == length) 
            {
                bool dupe = true;

                //check for dupes
                for (int j = 0; j < length; j++)
                {
                    int k;
                    for (k = 0; k < length; k++)
                    {
                        if ((buffer + j)->MakeCode == macroCommands->CommandList[i].MakeCodes[k].Code &&
                            (buffer + j)->Flags == macroCommands->CommandList[i].MakeCodes[k].Flag)
                        {
                            break;
                        }
                    }

                    if (k == length)
                    {
                        dupe = false;
                        break;
                    }
                }

                if (dupe) 
                {
                    cout << "Execute macro: " << command->Command.begin() << endl;
                    //thread(system, command->Command.begin()).detach();
                    //thread(WinExec, command->Command.begin(), SW_HIDE).detach();
                    thread(MacroInterface::RunCommand, command->Command.begin()).detach();
                }
            }
        }
    }
}

void MacroInterface::WriteToDevice(HANDLE deviceHandle, int code, long long data)
{
    BOOL operationSuccess;

    MACRO_REQUEST macroRequest;
    macroRequest.Code = code;
    macroRequest.Data = data;

    DWORD macroResponse;

    operationSuccess = WriteFile(deviceHandle, &macroRequest, sizeof(MACRO_REQUEST), &macroResponse, NULL);

    std::cout << "Macro response: " << hex << macroResponse << dec << endl << "Operation success: " << (operationSuccess ? "true" : "false") << endl;

    if (macroResponse & MACRO_RESPONSE_FLAG_AVAILABLE)
    {
        if (macroResponse & MACRO_RESPONSE_KEY_DATA)
        {
            ReadFromDevice(deviceHandle, InternalMacroReadCode::KeyData);
        }
        else if (macroResponse & MACRO_RESPONSE_KEYBOARD_DATA)
        {
            ReadFromDevice(deviceHandle, InternalMacroReadCode::KeyboardDevices);
        }
    }

    switch (code)
    {
    case MACRO_REQUEST_SET_CALLBACK:
        if (macroResponse & MACRO_RESPONSE_FLAG_NO_RENEW)
        {
            MacroInterface::MainCallbackActive = false;
            break;
        }

        WriteToDevice(deviceHandle, code, data);
        break;
    }
}

void MacroInterface::ReadFromDevice(HANDLE deviceHandle, InternalMacroReadCode readCode)
{
    BOOL operationSuccess;

    DWORD macroResponse;

    switch (readCode)
    {
    case InternalMacroReadCode::KeyData:
        OUTPUT_BUFFER_STRUCT inputBuffer[OUTPUT_BUFFER_SIZE];

        operationSuccess = ReadFile(deviceHandle, inputBuffer, OUTPUT_BUFFER_SIZE_BYTES, &macroResponse, NULL);

        if (operationSuccess)
        {
            MacroInterface::KeyboardPacketRecieved(inputBuffer, macroResponse / sizeof(OUTPUT_BUFFER_STRUCT));
            /*for (int i = 0; i < macroResponse / sizeof(OUTPUT_BUFFER_STRUCT); i++)
            {
                std::cout << "Key code: " << hex << inputBuffer[i].MakeCode << dec << endl;
                std::cout << "Flags: " << hex << inputBuffer[i].Flags << dec << endl;
            }*/
        }
        break;
    case InternalMacroReadCode::KeyboardDevices:

        long long deviceBuffer[KEYBOARD_DEVICES_MAX_LENGTH + 1];

        operationSuccess = ReadFile(deviceHandle, deviceBuffer, sizeof(HANDLE) * (KEYBOARD_DEVICES_MAX_LENGTH + 1), &macroResponse, NULL);
        if (operationSuccess)
        {
            for (int i = 0; i < macroResponse / sizeof(HANDLE); i++)
            {
                std::cout << "Device: " << hex << (unsigned long long)deviceBuffer[i] << dec << endl;
            }
        }

        break;
    }
}

HANDLE MacroInterface::CreateDeviceHandle()
{
    HANDLE handle = CreateFile(DOS_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        cerr << "Handle error: " << GetLastError() << endl;
        return NULL;
    }

    return handle;
}

void MacroInterface::BeginMainCallback(HANDLE callbackHandle)
{
    MacroInterface::MainCallbackActive = true;
    thread(MacroInterface::WriteToDevice, callbackHandle, MACRO_REQUEST_SET_CALLBACK, 0).detach();
}

void MacroInterface::RunCommand(char* command) 
{
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    CreateProcessA(
        NULL,
        command,
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}