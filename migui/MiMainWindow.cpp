#include "MiMainWindow.h"
#include "MiAudeGui.h"
#include "MiHttpWidget.h"
#include "MiMapWidget.h"
#include "MiWaveWidget.h"
#include <SDL2/SDL_timer.h>
#include <hello_imgui/hello_imgui.h>
#include <implot.h>

namespace Mi {
class MainWindow::Impl {
public:
  Mi::HttpWidget httpWidget;
  Mi::MapWidget mapWidget;
  Mi::WaveWidget waveWidget;
  Mi::AudeGui::DemoWidget audeWidget;

  int httpTicks{};
  int mapTicks{};
  int waveTicks{};
};

MainWindow::MainWindow() : impl_{std::make_unique<Impl>()} {
  HelloImGui::RunnerParams runnerParams;
  runnerParams.appWindowParams.windowTitle = "ImHello";
  runnerParams.appWindowParams.restorePreviousGeometry = true;
  runnerParams.appWindowParams.windowGeometry.windowSizeState =
      HelloImGui::WindowSizeState::Maximized;
  runnerParams.imGuiWindowParams.defaultImGuiWindowType =
      HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
  runnerParams.imGuiWindowParams.showStatusBar = true;
  runnerParams.imGuiWindowParams.showMenuBar = true;

  runnerParams.dockingParams.dockingSplits = {
      {"MainDockSpace", "BotSpaceLeft", ImGuiDir_Down, 0.3},
      {"BotSpaceLeft", "BotSpaceRight", ImGuiDir_Right, 0.5},
  };

  runnerParams.dockingParams.dockableWindows = {
      {"HttpFetch", "MainDockSpace",
       [&]() {
         auto tic = SDL_GetTicks();
         impl_->httpWidget.show();
         auto toc = SDL_GetTicks();
         impl_->httpTicks = toc - tic;
       }},

      {"GeoMap", "MainDockSpace",
       [&]() {
         auto tic = SDL_GetTicks();
         impl_->mapWidget.show();
         auto toc = SDL_GetTicks();
         impl_->mapTicks = toc - tic;
       }},

      {"Waveform", "MainDockSpace",
       [&]() {
         auto tic = SDL_GetTicks();
         impl_->waveWidget.show();
         auto toc = SDL_GetTicks();
         impl_->waveTicks = toc - tic;
       }},

      {"AudEngi", "MainDockSpace",
       [&]() {
         auto tic = SDL_GetTicks();
         impl_->audeWidget.show();
         auto toc = SDL_GetTicks();
       }},

  };

  runnerParams.callbacks.ShowGui = [&]() {};

  HelloImGui::EdgeToolbarOptions toolbarOptions;
  toolbarOptions.sizeEm = 2.0f;
  toolbarOptions.WindowBg = {0.14f, 0.14f, 0.14f, 1.f};

  runnerParams.callbacks.AddEdgeToolbar(
      HelloImGui::EdgeToolbarType::Top, [&] { onShowToolbar(); },
      toolbarOptions);

  runnerParams.callbacks.LoadAdditionalFonts = [&] {
    HelloImGui::ImGuiDefaultSettings::LoadDefaultFont_WithFontAwesomeIcons();
  };

  runnerParams.callbacks.ShowMenus = [&] { onShowMenus(); };

  runnerParams.callbacks.ShowStatus = [&] { onShowStatusbar(); };

  ImPlot::CreateContext();
  HelloImGui::Run(runnerParams);
  ImPlot::DestroyContext();
}

MainWindow::~MainWindow() {}

void MainWindow::onShowMenus() {}

void MainWindow::onShowToolbar() {}

void MainWindow::onShowStatusbar() {
  ImGui::Text("HttpFetch: %d, GeoMap: %d, Waveform: %d", impl_->httpTicks,
              impl_->mapTicks, impl_->waveTicks);
}
} // namespace Mi
