#include "MacroGUI.h"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "GLFW/glfw3.h"
#include <ntddkbd.h>

using namespace std;

#define MAX_DEVPATH_LENGTH 512

vector<HANDLE> handleList;

bool callbackActive = false;

int main()
{
    HANDLE callbackHandle = CreateDeviceHandle();
    HANDLE shortTermHandle = CreateDeviceHandle();

    handleList.push_back(callbackHandle);
    handleList.push_back(shortTermHandle);

    //Initialize glfw
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(600, 400, "Keyboard Macro", NULL, NULL);
    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);


    //Initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    // Build atlas
    /*unsigned char* tex_pixels = NULL;
    int tex_w, tex_h;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);*/

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool my_tool_active = true;

        ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_NoTitleBar);
        /*if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }*/

        if (ImGui::Button("Send callback"))
        {
            if (!callbackActive) 
            {
                cout << "Send callback" << endl;

                callbackActive = true;
                thread(WriteToDevice, callbackHandle, MACRO_REQUEST_SET_CALLBACK, 0).detach();
            }
        }

        if (ImGui::Button("Stop reading inputs"))
        {
            thread endCallbackThread(WriteToDevice, shortTermHandle, MACRO_REQUEST_CANCEL_CALLBACK, 0);

            

            endCallbackThread.join();
        }

        if (ImGui::Button("Get devices"))
        {
            thread devicesThread(WriteToDevice, shortTermHandle, MACRO_REQUEST_GET_KEYBOARDS, 0);

            devicesThread.join();
        }

        if (ImGui::Button("Unregister device"))
        {
            thread devicesThread(WriteToDevice, shortTermHandle, MACRO_REQUEST_UNREGISTER_KEYBOARD, 0);

            devicesThread.join();
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    thread endThread(WriteToDevice, shortTermHandle, MACRO_REQUEST_CANCEL_CALLBACK, 0);
    cout << "closing handles" << endl;
    for (int i = 0; i < handleList.size(); i++)
    {
        CloseHandle(handleList[i]);
    }

    return 0;
}


void WriteToDevice(HANDLE deviceHandle, int code, long long data)
{
    BOOL operationSuccess;

    MACRO_REQUEST macroRequest;
    macroRequest.Code = code;
    macroRequest.Data = data;

    DWORD macroResponse;

    operationSuccess = WriteFile(deviceHandle, &macroRequest, sizeof(MACRO_REQUEST), &macroResponse, NULL);

    cout << "Macro response: " << hex << macroResponse << dec << endl << "Operation success: " << (operationSuccess ? "true" : "false") << endl;

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
                callbackActive = false;
                break;
            }

            WriteToDevice(deviceHandle, code, data);
            break;
    }
}

void ReadFromDevice(HANDLE deviceHandle, InternalMacroReadCode readCode)
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
                for (int i = 0; i < macroResponse / sizeof(OUTPUT_BUFFER_STRUCT); i++)
                {
                    cout << "Key code: " << hex << inputBuffer[i].MakeCode << dec << endl;
                    cout << "Flags: " << hex << inputBuffer[i].Flags << dec << endl;
                }
            }
            break;
        case InternalMacroReadCode::KeyboardDevices:

            long long deviceBuffer[KEYBOARD_DEVICES_MAX_LENGTH + 1];

            operationSuccess = ReadFile(deviceHandle, deviceBuffer, sizeof(HANDLE) * (KEYBOARD_DEVICES_MAX_LENGTH + 1), &macroResponse, NULL);
            if (operationSuccess)
            {
                for (int i = 0; i < macroResponse / sizeof(HANDLE); i++)
                {
                    cout << "Device: " << hex << (unsigned long long)deviceBuffer[i] << dec << endl;
                }
            }

            break;
    }
}

HANDLE CreateDeviceHandle() 
{
    HANDLE handle = CreateFile(DOS_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (handle == INVALID_HANDLE_VALUE)
    {
        cerr << "Handle error: " << GetLastError() << endl;
        return NULL;
    }

    return handle;
}