#pragma once

#include <algorithm>

namespace scaled_controllers
{
// Anchor to the final segment of the SAME action goal. This remains valid when
// the controller retains a bridge from an earlier trajectory, and avoids using
// wall elapsed time under speed scaling. sample_time is the time actually passed
// to sample(), not time_data.time and not an independently integrated clock.
inline double goal_sample_phase(double sample_time, double trajectory_end,
                                double goal_duration)
{
  return std::min(goal_duration, sample_time - trajectory_end + goal_duration);
}
}  // namespace scaled_controllers
