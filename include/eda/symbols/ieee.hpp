/*
 * include/eda/symbols/ieee.hpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Factory declarations for IEEE / ANSI schematic symbology.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d2d1.h>
#include <wrl/client.h>

#include <eda/symbols/types.hpp>

namespace eda::cad::symbols {

/**
 * class IeeeSymbolFactory
 * Generates Direct2D path geometries and topological terminal anchors.
 *
 * All methods provide two calling conventions:
 * 1. Low-level COM ABI functions (prefixed with Build) that write directly
 *    to an ID2D1PathGeometry** and return HRESULT status codes.
 * 2. High-level C++ functions (prefixed with Create) that return a
 *    SymbolDefinition structure bundling the ComPtr geometry, bounding box,
 *    and snapping pins.
 */
class IeeeSymbolFactory {
public:
    IeeeSymbolFactory() = delete;

    /**
     * BuildCircuitBreaker() - Generate raw IEEE circuit breaker path geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEEE single-line diagram circuit breaker consisting of
     * an in-line square body (-10, -10 to 10, 10) and collinear terminal leads.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG if parameters are null, or a
     *         Direct2D HRESULT failure code.
     */
    static HRESULT BuildCircuitBreaker(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateCircuitBreaker() - Construct IEEE circuit breaker with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Allocates geometry and populates two in-line connection pins:
     * - "LINE" at (0, -20) with normal (0, -1)
     * - "LOAD" at (0, 20) with normal (0, 1)
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateCircuitBreaker(
        ID2D1Factory* factory
    ) noexcept;

    /**
     * BuildDisconnectSwitch() - Generate raw IEEE disconnect switch geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @state:    Operating position (Open or Closed)
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEEE single-line diagram disconnect switch (isolator)
     * featuring a stationary hinge pad, blade arm, and stationary contact terminal.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildDisconnectSwitch(
        ID2D1Factory* factory,
        SwitchState state,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateDisconnectSwitch() - Construct IEEE disconnect switch with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     * @state:   Operating position (defaults to SwitchState::Open)
     *
     * Allocates geometry and populates two in-line connection pins:
     * - "HINGE" at (0, -20) with normal (0, -1)
     * - "JAW" at (0, 20) with normal (0, 1)
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateDisconnectSwitch(
        ID2D1Factory* factory,
        SwitchState state = SwitchState::Open
    ) noexcept;

    /**
     * BuildFuse() - Generate raw IEEE power fuse path geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEEE single-line diagram power fuse featuring a continuous
     * sinusoidal / S-curve expansion between straight terminal leads.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildFuse(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateFuse() - Construct IEEE power fuse with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Allocates geometry and populates two in-line connection pins:
     * - "LINE" at (0, -20) with normal (0, -1)
     * - "LOAD" at (0, 20) with normal (0, 1)
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateFuse(
        ID2D1Factory* factory
    ) noexcept;

    /**
     * BuildTransformer2W() - Generate raw IEEE 2-winding transformer geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEEE single-line diagram two-winding transformer consisting
     * of two opposing scalloped induction coils.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildTransformer2W(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateTransformer2W() - Construct IEEE 2-winding transformer with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Allocates geometry and populates primary and secondary terminal pins:
     * - "PRI" at (0, -25) with normal (0, -1)
     * - "SEC" at (0, 25) with normal (0, 1)
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateTransformer2W(
        ID2D1Factory* factory
    ) noexcept;

    /**
     * BuildEarthGround() - Generate raw IEEE earth ground path geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEEE standard earth reference with descending horizontal bars.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildEarthGround(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateEarthGround() - Construct IEEE earth ground with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Allocates geometry and populates a single shunt connection pin:
     * - "GND" at (0, -15) with normal (0, -1)
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateEarthGround(
        ID2D1Factory* factory
    ) noexcept;
};

} /* namespace eda::cad::symbols */
