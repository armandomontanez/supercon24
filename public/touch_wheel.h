#pragma once

#include <array>
#include <cstring>

#include "log.h"
#include "i2c.h"

class TouchWheel {
public:
  TouchWheel(I2cBus& i2c_bus) : device_(i2c_bus, 0x54), last_update_us_(3000000), last_value_(0) {}

  uint8_t GetRaw() {
    // Rate limit sampling.
    uint64_t time = time_us_64();
    // Workaround: Wait for startup.
    if (time < last_update_us_) {
      return last_value_;
    }
    if (time - last_update_us_ < 10000) {
      return last_value_;
    }
    last_update_us_ = time;
    device_.Write({0x00});
    last_value_ = device_.ReadByte();
    return last_value_;
  }
private:
  I2cDevice device_;
  uint64_t last_update_us_;
  uint8_t last_value_;
};