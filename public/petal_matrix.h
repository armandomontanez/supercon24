#pragma once

#include <array>
#include <cstring>

#include "log.h"
#include "hardware/i2c.h"

class I2cDevice {
 public:
  I2cDevice(i2c_inst_t* i2c_bus) : i2c_bus_handle_(i2c_bus) {}

  template<size_t kSize>
  void Write(uint8_t address, uint8_t const (&command)[kSize]) {
    i2c_write_blocking(i2c_bus_handle_, address, command, kSize, false);
  }

 private:
  i2c_inst_t* i2c_bus_handle_;
};

class PetalMatrix {
 public:
  PetalMatrix() : i2c_bus_(i2c0), state_({}) {}

  void Init() {
    constexpr size_t kSdaPin = 0;
    constexpr size_t kSclPin = 1;
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(kSdaPin, GPIO_FUNC_I2C);
    gpio_set_function(kSclPin, GPIO_FUNC_I2C);
    gpio_pull_up(kSdaPin);
    gpio_pull_up(kSclPin);

    // Restart command.
    i2c_bus_.Write(kI2cAddr, {ShutdownRegister::Address(), ShutdownRegister::NormalOperationCmd(/*reset_to_default=*/true)});

    // No decode.
    i2c_bus_.Write(kI2cAddr, {DecodeEnableRegister::Address(), DecodeEnableRegister::NoDecodeCmd()});

    // Enable all characters/channels.
    i2c_bus_.Write(kI2cAddr, {0x0b, 0x07});

    // Intensity.
    i2c_bus_.Write(kI2cAddr, {0x0A, 0x07});

    // Start with all LEDs off.
    for (size_t i = 0; i < 8; i++) {
      i2c_bus_.Write(kI2cAddr, {DigitRegister::Address(i), 0});
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

    i2c_bus_.Write(kI2cAddr, {DigitRegister::Address(arm), state_[arm]});
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
  I2cDevice i2c_bus_;
  std::array<uint8_t, 8> state_;
};
