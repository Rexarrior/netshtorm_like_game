#include "raylib_stub.h"

#include <gtest/gtest.h>
#include "map/island.h"
#include "map/island_generator.h"
#include "map/bridge.h"
#include "map/bridge_placer.h"
#include "map/map.h"

using namespace ns;

// ── Island Tests ──────────────────────────────────────────────

TEST(IslandTest, DiamondIslandGeneration) {
    Island island = IslandGenerator::generate_diamond_island(5, 5, 2, 0, IslandType::Home);
    EXPECT_EQ(island.id, 0);
    EXPECT_EQ(island.type, IslandType::Home);
    EXPECT_EQ(island.center.x, 5);
    EXPECT_EQ(island.center.y, 5);
    EXPECT_EQ(island.tiles.size(), 13u);
}

TEST(IslandTest, IslandContains) {
    Island island = IslandGenerator::generate_diamond_island(5, 5, 2, 0, IslandType::Home);
    EXPECT_TRUE(island.contains(5, 5));
    EXPECT_TRUE(island.contains(5, 3));
    EXPECT_TRUE(island.contains(5, 7));
    EXPECT_TRUE(island.contains(3, 5));
    EXPECT_TRUE(island.contains(7, 5));
    EXPECT_FALSE(island.contains(0, 0));
    EXPECT_FALSE(island.contains(10, 10));
}

TEST(IslandTest, ConnectionPoints) {
    Island island = IslandGenerator::generate_diamond_island(5, 5, 2, 0, IslandType::Home);
    EXPECT_EQ(island.connection_points.size(), 4u);
    EXPECT_EQ(island.connection_points[0].gx, 5);
    EXPECT_EQ(island.connection_points[0].gy, 3);
    EXPECT_EQ(island.connection_points[1].gx, 5);
    EXPECT_EQ(island.connection_points[1].gy, 7);
}

TEST(IslandTest, NearestConnectionPoint) {
    Island island = IslandGenerator::generate_diamond_island(5, 5, 2, 0, IslandType::Home);
    TilePosition nearest = island.nearest_connection_point(5, 0);
    EXPECT_EQ(nearest.gx, 5);
    EXPECT_EQ(nearest.gy, 3);
}

// ── Bridge Tests ──────────────────────────────────────────────

TEST(BridgeTest, OccupiedTilesStraight) {
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Straight, {3, 3});
    EXPECT_EQ(tiles.size(), 1u);
    EXPECT_EQ(tiles[0].gx, 3);
    EXPECT_EQ(tiles[0].gy, 3);
}

TEST(BridgeTest, OccupiedTilesLShape) {
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::LShape, {3, 3});
    EXPECT_EQ(tiles.size(), 3u);
}

TEST(BridgeTest, OccupiedTilesTShape) {
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::TShape, {3, 3});
    EXPECT_EQ(tiles.size(), 4u);
}

TEST(BridgeTest, OccupiedTilesCross) {
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Cross, {3, 3});
    EXPECT_EQ(tiles.size(), 5u);
}

TEST(BridgeTest, OccupiedTilesLongStraight) {
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::LongStraight, {3, 3});
    EXPECT_EQ(tiles.size(), 2u);
}

TEST(BridgeTest, OccupiedTilesZigzag) {
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Zigzag, {3, 3});
    EXPECT_EQ(tiles.size(), 2u);
    EXPECT_EQ(tiles[0].gx, 3);
    EXPECT_EQ(tiles[0].gy, 3);
    EXPECT_EQ(tiles[1].gx, 4);
    EXPECT_EQ(tiles[1].gy, 4);
}

TEST(BridgeTest, BridgeHP) {
    BridgeSegment bridge;
    bridge.shape = BridgeShape::Straight;
    bridge.state = BridgeState::Cracked;
    EXPECT_EQ(bridge.max_hp(), 50);

    BridgeSegment bridge2;
    bridge2.harden();
    EXPECT_EQ(bridge2.max_hp(), 150);
    EXPECT_EQ(bridge2.hp, 150);
}

TEST(BridgeTest, BridgeDamage) {
    BridgeSegment bridge;
    bridge.hp = 50;
    bridge.take_damage(20);
    EXPECT_EQ(bridge.hp, 30);
    EXPECT_TRUE(bridge.is_alive());
    bridge.take_damage(30);
    EXPECT_EQ(bridge.hp, 0);
    EXPECT_FALSE(bridge.is_alive());
}

// ── Bridge Placement Tests ────────────────────────────────────

TEST(BridgePlacerTest, CanPlaceNoOverlap) {
    std::vector<Island> islands;
    std::vector<BridgeSegment> bridges;
    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Straight, {3, 3});
    EXPECT_TRUE(BridgePlacer::can_place(tiles, islands, bridges));
}

TEST(BridgePlacerTest, CanPlaceOverlap) {
    std::vector<Island> islands;
    std::vector<BridgeSegment> bridges;
    BridgeSegment existing;
    existing.shape = BridgeShape::Straight;
    existing.origin = {3, 3};
    existing.owner_id = 0;
    existing.occupied_tiles = {{3, 3}};
    existing.hp = 50;
    bridges.push_back(existing);

    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Straight, {3, 3});
    EXPECT_FALSE(BridgePlacer::can_place(tiles, islands, bridges));
}

TEST(BridgePlacerTest, IsAdjacentToIsland) {
    Island island = IslandGenerator::generate_diamond_island(5, 5, 2, 0, IslandType::Home);
    std::vector<Island> islands = {island};
    std::vector<BridgeSegment> bridges;

    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Straight, {5, 2});
    EXPECT_TRUE(BridgePlacer::is_adjacent_to_existing(tiles, islands, bridges, 0));
}

TEST(BridgePlacerTest, IsAdjacentToBridge) {
    Island island = IslandGenerator::generate_diamond_island(5, 5, 2, 0, IslandType::Home);
    std::vector<Island> islands = {island};
    std::vector<BridgeSegment> bridges;

    BridgeSegment b1;
    b1.shape = BridgeShape::Straight;
    b1.origin = {5, 2};
    b1.owner_id = 0;
    b1.occupied_tiles = {{5, 2}};
    b1.hp = 50;
    bridges.push_back(b1);

    auto tiles = BridgePlacer::get_occupied_tiles(BridgeShape::Straight, {5, 1});
    EXPECT_TRUE(BridgePlacer::is_adjacent_to_existing(tiles, islands, bridges, 0));
}

// ── GameMap Tests ─────────────────────────────────────────────

TEST(GameMapTest, AddIslands) {
    GameMap map;
    auto home = IslandGenerator::generate_home_island(5, 5, 2);
    for (const auto& i : home) map.add_island(i);
    EXPECT_EQ(map.islands().size(), 1u);
}

TEST(GameMapTest, PlaceBridge) {
    GameMap map;
    auto home = IslandGenerator::generate_home_island(5, 5, 2);
    for (const auto& i : home) map.add_island(i);

    bool placed = map.place_bridge(BridgeShape::Straight, {5, 2}, 0);
    EXPECT_TRUE(placed);
    EXPECT_EQ(map.active_bridge_count(0), 1);
}

TEST(GameMapTest, PlaceBridgeNotAdjacent) {
    GameMap map;
    auto home = IslandGenerator::generate_home_island(5, 5, 2);
    for (const auto& i : home) map.add_island(i);

    bool placed = map.place_bridge(BridgeShape::Straight, {20, 20}, 0);
    EXPECT_FALSE(placed);
}

TEST(GameMapTest, PlaceBridgeOverlap) {
    GameMap map;
    auto home = IslandGenerator::generate_home_island(5, 5, 2);
    for (const auto& i : home) map.add_island(i);

    bool placed1 = map.place_bridge(BridgeShape::Straight, {5, 2}, 0);
    EXPECT_TRUE(placed1);

    bool placed2 = map.place_bridge(BridgeShape::Straight, {5, 2}, 0);
    EXPECT_FALSE(placed2);
}

TEST(GameMapTest, TileOwner) {
    GameMap map;
    auto home = IslandGenerator::generate_home_island(5, 5, 2);
    for (const auto& i : home) map.add_island(i);

    EXPECT_EQ(map.tile_owner(5, 5), IslandType::Home);
    EXPECT_EQ(map.tile_owner(0, 0), IslandType::None);
}

TEST(GameMapTest, IsBridgeAt) {
    GameMap map;
    auto home = IslandGenerator::generate_home_island(5, 5, 2);
    for (const auto& i : home) map.add_island(i);

    map.place_bridge(BridgeShape::Straight, {5, 2}, 0);
    EXPECT_TRUE(map.is_bridge_at(5, 2));
    EXPECT_FALSE(map.is_bridge_at(5, 5));
}

TEST(GameMapTest, GeyserAddition) {
    GameMap map;
    map.add_geyser({10, 10}, 0);
    EXPECT_EQ(map.geysers().size(), 1u);
    EXPECT_EQ(map.geysers()[0].pos.x, 10);
    EXPECT_EQ(map.geysers()[0].pos.y, 10);
    EXPECT_EQ(map.geysers()[0].production_rate, 10);
}
