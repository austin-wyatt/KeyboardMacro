#pragma once
#include <string>
#include <fstream>
#include <ntddkbd.h>
#include <imgui.h>

#define CONFIG_FILE_PATH L".\\commands.conf"

#define MAKE_CODE_STRING_MIN_LENGTH 32

using namespace std;


class MakeCode 
{
	public:
		int Code;
		int Flag;

		static void GetStringFromMakeCode(int makeCode, USHORT flag, char* strBuf)
		{
#pragma warning(disable:4996)
			switch (makeCode)
			{
			case 0x0:
				strcpy(strBuf, "Error");
			case 0x1:
				strcpy(strBuf, "Escape");
			case 0x2:
				strcpy(strBuf, "1");
				break;
			case 0x3:
				strcpy(strBuf, "2");
				break;
			case 0x4:
				strcpy(strBuf, "3");
				break;
			case 0x5:
				strcpy(strBuf, "4");
				break;
			case 0x6:
				strcpy(strBuf, "5");
				break;
			case 0x7:
				strcpy(strBuf, "6");
				break;
			case 0x8:
				strcpy(strBuf, "7");
				break;
			case 0x9:
				strcpy(strBuf, "8");
				break;
			case 0xa:
				strcpy(strBuf, "9");
				break;
			case 0xb:
				strcpy(strBuf, "0");
				break;
			case 0xc:
				strcpy(strBuf, "-");
				break;
			case 0xd:
				strcpy(strBuf, "=");
				break;
			case 0xe:
				strcpy(strBuf, "Backspace");
				break;
			case 0xf:
				strcpy(strBuf, "Tab");
				break;
			case 0x10:
				strcpy(strBuf, "Q");
				break;
			case 0x11:
				strcpy(strBuf, "W");
				break;
			case 0x12:
				strcpy(strBuf, "E");
				break;
			case 0x13:
				strcpy(strBuf, "R");
				break;
			case 0x14:
				strcpy(strBuf, "T");
				break;
			case 0x15:
				strcpy(strBuf, "Y");
				break;
			case 0x16:
				strcpy(strBuf, "U");
				break;
			case 0x17:
				strcpy(strBuf, "I");
				break;
			case 0x18:
				strcpy(strBuf, "O");
				break;
			case 0x19:
				strcpy(strBuf, "P");
				break;
			case 0x1a:
				strcpy(strBuf, "[");
				break;
			case 0x1b:
				strcpy(strBuf, "]");
				break;
			case 0x1c:
				strcpy(strBuf, "Enter");
				break;
			case 0x1d:
				strcpy(strBuf, "LCtrl");
				break;
			case 0x1e:
				strcpy(strBuf, "A");
				break;
			case 0x1f:
				strcpy(strBuf, "S");
				break;
			case 0x20:
				strcpy(strBuf, "D");
				break;
			case 0x21:
				strcpy(strBuf, "F");
				break;
			case 0x22:
				strcpy(strBuf, "G");
				break;
			case 0x23:
				strcpy(strBuf, "H");
				break;
			case 0x24:
				strcpy(strBuf, "J");
				break;
			case 0x25:
				strcpy(strBuf, "K");
				break;
			case 0x26:
				strcpy(strBuf, "L");
				break;
			case 0x27:
				strcpy(strBuf, ";");
				break;
			case 0x28:
				strcpy(strBuf, "'");
				break;
			case 0x29:
				strcpy(strBuf, "Tilde");
				break;
			case 0x2a:
				strcpy(strBuf, "LShift");
				break;
			case 0x2b:
				strcpy(strBuf, "\\");
				break;
			case 0x2c:
				strcpy(strBuf, "Z");
				break;
			case 0x2d:
				strcpy(strBuf, "X");
				break;
			case 0x2e:
				strcpy(strBuf, "C");
				break;
			case 0x2f:
				strcpy(strBuf, "V");
				break;
			case 0x30:
				strcpy(strBuf, "B");
				break;
			case 0x31:
				strcpy(strBuf, "N");
				break;
			case 0x32:
				strcpy(strBuf, "M");
				break;
			case 0x33:
				strcpy(strBuf, ",");
				break;
			case 0x34:
				strcpy(strBuf, ".");
				break;
			case 0x35:
				strcpy(strBuf, "/");
				break;
			case 0x36:
				strcpy(strBuf, "RShift");
				break;
			case 0x37:
				strcpy(strBuf, "Num *");
				break;
			case 0x38:
				strcpy(strBuf, "LAlt");
				break;
			case 0x39:
				strcpy(strBuf, "Spacebar");
				break;
			case 0x3a:
				strcpy(strBuf, "Caps");
				break;
			case 0x3b:
				strcpy(strBuf, "F1");
				break;
			case 0x3c:
				strcpy(strBuf, "F2");
				break;
			case 0x3d:
				strcpy(strBuf, "F3");
				break;
			case 0x3e:
				strcpy(strBuf, "F4");
				break;
			case 0x3f:
				strcpy(strBuf, "F5");
				break;
			case 0x40:
				strcpy(strBuf, "F6");
				break;
			case 0x41:
				strcpy(strBuf, "F7");
				break;
			case 0x42:
				strcpy(strBuf, "F8");
				break;
			case 0x43:
				strcpy(strBuf, "F9");
				break;
			case 0x44:
				strcpy(strBuf, "F10");
				break;
			case 0x45:
				strcpy(strBuf, "Num Lock");
				break;
			case 0x46:
				strcpy(strBuf, "Scroll Lock");
				break;
			case 0x47:
				if (flag & KEY_E0)
					strcpy(strBuf, "Home");
				else
					strcpy(strBuf, "Num 7");
				break;
			case 0x48:
				if (flag & KEY_E0)
					strcpy(strBuf, "Up Arrow");
				else
					strcpy(strBuf, "Num 8");
				break;
			case 0x49:
				if (flag & KEY_E0)
					strcpy(strBuf, "Page Up");
				else
					strcpy(strBuf, "Num 9");
				break;
			case 0x4a:
				strcpy(strBuf, "Num -");
				break;
			case 0x4b:
				if (flag & KEY_E0)
					strcpy(strBuf, "Left Arrow");
				else
					strcpy(strBuf, "Num 4");
				break;
			case 0x4c:
				strcpy(strBuf, "Num 5");
				break;
			case 0x4d:
				if (flag & KEY_E0)
					strcpy(strBuf, "Right Arrow");
				else
					strcpy(strBuf, "Num 6");
				break;
			case 0x4e:
				strcpy(strBuf, "Num +");
				break;
			case 0x4f:
				if (flag & KEY_E0)
					strcpy(strBuf, "End");
				else
					strcpy(strBuf, "Num 1");
				break;
			case 0x50:
				if (flag & KEY_E0)
					strcpy(strBuf, "Down Arrow");
				else
					strcpy(strBuf, "Num 2");
				break;
			case 0x51:
				if (flag & KEY_E0)
					strcpy(strBuf, "Page Down");
				else
					strcpy(strBuf, "Num 3");
				break;
			case 0x52:
				if (flag & KEY_E0)
					strcpy(strBuf, "Insert");
				else
					strcpy(strBuf, "Num 0");
				break;
			case 0x53:
				if (flag & KEY_E0)
					strcpy(strBuf, "Delete");
				else
					strcpy(strBuf, "Num .");
				break;
			case 0x57:
				strcpy(strBuf, "F11");
				break;
			case 0x58:
				strcpy(strBuf, "F12");
				break;
			default:
				sprintf(strBuf, "0x%x", makeCode);
			}
#pragma warning(default:4996)
		}
};

class MacroCommand 
{
	public:
		std::vector<MakeCode> MakeCodes;
		ImVector<char> Command;

		std::string CodeString;

		int CommandLineHeight = 1;
		bool ChangingMacro = false;

		string BuildCodeString() 
		{
			return BuildCodeString(this);
		}

		void CalculateLineHeight() 
		{
			this->CommandLineHeight = 1;
			for (int j = 0; j < this->Command.Size; j++)
			{
				if (this->Command[j] == '\n')
					this->CommandLineHeight++;
			}
		}

		static string BuildCodeString(MacroCommand* command)
		{
			string str;

			int len = (int)command->MakeCodes.size();

			char charArr[MAKE_CODE_STRING_MIN_LENGTH];
			ZeroMemory(charArr, MAKE_CODE_STRING_MIN_LENGTH);

			for (int i = 0; i < len; i++)
			{
				MakeCode::GetStringFromMakeCode(command->MakeCodes[i].Code, command->MakeCodes[i].Flag, charArr);
				str.append(charArr);
				
				if (i != len - 1) 
				{
					str.append(" + ");
				}
			}

			return str;
		}
};

class MacroCommands 
{
	public:
		std::vector<MacroCommand> CommandList;

		void LoadCommands();

		void WriteCommands();

		void CreateCommandsTable();
	
		void ChangeMacro(MacroCommand* macroToChange = nullptr)
		{
			if (_macroBeingChanged != nullptr) 
			{
				_macroBeingChanged->ChangingMacro = false;
			}

			if (macroToChange != nullptr) 
			{
				macroToChange->ChangingMacro = true;
			}

			_macroBeingChanged = macroToChange;
		}

		MacroCommand* GetMacroBeingChanged() 
		{
			return _macroBeingChanged;
		}

		void RemoveMacro(MacroCommand* macroToRemove) 
		{
			int i;
			for (i = 0; i < this->CommandList.size(); i++)
			{
				if (&this->CommandList[i] == macroToRemove)
				{
					break;
				}
			}

			if (i != this->CommandList.size())
			{
				this->CommandList.erase(this->CommandList.begin() + i);
			}
		}

	private:
		MacroCommand* _macroBeingChanged = nullptr;

};


struct ImGuiExtensions
{
	static int StringResizeCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			ImVector<char>* my_str = (ImVector<char>*)data->UserData;
			IM_ASSERT(my_str->begin() == data->Buf);
			//my_str->reserve(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
			my_str->resize(data->BufTextLen + 1);
			data->Buf = my_str->begin();
		}
		return 0;
	}

	static bool MyInputTextMultiline(const char* label, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, StringResizeCallback, (void*)my_str);
	}

	static bool MyInputText(const char* label, ImVector<char>* my_str, ImGuiInputTextFlags flags = 0)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		return ImGui::InputText(label, my_str->begin(), (size_t)my_str->size(), flags | ImGuiInputTextFlags_CallbackResize, StringResizeCallback, (void*)my_str);
	}
};