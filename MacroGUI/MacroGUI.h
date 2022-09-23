#pragma once
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cfgmgr32.h>
#include <Fileapi.h>
#include <thread>
#include <mutex>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include <ntddkbd.h>

#include "public.h"
#include "MacroCommands.h"
#include "MacroInterface.h"






class MacroGuiWindowSettings 
{
	public:
		static bool UnsavedChanges;
		static bool DebugWindowOpen;

		static bool DuplicateMacro;
		static DuplicateMacroEnum _resolveDuplicate;

		static MacroCommands CurrMacroCommands;

		static MacroCommand LastPacket;
		static mutex _lastPacketLock;

		static void SaveChanges() 
		{
			CurrMacroCommands.WriteCommands();
			UnsavedChanges = false;
		}

		
};

void HandlePopups();
