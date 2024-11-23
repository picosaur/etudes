#include "MainWindow.h"
#include <hello_imgui/hello_imgui.h>
#include "FetchWidget.h"
#include "MiMapWidget.h"
#include "MiMapPlot.h"
#include <implot.h>

namespace Gui
{
    class MainWindow::Impl
    {
    public:
        Mi::FetchWidget fetchWidget;
        Mi::MapWidget mapWidget;
    };

    MainWindow::MainWindow() : impl_{std::make_unique<Impl>()}
    {
        HelloImGui::RunnerParams runnerParams;
        runnerParams.appWindowParams.windowTitle = "ImHello";
        runnerParams.appWindowParams.restorePreviousGeometry = true;
        runnerParams.appWindowParams.windowGeometry.windowSizeState = HelloImGui::WindowSizeState::Maximized;
        runnerParams.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
        runnerParams.imGuiWindowParams.showStatusBar = true;
        runnerParams.imGuiWindowParams.showMenuBar = true;

        runnerParams.dockingParams.dockingSplits = {
            {"MainDockSpace", "BotSpaceLeft", ImGuiDir_Down, 0.3},
            {"BotSpaceLeft", "BotSpaceRight", ImGuiDir_Right, 0.5},
        };

        runnerParams.dockingParams.dockableWindows = {
            {"FetchWidget", "MainDockSpace", [&]()
             {
                 impl_->fetchWidget.show();
             }},
            {"MapWidget", "MainDockSpace", [&]()
             {
                 impl_->mapWidget.show();
             }}};

        runnerParams.callbacks.ShowGui = [&]() {};

        HelloImGui::EdgeToolbarOptions toolbarOptions;
        toolbarOptions.sizeEm = 2.0f;
        toolbarOptions.WindowBg = {0.14f, 0.14f, 0.14f, 1.f};

        runnerParams.callbacks
            .AddEdgeToolbar(HelloImGui::EdgeToolbarType::Top, [&]
                            { onShowToolbar(); }, toolbarOptions);

        runnerParams.callbacks.LoadAdditionalFonts = [&]
        {
            HelloImGui::ImGuiDefaultSettings::LoadDefaultFont_WithFontAwesomeIcons();
        };

        runnerParams.callbacks.ShowMenus = [&]
        { onShowMenus(); };

        ImPlot::CreateContext();
        HelloImGui::Run(runnerParams);
        ImPlot::DestroyContext();
    }

    MainWindow::~MainWindow()
    {
    }

    void MainWindow::onShowMenus()
    {
    }

    void MainWindow::onShowToolbar()
    {
    }
}