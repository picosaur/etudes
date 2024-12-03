#include "AudeGenerator.h"

namespace Aude {

class Generator::Impl {
public:
  int sampleRate{48000};
  int carrierFreq{1000};
};

Generator::Generator() {}

Generator::~Generator() {}

void Generator::setSampleRate(int sampleRate) {}

void Generator::setFrequency(int frequency) {}

} // namespace Aude
