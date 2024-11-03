#pragma once

#include <array>
#include <cstring>

#include "log.h"
#include "i2c.h"

class PetalMatrix {
 public:
  PetalMatrix(I2cBus& i2c_bus) : device_(i2c_bus, kI2cAddr), state_({}) {}

  void Init() {
    // Restart command.
    device_.Write({ShutdownRegister::Address(), ShutdownRegister::NormalOperationCmd(/*reset_to_default=*/true)});

    // No decode.
    device_.Write({DecodeEnableRegister::Address(), DecodeEnableRegister::NoDecodeCmd()});

    // Enable all characters/channels.
    device_.Write({0x0b, 0x07});

    // Intensity.
    device_.Write({0x0A, 0x07});

    Reset();
  }

  void Reset() {
    // Turn off all LEDs.
    for (size_t i = 0; i < 8; i++) {
      device_.Write({DigitRegister::Address(i), 0});
    }
  }

  void LedState(size_t arm, size_t index, bool enabled) {
    if (arm > 7) {
      LOG(ERROR, "Arm %d doesn't exist", int(arm));
      return;
    }
    if (index > 7) {
      LOG(ERROR, "Index %d doesn't exist", int(index));
      return;
    }
    if (enabled) {
      state_[arm] |= DigitRegister::SegmentCmd(index);
    } else {
      state_[arm] &= (~DigitRegister::SegmentCmd(index));
    }

    device_.Write({DigitRegister::Address(arm), state_[arm]});
  }

 private:
  class DigitRegister {
   public:
    static constexpr uint8_t Address(uint8_t index) {
      return (index+1);
    }
    static constexpr uint8_t SegmentCmd(uint8_t segment) {
      return 1 << segment;
    }
  };
  class DecodeEnableRegister {
   public:
    static constexpr uint8_t Address() { return 0x09; }
    static constexpr uint8_t NoDecodeCmd() { return 0x00; }
  };
  class ShutdownRegister {
   public:
    static constexpr uint8_t Address() { return 0x0c; }
    static constexpr uint8_t ShutdownCmd(bool reset_to_default = false) {
      if (reset_to_default) {
        return 0x00;
      }
      return 0x80;
    }
    static constexpr uint8_t NormalOperationCmd(bool reset_to_default = false) {
      if (reset_to_default) {
        return 0x01;
      }
      return 0x81;
    }
  };
  static constexpr uint8_t kShutdownRegister = 0x0c;
  static constexpr uint8_t kI2cAddr = 0x00;
  I2cDevice device_;
  std::array<uint8_t, 8> state_;
};
