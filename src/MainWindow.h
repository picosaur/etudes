#pragma once
#include <memory>

class MainWindow {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  MainWindow();
  ~MainWindow();

  void show();

private:
  void onShowMenus();
  void onShowToolbar();
  void onShowStatusbar();
};
