#include "MinecraftChatManager.h"
#include <cstdio>
#include <iostream>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

const char* MinecraftChatManager::MinecraftClassName = "ave";
const char* MinecraftChatManager::MinecraftStaticFieldName = "S";
const char* MinecraftChatManager::MinecraftStaticFieldSignature = "Lave;";
const char* MinecraftChatManager::IngameGUIFieldName = "q";
const char* MinecraftChatManager::IngameGUISignature = "Lavo;";
const char* MinecraftChatManager::PersistantChatGUIFieldName = "l";
const char* MinecraftChatManager::PersistantChatGUISignature = "Lavt;";
const char* MinecraftChatManager::GuiNewChatMethodName = "a";
const char* MinecraftChatManager::GuiNewChatMethodSignature = "(Leu;I)V";
const char* MinecraftChatManager::ChatComponentTextClassName = "fa";
const char* MinecraftChatManager::ChatComponentTextConstructorSignature = "(Ljava/lang/String;)V";

std::string MinecraftChatManager::transformToMinecraftColorCodes(const std::string& input) {
    std::string output = input;
    for (size_t pos = 0; (pos = output.find('&', pos)) != std::string::npos; ++pos) {
        output.replace(pos, 1, "§");
    }
    return output;
}

void MinecraftChatManager::MainThread(HMODULE module) {
    JavaVM* javaVM = nullptr;
    JNIEnv* env = nullptr;

    if (JNI_GetCreatedJavaVMs(&javaVM, 1, nullptr) != JNI_OK || !javaVM ||
        javaVM->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
        printf("Error with the JVM\n");
        return;
    }

    if (!glfwInit()) {
        printf("Error with GLFW\n");
        return;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Minecraft Chat Faker", nullptr, nullptr);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool showGui = true;
    char inputBuffer[256] = "";

    while (!glfwWindowShouldClose(window) && showGui) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Chat Faker", &showGui);

        ImGui::Text("Enter a message to send: ");
        ImGui::InputText("Text", inputBuffer, sizeof(inputBuffer));

        if (ImGui::Button("Send")) {
            std::string transformedMessage = transformToMinecraftColorCodes(inputBuffer);

            jclass minecraftClass = env->FindClass(MinecraftClassName);
            jfieldID minecraftStaticField = env->GetStaticFieldID(minecraftClass, MinecraftStaticFieldName, MinecraftStaticFieldSignature);
            jobject minecraftInstance = env->GetStaticObjectField(minecraftClass, minecraftStaticField);

            jfieldID ingameGUIField = env->GetFieldID(minecraftClass, IngameGUIFieldName, IngameGUISignature);
            jobject ingameGUIInstance = env->GetObjectField(minecraftInstance, ingameGUIField);

            jclass guiIngameClass = env->GetObjectClass(ingameGUIInstance);
            jfieldID persistantChatGUIField = env->GetFieldID(guiIngameClass, PersistantChatGUIFieldName, PersistantChatGUISignature);
            jobject persistantChatGUIInstance = env->GetObjectField(ingameGUIInstance, persistantChatGUIField);

            jclass guiNewChatClass = env->GetObjectClass(persistantChatGUIInstance);
            jmethodID printChatMessageMethod = env->GetMethodID(guiNewChatClass, GuiNewChatMethodName, GuiNewChatMethodSignature);

            jclass chatComponentTextClass = env->FindClass(ChatComponentTextClassName);
            jmethodID chatComponentTextConstructor = env->GetMethodID(chatComponentTextClass, "<init>", ChatComponentTextConstructorSignature);

            jstring chatMessage = env->NewStringUTF(transformedMessage.c_str());
            jobject chatComponentInstance = env->NewObject(chatComponentTextClass, chatComponentTextConstructor, chatMessage);

            env->CallVoidMethod(persistantChatGUIInstance, printChatMessageMethod, chatComponentInstance, 1);

            env->DeleteLocalRef(chatMessage);
            printf("Sended: %s\n", transformedMessage.c_str());
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    javaVM->DetachCurrentThread();
    FreeLibrary(module);
}
