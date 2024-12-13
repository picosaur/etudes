#include "EtSndGenerator.h"

namespace EtSnd {

class Generator::Impl {
public:
  int sampleRate{48000};
  int carrierFreq{1000};
};

Generator::Generator() {}

Generator::~Generator() {}

void Generator::setSampleRate(int sampleRate) {}

void Generator::setFrequency(int frequency) {}

} // namespace EtSnd
