#include "MiQdspGui.h"
#include "QdspNoiseGen.h"
#include <imgui.h>
#include <implot.h>
#include <vector>

namespace Mi {
namespace QdspGui {

// TimeFreqPlot
// ----------------------------------------------------------------------------
class TimeFreqPlot {
public:
  std::vector<float> time;
  std::vector<float> freq;

  void show() {
    ImGui::PushID(this);
    if (ImPlot::BeginPlot("##time")) {
      ImPlot::PlotStairs("##", time.data(), time.size());
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##hist")) {
      ImPlot::PlotHistogram("##", time.data(), time.size());
      ImPlot::EndPlot();
    }
    ImGui::PopID();
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
  TimeFreqPlot plt;
  Qdsp::WhiteNoiseGen gen;
  int nsamples{1024};

  WhiteNoiseDemo() { generate(); }

  void generate() {
    plt.time.resize(nsamples);
    for (int i{}; i < nsamples; ++i) {
      plt.time[i] = gen();
    }
  }

  bool show() override final {
    ImGui::SeparatorText("White Noise Generator");
    ImGui::InputInt("Samples", &nsamples);
    ImGui::SameLine();
    if (ImGui::Button("Generate")) {
      generate();
    }
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

} // namespace QdspGui

// namespace QDsp
} // namespace Mi
