#pragma once

#include <array>
#include <cstring>

#include "log.h"
#include "i2c.h"

class TouchWheel {
public:
  TouchWheel(I2cBus& i2c_bus) : device_(i2c_bus, 0x54) {}

  uint8_t GetRaw() {
    device_.Write({0x00});
    return device_.ReadByte();
  }
private:
   I2cDevice device_;
};