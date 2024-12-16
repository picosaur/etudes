#include "MainWindow.h"
#include "backends/tex_inspect_opengl.h"
#include "etgui/EtGuiWaveWidget.h"
#include "etguiw/EtGuiDsp.h"
#include "etguiw/EtGuiFire.h"
#include "etguiw/EtGuiHttpWidget.h"
#include "etguiw/EtGuiSnd.h"
#include "etosm/EtOsmMapWidget.h"
#include "imgui_tex_inspect.h"
// #include <SDL2/SDL_timer.h>
#include <hello_imgui/hello_imgui.h>
#include <implot.h>

namespace HelloImGui {
std::string GlslVersion();
}

class MainWindow::Impl {
public:
  HelloImGui::RunnerParams params;
  EtGuiw::HttpWidget httpWidget;
  EtOsm::MapWidget mapWidget;
  EtGui::WaveWidget waveWidget;
  EtGuiw::DspGui::DemoWidget dspWidget;
  EtGuiw::SndGui::DemoWidget sndWidget;
  EtGuiw::FireGui::DemoWidget fireDemo;

  int httpTicks{};
  int mapTicks{};
  int waveTicks{};
};

MainWindow::MainWindow() : impl_{std::make_unique<Impl>()} {
  impl_->params.appWindowParams.windowTitle = "Etudes";

  impl_->params.appWindowParams.restorePreviousGeometry = true;

  impl_->params.appWindowParams.windowGeometry.windowSizeState =
      HelloImGui::WindowSizeState::Maximized;

  impl_->params.imGuiWindowParams.defaultImGuiWindowType =
      HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;

  impl_->params.imGuiWindowParams.showStatusBar = true;
  impl_->params.imGuiWindowParams.showMenuBar = true;

  impl_->params.dockingParams.dockingSplits = {
      {"MainDockSpace", "BotSpaceLeft", ImGuiDir_Down, 0.3},
      {"BotSpaceLeft", "BotSpaceRight", ImGuiDir_Right, 0.5},
  };

  impl_->params.dockingParams.dockableWindows = {
      {"HttpFetch", "MainDockSpace",
       [&]() {
         // auto tic = SDL_GetTicks();
         impl_->httpWidget.show();
         // auto toc = SDL_GetTicks();
         // impl_->httpTicks = toc - tic;
       }},

      {"Osm", "MainDockSpace",
       [&]() {
         // auto tic = SDL_GetTicks();
         impl_->mapWidget.show();
         // auto toc = SDL_GetTicks();
         // impl_->mapTicks = toc - tic;
       }},

      {"Waveform", "MainDockSpace",
       [&]() {
         // auto tic = SDL_GetTicks();
         impl_->waveWidget.show();
         // auto toc = SDL_GetTicks();
         // impl_->waveTicks = toc - tic;
       }},

      {"Dsp", "MainDockSpace", [&]() { impl_->dspWidget.show(); }},

      {"Snd", "MainDockSpace",
       [&]() {
         // auto tic = SDL_GetTicks();
         impl_->sndWidget.show();
         // auto toc = SDL_GetTicks();
       }},

      {"Fire", "MainDockSpace", [&]() {
         // auto tic = SDL_GetTicks();
         impl_->fireDemo.show();
         // auto toc = SDL_GetTicks();
       }}};

  impl_->params.callbacks.ShowGui = [&]() {};

  HelloImGui::EdgeToolbarOptions toolbarOptions;
  toolbarOptions.sizeEm = 2.0f;
  toolbarOptions.WindowBg = {0.14f, 0.14f, 0.14f, 1.f};

  impl_->params.callbacks.AddEdgeToolbar(
      HelloImGui::EdgeToolbarType::Top, [&] { onShowToolbar(); },
      toolbarOptions);

  impl_->params.callbacks.LoadAdditionalFonts = [&] {
    HelloImGui::ImGuiDefaultSettings::LoadDefaultFont_WithFontAwesomeIcons();
  };

  impl_->params.callbacks.ShowMenus = [&] { onShowMenus(); };

  impl_->params.callbacks.ShowStatus = [&] { onShowStatusbar(); };

  // ImGuiTexInspect
  {
    auto fn_ImGuiTextInspect_Init = [impl = impl_.get()]() {
      if (impl->params.rendererBackendType ==
          HelloImGui::RendererBackendType::OpenGL3) {
        ImGuiTexInspect::ImplOpenGL3_Init(HelloImGui::GlslVersion().c_str());
        ImGuiTexInspect::Init();
      }
    };
    impl_->params.callbacks.PostInit = HelloImGui::SequenceFunctions(
        fn_ImGuiTextInspect_Init, impl_->params.callbacks.PostInit);
  }
  {
    auto fn_ImGuiTextInspect_DeInit = []() {
      ImGuiTexInspect::Shutdown();
      ImGuiTexInspect::ImplOpenGl3_Shutdown();
    };
    impl_->params.callbacks.BeforeExit = HelloImGui::SequenceFunctions(
        fn_ImGuiTextInspect_DeInit, impl_->params.callbacks.BeforeExit);
  }
}

MainWindow::~MainWindow() {}

void MainWindow::show() {
  ImPlot::CreateContext();
  HelloImGui::Run(impl_->params);
  ImPlot::DestroyContext();
}

void MainWindow::onShowMenus() {}

void MainWindow::onShowToolbar() {}

void MainWindow::onShowStatusbar() {
  ImGui::Text("HttpFetch: %d, GeoMap: %d, Waveform: %d", impl_->httpTicks,
              impl_->mapTicks, impl_->waveTicks);
}
