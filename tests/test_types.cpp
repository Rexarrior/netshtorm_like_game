#include "raylib_stub.h"

#include <gtest/gtest.h>
#include "core/types.h"

using namespace ns;

TEST(TypesTest, PositionEquality) {
    Position a{3, 5};
    Position b{3, 5};
    Position c{3, 6};
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(TypesTest, PositionAddition) {
    Position a{3, 5};
    Position b{2, 1};
    Position result = a + b;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 6);
}

TEST(TypesTest, ElementToString) {
    EXPECT_EQ(element_to_string(Element::Sun), "Sun");
    EXPECT_EQ(element_to_string(Element::Thunder), "Thunder");
    EXPECT_EQ(element_to_string(Element::Wind), "Wind");
    EXPECT_EQ(element_to_string(Element::Rain), "Rain");
    EXPECT_EQ(element_to_string(Element::None), "None");
}

TEST(TypesTest, GameStateToString) {
    EXPECT_EQ(game_state_to_string(GameState::Menu), "Menu");
    EXPECT_EQ(game_state_to_string(GameState::Playing), "Playing");
    EXPECT_EQ(game_state_to_string(GameState::Paused), "Paused");
    EXPECT_EQ(game_state_to_string(GameState::GameOver), "GameOver");
    EXPECT_EQ(game_state_to_string(GameState::Victory), "Victory");
}

TEST(TypesTest, TilePositionEquality) {
    TilePosition a{1, 2};
    TilePosition b{1, 2};
    TilePosition c{2, 2};
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}
