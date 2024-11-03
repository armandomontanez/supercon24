#pragma once

#include <cstdint>

#include "pico/stdlib.h"

class Animation {
 public:
  Animation(uint32_t frame_timing_us) : frame_timing_us_(frame_timing_us) {}

  void PlayForwards(bool play_forwards) {
    if (play_forwards != playback_forwards_) {
      HandlePlaybackDirectionChanged(play_forwards);
    }
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

  void SetFrameTime(uint32_t frame_time) { frame_timing_us_ = frame_time; }
  virtual void Update(int count) = 0;
 private:
  virtual void HandlePlaybackDirectionChanged(bool play_forwards) {}

  bool playback_forwards_ = true;
  uint64_t last_update_us_ = 0;
  uint32_t frame_timing_us_;
};