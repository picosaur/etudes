#include <hello_imgui/hello_imgui.h>
#include <implot.h>

int main(int, char *[])
{
    ImPlot::CreateContext();
    HelloImGui::Run(
        [&] {
          ImGui::Text("Hello");
          if (ImPlot::BeginPlot("##plot")) {
            ImPlot::EndPlot();
          }
        },
        "Hello!", true);
    ImPlot::DestroyContext();
    return 0;
}
