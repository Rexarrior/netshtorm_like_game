#include <gtest/gtest.h>
#include "camera/isometric_camera.h"
#include "core/config.h"
#include "raylib_stub.h"

namespace ns {
namespace testing {

class IsometricCameraTest : public ::testing::Test {
protected:
    IsometricCamera2D cam_{Vector2{640, 360}, 1.0f};
};

TEST_F(IsometricCameraTest, GridToScreenOrigin) {
    Vector2 screen = cam_.grid_to_screen(0, 0);
    EXPECT_EQ(screen.x, 640.0f);
    EXPECT_EQ(screen.y, 360.0f);
}

TEST_F(IsometricCameraTest, GridToScreenCenter) {
    Vector2 screen = cam_.grid_to_screen(4, 4);
    EXPECT_EQ(screen.x, 640.0f);
    EXPECT_EQ(screen.y, 488.0f);
}

TEST_F(IsometricCameraTest, GridToScreenRight) {
    Vector2 screen = cam_.grid_to_screen(8, 4);
    EXPECT_EQ(screen.x, 768.0f);
    EXPECT_FLOAT_EQ(screen.y, 552.0f);
}

TEST_F(IsometricCameraTest, GridToScreenUp) {
    Vector2 screen = cam_.grid_to_screen(4, 8);
    EXPECT_EQ(screen.x, 512.0f);
    EXPECT_FLOAT_EQ(screen.y, 552.0f);
}

TEST_F(IsometricCameraTest, RoundtripGridToScreenToGrid) {
    for (int gx = -5; gx <= 20; gx += 3) {
        for (int gy = -5; gy <= 20; gy += 3) {
            Vector2 screen = cam_.grid_to_screen(gx, gy);
            Position back = cam_.screen_to_grid(screen);
            EXPECT_EQ(back.x, gx) << "gx=" << gx << " gy=" << gy;
            EXPECT_EQ(back.y, gy) << "gx=" << gx << " gy=" << gy;
        }
    }
}

TEST_F(IsometricCameraTest, RoundtripAtDifferentZooms) {
    std::vector<float> zooms = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f};
    for (float z : zooms) {
        cam_.set_zoom(z);
        for (int gx = 0; gx <= 16; gx += 4) {
            for (int gy = 0; gy <= 16; gy += 4) {
                Vector2 screen = cam_.grid_to_screen(gx, gy);
                Position back = cam_.screen_to_grid(screen);
                EXPECT_EQ(back.x, gx) << "zoom=" << z << " gx=" << gx << " gy=" << gy;
                EXPECT_EQ(back.y, gy) << "zoom=" << z << " gx=" << gx << " gy=" << gy;
            }
        }
    }
}

TEST_F(IsometricCameraTest, ScreenToGridAtCenter) {
    cam_.follow(8, 6);
    cam_.update(1.0f);
    
    Position grid = cam_.screen_to_grid(Vector2{640, 360});
    EXPECT_EQ(grid.x, 8);
    EXPECT_EQ(grid.y, 6);
}

TEST_F(IsometricCameraTest, ScreenToGridAtExtremeCorners) {
    cam_.follow(8, 8);
    cam_.update(1.0f);
    
    Position grid_tl = cam_.screen_to_grid(Vector2{0, 0});
    EXPECT_TRUE(grid_tl.x < 8 || grid_tl.y < 8);
    
    Position grid_br = cam_.screen_to_grid(Vector2{1280, 720});
    EXPECT_TRUE(grid_br.x > 8 || grid_br.y > 8);
}

TEST_F(IsometricCameraTest, InitialPositionIsZero) {
    EXPECT_EQ(cam_.target_grid_x(), 0);
    EXPECT_EQ(cam_.target_grid_y(), 0);
    EXPECT_EQ(cam_.visual_grid_x(), 0);
    EXPECT_EQ(cam_.visual_grid_y(), 0);
}

TEST_F(IsometricCameraTest, FollowSetsTargetOnly) {
    cam_.follow(10, 15);
    EXPECT_EQ(cam_.target_grid_x(), 10);
    EXPECT_EQ(cam_.target_grid_y(), 15);
    EXPECT_EQ(cam_.visual_grid_x(), 0);
    EXPECT_EQ(cam_.visual_grid_y(), 0);
}

TEST_F(IsometricCameraTest, UpdateInterpolates) {
    cam_.follow(10, 10);
    cam_.set_follow_speed(8.0f);
    
    cam_.update(0.5f);
    EXPECT_EQ(cam_.visual_grid_x(), 4);
    EXPECT_EQ(cam_.visual_grid_y(), 4);
    
    cam_.update(0.5f);
    EXPECT_EQ(cam_.visual_grid_x(), 8);
    EXPECT_EQ(cam_.visual_grid_y(), 8);
    
    cam_.update(0.5f);
    EXPECT_EQ(cam_.visual_grid_x(), 10);
    EXPECT_EQ(cam_.visual_grid_y(), 10);
}

TEST_F(IsometricCameraTest, UpdateSnapsWhenClose) {
    cam_.follow(2, 2);
    cam_.set_follow_speed(8.0f);
    
    cam_.update(1.0f);
    EXPECT_EQ(cam_.visual_grid_x(), 2);
    EXPECT_EQ(cam_.visual_grid_y(), 2);
}

TEST_F(IsometricCameraTest, MultipleFollowsLastWins) {
    cam_.follow(100, 100);
    cam_.follow(5, 5);
    EXPECT_EQ(cam_.target_grid_x(), 5);
    EXPECT_EQ(cam_.target_grid_y(), 5);
}

TEST_F(IsometricCameraTest, PanRelative) {
    cam_.follow(10, 10);
    cam_.update(1.0f);
    
    cam_.pan(3, -1);
    EXPECT_EQ(cam_.target_grid_x(), 13);
    EXPECT_EQ(cam_.target_grid_y(), 9);
}

TEST_F(IsometricCameraTest, PanToAbsolute) {
    cam_.pan_to(7, 12);
    EXPECT_EQ(cam_.target_grid_x(), 7);
    EXPECT_EQ(cam_.target_grid_y(), 12);
}

TEST_F(IsometricCameraTest, SetZoom) {
    cam_.set_zoom(1.5f);
    EXPECT_EQ(cam_.get_zoom(), 1.5f);
}

TEST_F(IsometricCameraTest, ZoomClampedToMin) {
    cam_.set_zoom(0.1f);
    EXPECT_EQ(cam_.get_zoom(), 0.5f);
}

TEST_F(IsometricCameraTest, ZoomClampedToMax) {
    cam_.set_zoom(10.0f);
    EXPECT_EQ(cam_.get_zoom(), 2.0f);
}

TEST_F(IsometricCameraTest, ZoomAtCenterKeepsCenter) {
    cam_.set_zoom(1.0f);
    cam_.follow(8, 8);
    cam_.update(1.0f);
    
    Vector2 before = cam_.grid_to_screen(8, 8);
    cam_.zoom_at(2.0f, Vector2{640, 360});
    Vector2 after = cam_.grid_to_screen(8, 8);
    
    EXPECT_EQ(before.x, after.x);
    EXPECT_EQ(before.y, after.y);
}

TEST_F(IsometricCameraTest, NegativeGridCoordinates) {
    cam_.follow(-5, -5);
    cam_.update(1.0f);
    
    Vector2 screen = cam_.grid_to_screen(-5, -5);
    EXPECT_EQ(screen.x, 640.0f);
    EXPECT_EQ(screen.y, 360.0f);
}

TEST_F(IsometricCameraTest, ZeroDtUpdateDoesNothing) {
    cam_.follow(10, 10);
    cam_.update(0.0f);
    EXPECT_EQ(cam_.visual_grid_x(), 0);
    EXPECT_EQ(cam_.visual_grid_y(), 0);
}

TEST_F(IsometricCameraTest, FollowSpeedDefaultIsReasonable) {
    IsometricCamera2D new_cam{Vector2{640, 360}, 1.0f};
    EXPECT_GE(new_cam.get_follow_speed(), 1.0f);
    EXPECT_LE(new_cam.get_follow_speed(), 20.0f);
}

TEST_F(IsometricCameraTest, GridToScreenWithFloatCoords) {
    Vector2 screen_int = cam_.grid_to_screen(4, 4);
    Vector2 screen_float = cam_.grid_to_screen(4.0f, 4.0f);
    EXPECT_EQ(screen_int.x, screen_float.x);
    EXPECT_EQ(screen_int.y, screen_float.y);
    
    Vector2 screen_4_5 = cam_.grid_to_screen(4.5f, 4.0f);
    Vector2 screen_5_4 = cam_.grid_to_screen(4.0f, 4.5f);
    
    EXPECT_NE(screen_4_5.x, screen_int.x);
    EXPECT_NE(screen_4_5.y, screen_int.y);
}

} // namespace testing
} // namespace ns