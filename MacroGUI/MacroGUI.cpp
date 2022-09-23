#include "MacroGUI.h"


using namespace std;

#define MAX_DEVPATH_LENGTH 512

vector<HANDLE> handleList;

bool MacroGuiWindowSettings::UnsavedChanges = false;
bool MacroGuiWindowSettings::DebugWindowOpen = false;
bool MacroGuiWindowSettings::DuplicateMacro = false;
DuplicateMacroEnum MacroGuiWindowSettings::_resolveDuplicate = DuplicateMacroEnum::Default;
MacroCommands MacroGuiWindowSettings::CurrMacroCommands = MacroCommands();
MacroCommand MacroGuiWindowSettings::LastPacket = MacroCommand();
mutex MacroGuiWindowSettings::_lastPacketLock;




int main()
{
    MacroCommands* commands = &MacroGuiWindowSettings::CurrMacroCommands;
    commands->LoadCommands();


    HANDLE callbackHandle = MacroInterface::CreateDeviceHandle();
    HANDLE shortTermHandle = MacroInterface::CreateDeviceHandle();

    if (callbackHandle != INVALID_HANDLE_VALUE && callbackHandle != NULL) 
    {
        MacroInterface::BeginMainCallback(callbackHandle);
    }

    handleList.push_back(callbackHandle);
    handleList.push_back(shortTermHandle);

    //Initialize glfw
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Keyboard Macro Tool", NULL, NULL);
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

    //glfwSetKeyCallback(window, MacroGLFWKeyCallback);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        bool my_tool_active = true;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (MacroGuiWindowSettings::UnsavedChanges) window_flags |= ImGuiWindowFlags_UnsavedDocument;

        ImGui::Begin("Keyboard Macro", &my_tool_active, window_flags);

        if (MacroGuiWindowSettings::UnsavedChanges && ImGui::IsKeyPressed(ImGuiKey_S, false) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
        {
            MacroGuiWindowSettings::SaveChanges();
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Reload macros"))
                {
                    commands->CommandList.clear();
                    commands->LoadCommands();
                }
                if (ImGui::MenuItem("Save", "CTRL+S", false, MacroGuiWindowSettings::UnsavedChanges))
                { 
                    MacroGuiWindowSettings::SaveChanges();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Debug"))
            {
                if (ImGui::MenuItem("Open debug window"))
                {
                    MacroGuiWindowSettings::DebugWindowOpen = true;
                }
                else if (ImGui::MenuItem("Send callback"))
                {
                    if (!MacroInterface::MainCallbackActive)
                    {
                        std::cout << "Send callback" << endl;

                        MacroInterface::BeginMainCallback(callbackHandle);
                    }
                }
                else if (ImGui::MenuItem("Stop reading inputs")) 
                {
                    thread endCallbackThread(MacroInterface::WriteToDevice, shortTermHandle, MACRO_REQUEST_CANCEL_CALLBACK, 0);

                    endCallbackThread.join();
                }
                else if (ImGui::MenuItem("Get devices"))
                {
                    thread devicesThread(MacroInterface::WriteToDevice, shortTermHandle, MACRO_REQUEST_GET_KEYBOARDS, 0);

                    devicesThread.join();
                }
                else if (ImGui::MenuItem("Unregister device"))
                {
                    thread devicesThread(MacroInterface::WriteToDevice, shortTermHandle, MACRO_REQUEST_UNREGISTER_KEYBOARD, 0);

                    devicesThread.join();
                }
                else if (ImGui::MenuItem("Duplicate test"))
                {
                    MacroGuiWindowSettings::DuplicateMacro = true;
                }


                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        if (ImGui::CollapsingHeader("Commands", ImGuiTreeNodeFlags_DefaultOpen))
        {
            commands->CreateCommandsTable();
        }
        

        ImGui::Separator();

        /*bool demoWindowOpen = true;
        ImGui::ShowDemoWindow(&demoWindowOpen);*/

        ImGui::End();

        ImGui::PopStyleVar(1);

        HandlePopups();

        if (MacroGuiWindowSettings::DebugWindowOpen)
        {
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;

            ImGui::SetNextWindowPos(ImVec2(0.0f, displaySize.y - 100));
            ImGui::SetNextWindowSize(ImVec2(displaySize.x, 100));

            //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

            window_flags = ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

            ImGui::Begin("Debug Window", &MacroGuiWindowSettings::DebugWindowOpen, window_flags);

            ImGui::Text("Connected to driver: %s", MacroInterface::MainCallbackActive ? "true" : "false");

            ImGui::SameLine(0, 100);

            MacroGuiWindowSettings::_lastPacketLock.lock();
            ImGui::Text("Last packet: %s", MacroGuiWindowSettings::LastPacket.CodeString.c_str());
            MacroGuiWindowSettings::_lastPacketLock.unlock();

            ImGui::End();
        }

        

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

    thread endThread(MacroInterface::WriteToDevice, shortTermHandle, MACRO_REQUEST_CANCEL_CALLBACK, 0);
    std::cout << "closing handles" << endl;
    for (int i = 0; i < handleList.size(); i++)
    {
        CloseHandle(handleList[i]);
    }

    return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    main();
}

void HandlePopups() 
{

    if(MacroGuiWindowSettings::DuplicateMacro)
    {
        ImGui::OpenPopup("Duplicate Key Combination");
    }
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Duplicate Key Combination", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) 
    {
        ImGui::Text("This key combination is already in use by another macro.\n\n");
        ImGui::Separator();

        auto create = []()
        {
            MacroGuiWindowSettings::DuplicateMacro = false;
            MacroGuiWindowSettings::_resolveDuplicate = DuplicateMacroEnum::Ignore;
            ImGui::CloseCurrentPopup();
        };

        auto cancel = []()
        {
            MacroGuiWindowSettings::DuplicateMacro = false;
            MacroGuiWindowSettings::_resolveDuplicate = DuplicateMacroEnum::Cancel;
            ImGui::CloseCurrentPopup();
        };

        if (ImGui::Button("Cancel"))
        {
            cancel();
        }

        ImGui::SameLine();

        if (ImGui::Button("Create anyway"))
        {
            create();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) 
        {
            cancel();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_Space))
        {
            create();
        }

        ImGui::EndPopup();
    }
}