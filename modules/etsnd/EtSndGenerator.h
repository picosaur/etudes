#pragma once
#include <memory>

namespace EtSnd {

class Generator {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  Generator();
  ~Generator();

  void setSampleRate(int sampleRate);
  void setFrequency(int frequency);
};

} // namespace EtSnd
