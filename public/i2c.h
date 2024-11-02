#pragma once

#include "hardware/i2c.h"

class I2cBus {
 public:
  I2cBus(i2c_inst_t* i2c_bus, int sda_pin, int scl_pin) : native_handle_(i2c_bus), sda_pin_(sda_pin), scl_pin_(scl_pin) {}

  void Init() {
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(sda_pin_, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin_, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin_);
    gpio_pull_up(scl_pin_);
  }

  i2c_inst_t* native_handle() { return native_handle_; }

 private:
  i2c_inst_t* native_handle_;
  int sda_pin_;
  int scl_pin_;
};

class I2cDevice {
 public:
  I2cDevice(I2cBus& i2c_bus, uint8_t device_address) : i2c_bus_(i2c_bus), device_address_(device_address) {}

  template<size_t kSize>
  void Write(uint8_t const (&command)[kSize]) {
    i2c_write_blocking(i2c_bus_.native_handle(), device_address_, command, kSize, false);
  }

  uint8_t ReadByte() {
    uint8_t dest;
    i2c_read_blocking(i2c_bus_.native_handle(), device_address_, &dest, 1, false);
    return dest;
  }

 private:
  I2cBus& i2c_bus_;
  uint8_t device_address_;
};