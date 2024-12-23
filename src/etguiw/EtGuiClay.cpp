#define CLAY_IMPLEMENTATION
#include "EtGuiClay.h"
#include <clay.h>
#include <functional>
#include <imgui.h>

// See
// https://github.com/nicbarker/clay/pull/69/commits/b5b47b2486dc21b860f59edbbdee3440012aa1c4

namespace EtGuiw {
namespace ClayGui {

void ImClayInit();
void ImClayRender(const std::function<Clay_RenderCommandArray()> &layoutFunc);
void ImClayRenderCommands(Clay_RenderCommandArray renderCommands);
void ImClayRenderRectangle(Clay_RenderCommand *renderCommand);
void ImClayRenderText(Clay_RenderCommand *renderCommand);

class DemoWidget::Impl {
public:
};

DemoWidget::DemoWidget() : impl_{std::make_unique<Impl>()} {}

DemoWidget::~DemoWidget() {}

void DemoWidget::show() {

  auto layout = []() {
    const Clay_Color COLOR_ORANGE = (Clay_Color){225, 138, 50, 255};
    const Clay_Color COLOR_BLUE = (Clay_Color){111, 173, 162, 255};
    const Clay_Color COLOR_LIGHT = (Clay_Color){224, 215, 210, 255};
    Clay_BeginLayout();
    CLAY(CLAY_ID("MainContent"),
         CLAY_LAYOUT({.sizing =
                          {
                              .width = CLAY_SIZING_GROW(),
                              .height = CLAY_SIZING_GROW(),
                          },
                      .childAlignment =
                          {
                              .x = CLAY_ALIGN_X_CENTER,
                              .y = CLAY_ALIGN_Y_CENTER,
                          }}),
         CLAY_RECTANGLE({
             .color = COLOR_LIGHT,
         })) {
      // Label(CLAY_STRING("Hello, World!"));
    }
    return Clay_EndLayout();
  };

  ImClayInit();
  ImClayRender(layout);
}

void ImClayInit() {
  static bool clayInit{};
  if (!clayInit) {
    const auto winSize = ImGui::GetWindowSize();
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena){
        .label = CLAY_STRING("Clay Memory Arena"),
        .capacity = totalMemorySize,
        .memory = new char[totalMemorySize],
    };
    Clay_Initialize(clayMemory, (Clay_Dimensions){winSize.x, winSize.y});
    clayInit = true;
  }
}

void ImClayRender(const std::function<Clay_RenderCommandArray()> &layoutFunc) {
  const auto winSize = ImGui::GetWindowSize();
  Clay_SetLayoutDimensions((Clay_Dimensions){winSize.x, winSize.y});
  Clay_RenderCommandArray renderCommands = layoutFunc();
  ImClayRenderCommands(renderCommands);
}

void ImClayRenderCommands(Clay_RenderCommandArray renderCommands) {
  for (uint32_t i = 0; i < renderCommands.length; i++) {
    Clay_RenderCommand *renderCommand =
        Clay_RenderCommandArray_Get(&renderCommands, i);
    if (renderCommand->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE) {
      ImClayRenderRectangle(renderCommand);
    } else if (renderCommand->commandType == CLAY_RENDER_COMMAND_TYPE_TEXT) {
      ImClayRenderText(renderCommand);
    }
  }
}

void ImClayRenderRectangle(Clay_RenderCommand *renderCommand) {
  Clay_BoundingBox boundingBox = renderCommand->boundingBox;
  Clay_RectangleElementConfig *config =
      renderCommand->config.rectangleElementConfig;
  Clay_Color color = config->color;

  ImDrawList *dl = ImGui::GetWindowDrawList();
  float xmin = boundingBox.x;
  float ymin = boundingBox.y;
  float xmax = xmin + boundingBox.width;
  float ymax = ymin + boundingBox.height;
  dl->AddRectFilled(ImVec2{xmin, ymin}, ImVec2{xmax, ymax},
                    ImColor(color.r, color.g, color.b, color.a));
}

void ImClayRenderText(Clay_RenderCommand *renderCommand) {
  Clay_BoundingBox boundingBox = renderCommand->boundingBox;
  Clay_TextElementConfig *config = renderCommand->config.textElementConfig;
  Clay_String text = renderCommand->text;
}

} // namespace ClayGui
} // namespace EtGuiw
