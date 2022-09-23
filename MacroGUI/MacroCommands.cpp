#include "MacroGUI.h"

void MacroCommands::LoadCommands()
{
	fstream commandsFile;
	commandsFile.open(CONFIG_FILE_PATH, fstream::in | fstream::out);

	if (!commandsFile.is_open())
	{
		cerr << "Could not load config file" << endl;
		return;
	}

	MacroCommand currCommand;

	string placeholderStr;

	MakeCode currCode;

	string line;
	char currChar;
	int makeCodePlaceholder;
	int i;
	while (getline(commandsFile, line, '|'))
	{
		char lastChar = line[line.length() - 1];

		switch (lastChar) 
		{
			case ',':
				for (i = 0; i < line.length(); i++)
				{
					currChar = line.at(i);
					if(currChar == '*')
					{
						makeCodePlaceholder = stoi(placeholderStr, nullptr, 16);

						currCode.Code = makeCodePlaceholder;

						placeholderStr.clear();
						continue;
					}
					else if (currChar == ',') 
					{
						makeCodePlaceholder = stoi(placeholderStr, nullptr, 16);

						currCode.Flag = makeCodePlaceholder;
						currCommand.MakeCodes.push_back(currCode);

						placeholderStr.clear();
						currCode = MakeCode();
						continue;
					}

					placeholderStr.push_back(currChar);
				}
				break;
			case ';':
				currCommand.Command.reserve((int)line.size() - 1);

				memcpy(currCommand.Command.begin(), line.c_str(), line.size() - 1);
				currCommand.Command.Size = (int)line.size() - 1;
				currCommand.Command.push_back('\0');

				currCommand.CalculateLineHeight();

				currCommand.CodeString = currCommand.BuildCodeString();

				CommandList.push_back(currCommand);

				currCommand = MacroCommand();
				placeholderStr.clear();
				break;
		}
	}

	commandsFile.close();
}

void MacroCommands::WriteCommands() 
{
	fstream commandsFile;
	commandsFile.open(CONFIG_FILE_PATH, fstream::out);

	if (!commandsFile.is_open())
	{
		cerr << "Could not load config file" << endl;
		return;
	}

	for (int i = 0; i < this->CommandList.size(); i++) 
	{
		MacroCommand* macro = &this->CommandList[i];
		if (macro->MakeCodes.size() == 0)
			continue;

		const int MAX_SIZE = 256;

		char tempBuf[MAX_SIZE];
		int size = 0;

		for (int j = 0; j < macro->MakeCodes.size(); j++) 
		{
			size = sprintf_s(tempBuf, "0x%x*%d,|", macro->MakeCodes[j].Code, macro->MakeCodes[j].Flag);
			commandsFile.write(tempBuf, size);
		}

		commandsFile.write(macro->Command.begin(), macro->Command.Size - 1);

		size = sprintf_s(tempBuf, ";|\n");

		commandsFile.write(tempBuf, size);
	}

	commandsFile.close();
}



void MacroCommands::CreateCommandsTable() 
{
	if (ImGui::Button("Add New")) 
	{
		MacroCommand newCommand = MacroCommand();
		newCommand.CodeString.append("<empty>");
		newCommand.Command.push_back('\0');

		this->CommandList.push_back(newCommand);

		MacroGuiWindowSettings::UnsavedChanges = true;
	}

	ImGui::SameLine();
	ImGui::BeginDisabled(this->GetMacroBeingChanged() == nullptr);
	if (ImGui::Button("Delete"))
	{
		RemoveMacro(this->GetMacroBeingChanged());
		this->ChangeMacro();

		MacroGuiWindowSettings::UnsavedChanges = true;
	}
	ImGui::EndDisabled();

	if(ImGui::BeginTable("commandsTable", 2, 
		ImGuiTableFlags_Borders | 
		ImGuiTableFlags_RowBg | 
		ImGuiTableFlags_Resizable |
		ImGuiTableFlags_BordersV))
	{
		ImGui::TableSetupColumn("Macro");
		ImGui::TableSetupColumn("Command");
		ImGui::TableHeadersRow();

		for (int i = 0; i < this->CommandList.size(); i++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			if (ImGui::IsKeyPressed(ImGuiKey_Escape) && this->CommandList[i].ChangingMacro)
			{
				this->ChangeMacro();
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && this->CommandList[i].ChangingMacro)
			{
				this->ChangeMacro();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Delete) && this->CommandList[i].ChangingMacro)
			{
				RemoveMacro(this->GetMacroBeingChanged());
				this->ChangeMacro();

				MacroGuiWindowSettings::UnsavedChanges = true;
			}

			char buttonLabel[75];
			sprintf_s(buttonLabel, "%s##%d", this->CommandList[i].CodeString.c_str(), i);

			if (ImGui::Button(this->CommandList[i].ChangingMacro ? "{input new macro}" : buttonLabel))
			{
				this->ChangeMacro(&this->CommandList[i]);
				MacroGuiWindowSettings::UnsavedChanges = true;
			}

			if (ImGui::IsItemHovered() && this->CommandList[i].ChangingMacro)
				ImGui::SetTooltip("Right click to cancel");


			char label[12];
			sprintf_s(label, "##input%d", i);

			float rowHeight = ImGui::GetTextLineHeight() * (this->CommandList[i].CommandLineHeight == 1 ? 1.5f : 1.1f * this->CommandList[i].CommandLineHeight);

			ImGui::TableSetColumnIndex(1);
			if (ImGuiExtensions::MyInputTextMultiline(label, &this->CommandList[i].Command, ImVec2(-FLT_MIN, rowHeight))) 
			{
				MacroGuiWindowSettings::UnsavedChanges = true;
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) 
			{
				this->CommandList[i].CommandLineHeight = 1;
				for (int j = 0; j < this->CommandList[i].Command.Size; j++) 
				{
					if (this->CommandList[i].Command[j] == '\n')
						this->CommandList[i].CommandLineHeight++;
				}
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Right click to resize");
		}

		ImGui::EndTable();
	}
}