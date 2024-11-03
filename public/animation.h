#pragma once

#include <cstdint>

#include "pico/stdlib.h"

class Animation {
 public:
  virtual void Reset() = 0;
  virtual void Update(int step_count) = 0;
  virtual void HandlePlaybackDirectionChanged(bool play_forwards) {}
};

class Animator {
 public:
  Animator(uint32_t frame_timing_us) : frame_timing_us_(frame_timing_us) {}

  void PlayForwards(bool play_forwards) {
    if (play_forwards != playback_forwards_ && animation_ != nullptr) {
      animation_->HandlePlaybackDirectionChanged(play_forwards);
    }
    playback_forwards_ = play_forwards;
  }

  void Advance() {
    uint64_t time = time_us_64();

    int steps = (time - last_update_us_)/frame_timing_us_;
    if (steps) {
      steps = playback_forwards_ ? steps : -1*steps;
      if (animation_ != nullptr) {
        animation_->Update(steps);
      }
      last_update_us_ = time;
    }
  }

  void SetAnimation(Animation* animation) {
    animation_ = animation;
    if (animation_ != nullptr) {
      animation_->Reset();
    }
  }

  void SetFrameTime(uint32_t frame_time) { frame_timing_us_ = frame_time; }
 private:
  Animation* animation_ = nullptr;
  bool playback_forwards_ = true;
  uint64_t last_update_us_ = 0;
  uint32_t frame_timing_us_;
};
