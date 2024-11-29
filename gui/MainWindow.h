#pragma once
#include <memory>

namespace Gui {
class MainWindow {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  MainWindow();
  ~MainWindow();

private:
  void onShowMenus();
  void onShowToolbar();
  void onShowStatusbar();
};
} // namespace Gui
