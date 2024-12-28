#ifndef MINECRAFT_CHAT_MANAGER_H
#define MINECRAFT_CHAT_MANAGER_H

#include <string>
#include <Windows.h>
#include <jni.h>

class MinecraftChatManager {
public:
    static void MainThread(HMODULE module);

private:
    static const char* MinecraftClassName;
    static const char* MinecraftStaticFieldName;
    static const char* MinecraftStaticFieldSignature;
    static const char* IngameGUIFieldName;
    static const char* IngameGUISignature;
    static const char* PersistantChatGUIFieldName;
    static const char* PersistantChatGUISignature;
    static const char* GuiNewChatMethodName;
    static const char* GuiNewChatMethodSignature;
    static const char* ChatComponentTextClassName;
    static const char* ChatComponentTextConstructorSignature;

    static std::string transformToMinecraftColorCodes(const std::string& input);
};

#endif 
