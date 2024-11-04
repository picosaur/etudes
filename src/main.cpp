#include "hello_imgui/hello_imgui.h"
int main(int , char *[]) {
    HelloImGui::Run(
        []{ ImGui::Text("Hello, world!"); }, // Gui code
        "Hello!", true);                     // Window title + Window size auto
}
