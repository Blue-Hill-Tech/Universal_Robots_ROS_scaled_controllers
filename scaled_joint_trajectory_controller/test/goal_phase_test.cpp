#include <gtest/gtest.h>
#include "scaled_joint_trajectory_controller/goal_phase.h"

TEST(GoalPhase, UsesControllerSampleTimeNotWallElapsed) {
  EXPECT_DOUBLE_EQ(2.0, scaled_controllers::goal_sample_phase(102.0, 110.0, 10.0));
  // Ten seconds of wall time passing without a changed sample time is a stall.
  EXPECT_DOUBLE_EQ(2.0, scaled_controllers::goal_sample_phase(102.0, 110.0, 10.0));
  EXPECT_DOUBLE_EQ(2.5, scaled_controllers::goal_sample_phase(102.5, 110.0, 10.0));
}
TEST(GoalPhase, EndpointAnchorSurvivesRetainedPrefixAndNewGoal) {
  EXPECT_DOUBLE_EQ(3.0, scaled_controllers::goal_sample_phase(503.0, 510.0, 10.0));
  EXPECT_DOUBLE_EQ(0.0, scaled_controllers::goal_sample_phase(503.0, 507.0, 4.0));
}
TEST(GoalPhase, DoesNotReportPastEndpointOrHideFutureStart) {
  EXPECT_DOUBLE_EQ(10.0, scaled_controllers::goal_sample_phase(111.0, 110.0, 10.0));
  EXPECT_DOUBLE_EQ(-1.0, scaled_controllers::goal_sample_phase(99.0, 110.0, 10.0));
}
