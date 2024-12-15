#pragma ocne
#include <cstdint>

namespace EtDsp {

// WhiteNoiseGen
// https://www.musicdsp.org/en/latest/Synthesis/216-fast-whitenoise-generator.html
class WhiteNoiseGen {
public:
  static constexpr float scale = 2.0f / float(0xffffffff);
  std::uint32_t x1 = 0x67452301;
  std::uint32_t x2 = 0xefcdab89;
  float s = 0.0f;

  float operator()() {
    x1 ^= x2;
    s = x2 * scale;
    x2 += x1;
    return s;
  }
};

// PinkNoiseGen
// https://www.musicdsp.org/en/latest/Filters/76-pink-noise-filter.html
class PinkNoiseGen : public WhiteNoiseGen {
public:
  static constexpr float scale = 0.2f;
  static constexpr float c1 = 0.99765f;
  static constexpr float c2 = 0.0990460f * scale;
  static constexpr float c3 = 0.96300f;
  static constexpr float c4 = 0.2965164f * scale;
  static constexpr float c5 = 0.57000f;
  static constexpr float c6 = 1.0526913f * scale;
  static constexpr float c7 = 0.1848f * scale;

  float b0 = 0.0f;
  float b1 = 0.0f;
  float b2 = 0.0f;

  float operator()() {
    auto white = WhiteNoiseGen::operator()();
    b0 = c1 * b0 + white * c2;
    b1 = c3 * b1 + white * c4;
    b2 = c5 * b2 + white * c6;
    return b0 + b1 + b2 + white * c7;
  }
};
} // namespace EtDsp
