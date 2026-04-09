#pragma once

namespace ns {

enum class InputAction {
    None,
    PanUp, PanDown, PanLeft, PanRight,
    ZoomIn, ZoomOut,
    SelectBridge, PlaceBridge,
    SelectUnit, PlaceUnit,
    SelectSpell, CastSpell,
    Pause, SpeedUp, SpeedDown,
    Enter, Escape,
    MouseLeft, MouseRight, MouseMiddle
};

} // namespace ns
