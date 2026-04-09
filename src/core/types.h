#pragma once

#include <string>

namespace ns {

enum class GameState { Menu, Playing, Paused, GameOver, Victory };
enum class Element { Sun, Thunder, Wind, Rain, None };
enum class UnitType { Offensive, Defensive, Transport, Building };
enum class BridgeShape { Straight, LShape, TShape, Cross, LongStraight, Zigzag };
enum class BridgeState { Cracked, Hardened };
enum class IslandType { None, Home, Resource, Strategic, Enemy };
enum class Direction { North, South, East, West, None };

struct Position {
    int x = 0;
    int y = 0;
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position& other) const { return !(*this == other); }
    Position operator+(const Position& other) const { return {x + other.x, y + other.y}; }
};

struct TilePosition {
    int gx = 0;
    int gy = 0;
    bool operator==(const TilePosition& other) const { return gx == other.gx && gy == other.gy; }
    bool operator!=(const TilePosition& other) const { return !(*this == other); }
};

inline std::string element_to_string(Element e) {
    switch (e) {
        case Element::Sun: return "Sun";
        case Element::Thunder: return "Thunder";
        case Element::Wind: return "Wind";
        case Element::Rain: return "Rain";
        default: return "None";
    }
}

inline std::string game_state_to_string(GameState s) {
    switch (s) {
        case GameState::Menu: return "Menu";
        case GameState::Playing: return "Playing";
        case GameState::Paused: return "Paused";
        case GameState::GameOver: return "GameOver";
        case GameState::Victory: return "Victory";
    }
    return "Unknown";
}

} // namespace ns
