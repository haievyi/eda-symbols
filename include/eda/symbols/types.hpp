/*
 * include/eda/symbols/types.hpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Shared enumerations, terminal pin metadata, and symbol definition structs.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d2d1.h>
#include <wrl/client.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace eda::cad::symbols {

/**
 * enum class Standard
 * @IEEE: North American IEEE / ANSI 315 conventions
 * @IEC:  International / European IEC 60617 conventions
 */
enum class Standard : std::uint8_t {
    IEEE,
    IEC,
};

/**
 * enum class Topology
 * @InLine: Two-terminal series device inserted along a feeder
 * @Shunt:  Single-terminal reference or tap element (e.g., ground)
 */
enum class Topology : std::uint8_t {
    InLine,
    Shunt,
};

/**
 * enum class SwitchState
 * @Open:   Switch blade is thrown open (isolated)
 * @Closed: Switch blade is closed (conducting)
 */
enum class SwitchState : std::uint8_t {
    Open,
    Closed,
};

/**
 * struct TerminalPin - Topological wire connection point
 * @name:     Semantic identifier (e.g., "LINE", "LOAD", "PRI", "SEC", "GND")
 * @position: Pin coordinates in local symbol DIP space
 * @normal:   Outward unit direction vector for wire auto-routing
 */
struct TerminalPin {
    std::string_view name{};
    D2D1_POINT_2F position{0.0f, 0.0f};
    D2D1_POINT_2F normal{0.0f, 0.0f};
};

/**
 * struct SymbolDefinition - Complete schematic symbol package
 * @geometry: Direct2D path geometry handle
 * @bounds:   Precomputed local-space bounding box in DIPs
 * @pins:     List of wire snapping terminals
 * @standard: Originating schematic standard (IEEE or IEC)
 * @topology: Placement topology (InLine or Shunt)
 */
struct SymbolDefinition {
    Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry{};
    D2D1_RECT_F bounds{0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<TerminalPin> pins{};
    Standard standard{Standard::IEEE};
    Topology topology{Topology::InLine};

    [[nodiscard]] explicit operator bool() const noexcept {
        return geometry != nullptr;
    }
};

} /* namespace eda::cad::symbols */
