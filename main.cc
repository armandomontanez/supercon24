#include <cmath>

#include "pico/stdlib.h"
#include "log.h"
#include "petal_matrix.h"

class Animation {
 public:
  void Advance() {
    uint64_t time = time_us_64();

    int steps = (time - last_update_us_)/frame_timing_us_;
    if (steps) {
      Update(steps);
      last_update_us_ = time;
    }
  }
  virtual void Update(int count) = 0;
 private:
  uint64_t last_update_us_ = 0;
  uint32_t frame_timing_us_ = 10000;
};

class PetalAnimation : public Animation {
 public:
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
  stdio_init_all();
  PetalAnimation pa;
  pa.Init();
  while (true) {
    pa.Advance();
    sleep_ms(10);
  }
}
