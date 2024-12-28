#include <thread>
#include <cstdio>
#include <Windows.h>
#include "MinecraftChatManager.h"

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    static FILE* consoleFile = nullptr;
    static std::thread mainThread;

    switch (reason) {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen_s(&consoleFile, "CONOUT$", "w", stdout);

        mainThread = std::thread([instance] { MinecraftChatManager::MainThread(instance); });
        if (mainThread.joinable()) {
            mainThread.detach();
        }
        break;

    case DLL_PROCESS_DETACH:
        if (consoleFile) {
            fclose(consoleFile);
        }
        FreeConsole();
        break;
    }

    return true;
}
