# Goal-scoped execution-phase feedback

The scaled controller now publishes `actual.time_from_start` and
`desired.time_from_start` in `FollowJointTrajectory` action feedback as the
goal-relative time used by its interpolation sample. `error.time_from_start` is
zero because both fields refer to the same sample. This does not change the
sample clock, joint commands, speed scaling, or tolerance checks.

`header.frame_id == "ur_phase_v1"` identifies this contract; it is a protocol
marker, not a geometric frame. Feedback from a retained segment not owned by the
active action goal has no marker and must not be used as that goal's clock.
Actionlib's goal ID, not this marker or the index-only tracking topic, identifies
which goal produced the feedback. No new ROS message or topic is introduced.

The phase is anchored to the active goal's final segment:

```
phase = min(goal_duration, sample_time - trajectory_end + goal_duration)
```

This avoids accumulating wall-clock time and works with retained entry segments,
new goals and speed scaling. A negative phase is not marked valid. The phase
uses the controller's existing `traj_time` exactly; it does not promise monotonic
time beyond what that sampler supplies. Consumers must reject discontinuities,
stale feedback, and cross-goal callbacks, and stop extrapolating at termination.
Controller-state topic timing and the legacy tracking topic are not changed.

Validation: three arithmetic unit tests cover scaled progression/stall, new-goal
endpoint anchoring, future starts and endpoint clamping. Both position and
velocity plugin template instantiations pass a local C++ syntax check using ROS
Noetic controller headers. Full target-environment Release build and controller
integration tests (including retained segments, cancellation, zero scaling and
period jitter) remain required. These checks do not prove physical stopping or
multi-actuator synchronization safety.
