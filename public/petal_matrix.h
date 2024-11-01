#pragma once

#include "log.h"
#include "hardware/i2c.h"

class PetalMatrix {
 public:
  PetalMatrix() : i2c_instance_(i2c0) {
  }

  void Init() {
    constexpr size_t kSdaPin = 0;
    constexpr size_t kSclPin = 1;
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(kSdaPin, GPIO_FUNC_I2C);
    gpio_set_function(kSclPin, GPIO_FUNC_I2C);
    gpio_pull_up(kSdaPin);
    gpio_pull_up(kSclPin);

    uint8_t restart_cmd[] = {
      ShutdownRegister::Address(),
      ShutdownRegister::NormalOperationCmd(),
    };
    i2c_write_blocking(i2c_instance_, kI2cAddr, restart_cmd, sizeof(restart_cmd), false);

    uint8_t mode_cmd[] = {
      DecodeEnableRegister::Address(),
      DecodeEnableRegister::NoDecodeCmd(),
    };
    i2c_write_blocking(i2c_instance_, kI2cAddr, mode_cmd, sizeof(mode_cmd), false);

    uint8_t scan_limit_cmd[] = {
      0x0b,
      0x07,
    };
    i2c_write_blocking(i2c_instance_, kI2cAddr, scan_limit_cmd, sizeof(scan_limit_cmd), false);

    uint8_t global_intensity_cmd[] = {
      0x0A,
      0x07,
    };
    i2c_write_blocking(i2c_instance_, kI2cAddr, global_intensity_cmd, sizeof(global_intensity_cmd), false);
  }

  void EnableLed(size_t arm, size_t index) {
    if (arm > 7) {
      LOG(ERROR, "Arm %d doesn't exist", int(arm));
    }
    if (index > 7) {
      LOG(ERROR, "Index %d doesn't exist", int(index));
    }

    uint8_t cmd[] = {
      DigitRegister::Address(arm),
      DigitRegister::SegmentCmd(index)
    };
    i2c_write_blocking(i2c_instance_, kI2cAddr, cmd, sizeof(cmd), false);
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
  i2c_inst_t* i2c_instance_;
  size_t foo_;
};