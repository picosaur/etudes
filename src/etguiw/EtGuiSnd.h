#pragma once
#include <memory>
#include <string>
#include <vector>

namespace EtSnd {
class DriverInfo;
class DeviceInfo;
class DeviceList;
class DriverList;
} // namespace EtSnd

namespace EtGuiw {
namespace SndGui {

// DriverCombo
// ----------------------------------------------------------------------------
class DriverCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DriverCombo(const std::string &label = "Driver");
  ~DriverCombo();

  bool show();
  void setList(const EtSnd::DriverList &list);
  void setCurr(const std::string &name);
  bool hasCurr() const;
  const EtSnd::DriverInfo &curr() const;
};

// DeviceCombo
// ----------------------------------------------------------------------------
class DeviceCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DeviceCombo(const std::string &label = "Device");
  ~DeviceCombo();

  bool show();
  void setList(const EtSnd::DeviceList &list);
  void setCurr(const std::string &name);
  bool hasCurr() const;
  const EtSnd::DeviceInfo &curr() const;
};

// SampleRateCombo
// ----------------------------------------------------------------------------
class SampleRateCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  SampleRateCombo(const std::string &label = "Sample Rate",
                  const std::vector<int> &list = {8000, 11025, 16000, 22050,
                                                  44100, 48000, 88200, 96000,
                                                  176400, 192000},
                  int curr = 48000);
  ~SampleRateCombo();

  bool show();
};

// BufferSizeCombo
// ----------------------------------------------------------------------------
class BufferSizeCombo {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  BufferSizeCombo(const std::string &label = "Buffer Size",
                  const std::vector<int> &list = {32, 64, 128, 256, 512, 1024,
                                                  2048, 4096},
                  int curr = 128);
  ~BufferSizeCombo();

  bool show();
};

// ChannelSpin
// ----------------------------------------------------------------------------
class ChannelsSpin {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  ChannelsSpin(const std::string &label = "Channels", int min = 1,
               int max = 64);
  ~ChannelsSpin();

  bool show();
};

// DeviceChooserWidget
// ----------------------------------------------------------------------------
class DeviceChooserWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DeviceChooserWidget();
  ~DeviceChooserWidget();

  void show();
};

// StreamSetupWidget
// ----------------------------------------------------------------------------
class StreamSetupWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  StreamSetupWidget();
  ~StreamSetupWidget();

  void show();
};

// DemoWidget
// ----------------------------------------------------------------------------
class DemoWidget {
  class Impl;
  std::unique_ptr<Impl> impl_;

public:
  DemoWidget();
  ~DemoWidget();

  void show();
};

} // namespace SndGui
} // namespace EtGuiw
