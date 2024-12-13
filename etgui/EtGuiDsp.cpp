#include "EtGuiDsp.h"
#include "EtDspFft.h"
#include "EtDspNoise.h"
#include <imgui.h>
#include <implot.h>
#include <string>
#include <vector>

namespace EtGui {
namespace DspGui {

// TimeFreqPlot
// ----------------------------------------------------------------------------
class TimeFreqHistPlot {
public:
  std::vector<float> x;
  std::vector<float> y;
  bool fitFlag{};

  void fit() { fitFlag = true; }

  void show() {
    ImGui::PushID(this);
    if (ImPlot::BeginPlot("Time domain")) {
      if (fitFlag) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, x.size() - 1, ImPlotCond_Always);
      }
      ImPlot::PlotStairs("##", x.data(), x.size());
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("Histogram")) {
      ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_AutoFit);
      ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
      ImPlot::PlotHistogram("##", x.data(), x.size(), 100);
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("Magnitude Spectrum")) {
      ImPlot::PlotStems("##", y.data(), x.size() / 2);
      ImPlot::EndPlot();
    }
    if (fitFlag) {
      fitFlag = {};
    }
    ImGui::PopID();
  }
};

class BufferSizeSpin {
public:
  std::string label{"Buffer Size"};
  int value{10};

  int bufferSize() const { return 1 << value; }

  bool show() {
    ImGui::PushID(this);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("2^");
    ImGui::SameLine(0.f, 0.f);
    ImGui::SetNextItemWidth(100.f);
    const bool changed = ImGui::InputInt(label.c_str(), &value, 1, 0);
    ImGui::SameLine(0.f, 0.f);
    ImGui::Text("=%d", bufferSize());
    ImGui::PopID();
    return changed;
  }
};

// Demos
// ----------------------------------------------------------------------------
class IShowable {
public:
  virtual ~IShowable() {}
  virtual bool show() = 0;
};

class MainDemo : public IShowable {
public:
  bool show() override final { return {}; }
};

class WhiteNoiseDemo : public IShowable {
public:
  TimeFreqHistPlot plt;
  BufferSizeSpin bufSzSpin;
  EtDsp::WhiteNoiseGen gen;

  WhiteNoiseDemo() { generate(); }

  void generate() {
    plt.x.resize(bufSzSpin.bufferSize());
    for (int i{}; i < bufSzSpin.bufferSize(); ++i) {
      plt.x[i] = gen();
    }
    plt.y = plt.x;
    plt.y.resize(2 * plt.y.size());
    EtDsp::Fft::AbsFft<1024>(plt.y.data());
  }

  bool show() override final {
    ImGui::SeparatorText("White Noise Generator");
    if (ImGui::Button("Generate")) {
      generate();
      plt.fit();
    }
    ImGui::SameLine();
    bufSzSpin.show();
    plt.show();
    return {};
  }
};

// DemoMenu
// ----------------------------------------------------------------------------
class DemoMenu {
public:
  enum Item {
    Item_Main = 0,
    Item_WhiteNoiseGen,
    Item_PinkNoiseGen,
    Item_SineGen,
  };

  Item curr{};

  bool show() {
    ImGui::PushID(this);
    const int prev = curr;
    if (ImGui::Selectable("Main", curr == Item_Main)) {
      curr = Item_Main;
    }
    if (ImGui::CollapsingHeader("Noise Generator")) {
      if (ImGui::Selectable("White Noise", curr == Item_WhiteNoiseGen)) {
        curr = Item_WhiteNoiseGen;
      }
      if (ImGui::Selectable("Pink Noise", curr == Item_PinkNoiseGen)) {
        curr = Item_PinkNoiseGen;
      }
    }
    if (ImGui::CollapsingHeader("Oscillator")) {
      if (ImGui::Selectable("Sine", curr == Item_SineGen)) {
        curr = Item_SineGen;
      }
    }
    ImGui::PopID();
    return prev != curr;
  }
};

// DemoWidget
// ----------------------------------------------------------------------------
class DemoWidget::Impl {
public:
  DemoMenu menu;
  std::unique_ptr<IShowable> shobl;

  void setMenuShowable();
};

DemoWidget::DemoWidget() : impl_{std::make_unique<Impl>()} {
  impl_->setMenuShowable();
}

DemoWidget::~DemoWidget() {}

void DemoWidget::show() {
  ImGui::PushID(this);
  // Menu
  ImGui::BeginChild("##Menu", ImVec2(250.f, 0.f), ImGuiChildFlags_Borders);
  if (impl_->menu.show()) {
    impl_->setMenuShowable();
  }
  ImGui::EndChild();

  // Demo
  ImGui::SameLine({}, 5.f);
  ImGui::BeginChild("##Demo", {}, ImGuiChildFlags_Borders);
  impl_->shobl->show();
  ImGui::EndChild();
  ImGui::PopID();
}

void DemoWidget::Impl::setMenuShowable() {
  if (menu.curr == DemoMenu::Item_Main) {
    shobl = std::make_unique<MainDemo>();
  } else if (menu.curr == DemoMenu::Item_WhiteNoiseGen) {
    shobl = std::make_unique<WhiteNoiseDemo>();
  } else if (menu.curr == DemoMenu::Item_PinkNoiseGen) {
  }
}

} // namespace DspGui
} // namespace EtGui
