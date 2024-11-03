#include <cmath>

#include "animation.h"
#include "pico/stdlib.h"
#include "log.h"
#include "i2c.h"
#include "petal_matrix.h"
#include "touch_wheel.h"
#include "hardware/i2c.h"

class PetalAnimation1 : public Animation {
 public:
  PetalAnimation1(PetalMatrix& petal_matrix) : pm_(petal_matrix) {}

  void Reset() override {
    pm_.Reset();
    set_or_clear_ = true;
    next_i_ = 0;
    next_j_ = 0;
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
  void HandlePlaybackDirectionChanged(bool play_forwards) override {
    set_or_clear_ = !set_or_clear_;
  }

private:
  static constexpr size_t kMaxi = 8;
  static constexpr size_t kMaxj = 7;
  PetalMatrix& pm_;
  bool set_or_clear_ = true;
  int next_i_ = 0;
  int next_j_ = 0;
};

class PetalAnimation2 : public Animation {
 public:
  PetalAnimation2(PetalMatrix& petal_matrix) : pm_(petal_matrix) {}

  void Reset() override {
    pm_.Reset();
    set_or_clear_ = true;
    next_i_ = 0;
    next_j_ = 0;
  }

  void Update(int count) override {
    int step = count < 0 ? -1 : 1;
    for (size_t i = 0; i < abs(count); i++) {
      pm_.LedState(next_j_, next_i_, set_or_clear_);
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
  void HandlePlaybackDirectionChanged(bool play_forwards) override {
    set_or_clear_ = !set_or_clear_;
  }

private:
  static constexpr size_t kMaxi = 7;
  static constexpr size_t kMaxj = 8;
  PetalMatrix& pm_;
  bool set_or_clear_ = true;
  int next_i_ = 0;
  int next_j_ = 0;
};

class PetalAnimation3 : public Animation {
 public:
  PetalAnimation3(PetalMatrix& petal_matrix) : pm_(petal_matrix) {}

  void Reset() override {
    pm_.Reset();
    next_i_ = 0;
    step_accumulator_ = 0;
  }

  void Update(int count) override {
    int step = count < 0 ? -1 : 1;
    for (size_t i = 0; i < abs(count); i++) {
      if (step_accumulator_ % kThrottleRate == 0) {
        int prev = next_i_ + (step * -1);
        for (size_t j = 0; j < kMaxj; j++) {
          pm_.LedState(next_i_ % kMaxi, j, true);
          if (prev >= 0) {
            pm_.LedState(prev % kMaxi, j, false);
          }
        }
        next_i_ += step;
      }
      step_accumulator_++;
    }
  }

private:
  static constexpr size_t kThrottleRate = 5;
  static constexpr size_t kMaxi = 8;
  static constexpr size_t kMaxj = 7;
  PetalMatrix& pm_;
  int next_i_ = 0;
  int step_accumulator_ = 0;;
};

class PetalAnimation4 : public Animation {
 public:
  PetalAnimation4(PetalMatrix& petal_matrix) : pm_(petal_matrix) {}

  void Reset() override {
    pm_.Reset();
    next_i_ = 0;
    step_accumulator_ = 0;
  }

  void Update(int count) override {
    int step = count < 0 ? -1 : 1;
    for (size_t i = 0; i < abs(count); i++) {
      if (step_accumulator_ % kThrottleRate == 0) {
        int prev = next_i_ + (step * -1);
        for (size_t j = 0; j < kMaxj; j++) {
          pm_.LedState(j, next_i_ % kMaxi, true);
          if (prev >= 0) {
            pm_.LedState(j, prev % kMaxi, false);
          }
        }
        next_i_ += step;
      }
      step_accumulator_++;
    }
  }

private:
  static constexpr size_t kThrottleRate = 5;
  static constexpr size_t kMaxi = 7;
  static constexpr size_t kMaxj = 8;
  PetalMatrix& pm_;
  int next_i_ = 0;
  int step_accumulator_ = 0;;
};

uint32_t PositionToFrameTime(uint8_t position) {
  constexpr size_t kMinFrameTime = 5000;
  constexpr size_t kMaxFrameTime = 100000;
  if (position > 128) {
    position = 255 - position;
  }
  return ((kMaxFrameTime - kMinFrameTime) * position)/128 + kMinFrameTime;
}

int main() {
  constexpr size_t kDefaultFrameTiming = 10000;
  constexpr uint64_t kAnimationSwitchInterval = 10000000;

  stdio_init_all();
  I2cBus i2c_bus0(i2c0, 0, 1);
  I2cBus i2c_bus1(i2c1, 26, 27);
  i2c_bus0.Init();
  i2c_bus1.Init();

  // Application stuff.
  Animator pa(kDefaultFrameTiming);
  PetalMatrix pm(i2c_bus0);
  pm.Init();

  PetalAnimation1 pa1(pm);
  PetalAnimation2 pa2(pm);
  PetalAnimation3 pa3(pm);
  PetalAnimation4 pa4(pm);

  std::array<Animation*, 4> animations{
    &pa1,
    &pa2,
    &pa3,
    &pa4,
  };

  TouchWheel tw(i2c_bus1);
  size_t current_animation = 0;
  pa.SetAnimation(animations[current_animation]);

  uint64_t last_animation_change = time_us_64();
  while (true) {
    pa.Advance();

    uint8_t wheel_pos = tw.GetRaw();
    if (wheel_pos != 0 && wheel_pos != 255) {
      if (wheel_pos < 128) {
        pa.PlayForwards(false);
      } else {
        pa.PlayForwards(true);
      }
      pa.SetFrameTime(PositionToFrameTime(wheel_pos));
      LOG(DEBUG, "Touch wheel pos %d", int(wheel_pos));
    } else if (wheel_pos == 0) {
      pa.PlayForwards(true);
      pa.SetFrameTime(kDefaultFrameTiming);

      uint64_t current_time = time_us_64();
      if (current_time > last_animation_change + kAnimationSwitchInterval) {
        last_animation_change = current_time;
        current_animation = (current_animation + 1) % animations.size();
        pa.SetAnimation(animations[current_animation]);
      }
    }
  }
}
