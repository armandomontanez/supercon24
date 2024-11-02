#include <cmath>

#include "pico/stdlib.h"
#include "log.h"
#include "i2c.h"
#include "petal_matrix.h"
#include "touch_wheel.h"
#include "hardware/i2c.h"

class Animation {
 public:
  Animation(uint32_t frame_timing_us) : frame_timing_us_(frame_timing_us) {}

  void PlayForwards(bool play_forwards) {
    playback_forwards_ = play_forwards;
  }

  void Advance() {
    uint64_t time = time_us_64();

    int steps = (time - last_update_us_)/frame_timing_us_;
    if (steps) {
      steps = playback_forwards_ ? steps : -1*steps;
      Update(steps);
      last_update_us_ = time;
    }
  }
  virtual void Update(int count) = 0;
 private:
  bool playback_forwards_ = true;
  uint64_t last_update_us_ = 0;
  uint32_t frame_timing_us_;
};

class PetalAnimation : public Animation {
 public:
  PetalAnimation(I2cBus& bus, uint32_t frame_timing_us) : Animation(frame_timing_us), pm_(bus) {}
  void Init() {
    pm_.Init();
  }

  void Update(int count) override {
    int step = count < 0 ? -1 : 1;
    for (size_t i = 0; i < abs(count); i++) {
      pm_.LedState(next_i_, next_j_, set_or_clear_);
      next_j_ += step;
      if (next_j_ < 0 || next_j_ >= kMaxj) {
        next_i_ += step;
        next_j_ = next_j_ < 0 ? kMaxj - 1 : 0;
      }
      if (next_i_ < 0 || next_i_ >= kMaxi) {
        next_i_ = next_i_ < 0 ? kMaxi - 1 : 0;
        set_or_clear_ = !set_or_clear_;
      }
    }
  }

private:
  static constexpr size_t kMaxi = 8;
  static constexpr size_t kMaxj = 7;
  PetalMatrix pm_;
  bool set_or_clear_ = true;
  int next_i_ = 0;
  int next_j_ = 0;
};

int main() {
  constexpr size_t kDefaultFrameTiming = 10000;

  stdio_init_all();
  I2cBus i2c_bus0(i2c0, 0, 1);
  I2cBus i2c_bus1(i2c0, 26, 27);
  i2c_bus0.Init();
  i2c_bus1.Init();

  // Application stuff.
  PetalAnimation pa(i2c_bus0, kDefaultFrameTiming);
  TouchWheel tw(i2c_bus1);
  pa.Init();
  while (true) {
    pa.Advance();
    uint8_t wheel_pos = tw.GetRaw();
    if (wheel_pos != 0) {
      LOG(DEBUG, "Touch wheel pos %d", int(wheel_pos));
    }
  }
}
