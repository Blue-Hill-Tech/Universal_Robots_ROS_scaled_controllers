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

Ordering is part of the consumer contract: an unmarked retained/pre-goal interval
can precede the first valid marked sample of a goal. Consumers may wait through
that interval only with a bounded first-phase deadline (legacy publishers can
remain unmarked forever). After the first valid sample, an empty marker means
loss of the phase contract, not a new pre-goal interval. Consumers must latch that
goal invalid rather than resume accepting it when the marker returns. Only a new
goal identity resets this qualification. A producer change allowing ordinary
unmarked gaps after qualification requires a reviewed contract change, not silent
reuse of `ur_phase_v1`.

The phase is anchored to the active goal's final segment:

```
phase = min(goal_duration, sample_time - trajectory_end + goal_duration)
```

This avoids accumulating wall-clock time and works with retained entry segments,
new goals and speed scaling. A negative phase is not marked valid. The phase
uses the controller's existing `traj_time` exactly; it does not promise monotonic
time beyond what that sampler supplies. Consumers must reject discontinuities,
stale feedback, and cross-goal callbacks, and stop extrapolating at termination.
Controller-state topic timing and normal legacy tracking behavior are unchanged.

The iterator-guard move also fixes an independent undefined-behavior defect in
the base controller: joint-0 tracking dereferenced `segment_it->endTime()` before
checking whether `sample()` returned `end()`. The guard now runs before that
dereference. On a cycle with no joint-0 sample, the controller logs and returns
without publishing tracking status; that error-cycle behavior change is
intentional. If a later joint has no sample, joint-0 tracking may already have
published earlier in the loop. The error logger is not real-time safe; this path
is exceptional, not a new real-time guarantee. Missing-segment injection and
update-loop overhead still require controller-in-loop validation.

Validation: three arithmetic unit tests cover scaled progression/stall, new-goal
endpoint anchoring, future starts and endpoint clamping. Both position and
velocity plugin template instantiations pass a local C++ syntax check using ROS
Noetic controller headers. Full target-environment Release build and controller
integration tests (including retained segments, cancellation, zero scaling and
period jitter) remain required. These checks do not prove physical stopping or
multi-actuator synchronization safety.
