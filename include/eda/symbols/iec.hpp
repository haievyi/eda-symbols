/*
 * include/eda/symbols/iec.hpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Factory declarations for IEC schematic symbology.
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
 * class IecSymbolFactory
 * Generates Direct2D path geometries and topological terminal anchors.
 */
class IecSymbolFactory {
public:
    IecSymbolFactory() = delete;

    /**
     * BuildCircuitBreaker() - Generate raw IEC circuit breaker path geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEC 60617 enclosed circuit breaker frame (-6, -12 to 6, 12)
     * with an internal circuit-breaker cross ('X') and collinear terminal leads.
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
     * CreateCircuitBreaker() - Construct IEC circuit breaker with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateCircuitBreaker(
        ID2D1Factory* factory
    ) noexcept;

    /**
     * BuildDisconnectSwitch() - Generate raw IEC disconnector (isolator) geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @state:    Operating position (Open or Closed)
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEC 60617-07-02-01 disconnector featuring a pivot hinge
     * ring, moving switch blade, and a horizontal isolator stop bar.
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
     * CreateDisconnectSwitch() - Construct IEC disconnector with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     * @state:   Operating position (defaults to SwitchState::Open)
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateDisconnectSwitch(
        ID2D1Factory* factory,
        SwitchState state = SwitchState::Open
    ) noexcept;

    /**
     * BuildFuse() - Generate raw IEC power fuse path geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEC 60617-07-21-01 fuse featuring a rectangular body
     * (-5, -10 to 5, 10) with the continuous conductor line passing straight
     * through its longitudinal axis.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildFuse(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateFuse() - Construct IEC power fuse with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateFuse(
        ID2D1Factory* factory
    ) noexcept;

    /**
     * BuildTransformer2W() - Generate raw IEC 2-winding transformer geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Constructs an IEC 60617-06-09-02 single-line diagram two-winding
     * transformer represented by two interlocking circles of radius 9 DIPs.
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildTransformer2W(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateTransformer2W() - Construct IEC 2-winding transformer with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateTransformer2W(
        ID2D1Factory* factory
    ) noexcept;

    /**
     * BuildEarthGround() - Generate raw IEC protective earth path geometry
     * @factory:  Pointer to an initialized Direct2D factory instance
     * @geometry: Output pointer receiving the allocated path geometry
     *
     * Context: Caller acquires reference ownership; must release on completion.
     * Return: S_OK on success, E_INVALIDARG on null pointers, or D2D error code.
     */
    static HRESULT BuildEarthGround(
        ID2D1Factory* factory,
        ID2D1PathGeometry** geometry
    ) noexcept;

    /**
     * CreateEarthGround() - Construct IEC earth ground with metadata
     * @factory: Pointer to an initialized Direct2D factory instance
     *
     * Return: Populated SymbolDefinition; empty definition on failure.
     */
    [[nodiscard]] static SymbolDefinition CreateEarthGround(
        ID2D1Factory* factory
    ) noexcept;
};

} /* namespace eda::cad::symbols */
