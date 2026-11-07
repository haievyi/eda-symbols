/*
 * tests/test_symbols.cpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Headless Direct2D verification test suite for eda-symbols.
 * Validates null-pointer safety, bounding box calculations,
 * and wire connection pin metadata for both IEEE and IEC symbols.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d2d1.h>
#include <wrl/client.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include <eda/symbols/iec.hpp>
#include <eda/symbols/ieee.hpp>
#include <eda/symbols/types.hpp>

#pragma comment(lib, "d2d1.lib")

using Microsoft::WRL::ComPtr;
using namespace eda::cad::symbols;

namespace {

static int g_failed_assertions = 0;

void assert_true(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "  [FAIL] " << message << "\n";
        ++g_failed_assertions;
    }
}

bool float_near(float a, float b) noexcept {
    return std::fabs(a - b) < 0.001f;
}

void test_null_safety(ID2D1Factory* factory) {
    std::cout << "[TEST] Null safety enforcement...\n";

    ID2D1PathGeometry* geom = nullptr;

    assert_true(
        IeeeSymbolFactory::BuildCircuitBreaker(nullptr, &geom) == E_INVALIDARG,
        "BuildCircuitBreaker must fail with E_INVALIDARG when factory is null"
    );
    assert_true(
        IeeeSymbolFactory::BuildDisconnectSwitch(nullptr, SwitchState::Open, &geom) == E_INVALIDARG,
        "BuildDisconnectSwitch must fail with E_INVALIDARG when factory is null"
    );
    assert_true(
        IeeeSymbolFactory::BuildFuse(nullptr, &geom) == E_INVALIDARG,
        "BuildFuse must fail with E_INVALIDARG when factory is null"
    );
    assert_true(
        IeeeSymbolFactory::BuildTransformer2W(nullptr, &geom) == E_INVALIDARG,
        "BuildTransformer2W must fail with E_INVALIDARG when factory is null"
    );
    assert_true(
        IeeeSymbolFactory::BuildEarthGround(nullptr, &geom) == E_INVALIDARG,
        "BuildEarthGround must fail with E_INVALIDARG when factory is null"
    );

    assert_true(
        IeeeSymbolFactory::BuildCircuitBreaker(factory, nullptr) == E_INVALIDARG,
        "BuildCircuitBreaker must fail with E_INVALIDARG when geometry pointer is null"
    );
    assert_true(
        IeeeSymbolFactory::BuildDisconnectSwitch(factory, SwitchState::Open, nullptr) == E_INVALIDARG,
        "BuildDisconnectSwitch must fail with E_INVALIDARG when geometry pointer is null"
    );

    SymbolDefinition breaker_def = IeeeSymbolFactory::CreateCircuitBreaker(nullptr);
    assert_true(!breaker_def, "CreateCircuitBreaker(nullptr) must evaluate to false");
    assert_true(breaker_def.geometry == nullptr, "CreateCircuitBreaker(nullptr) geometry must be null");

    SymbolDefinition gnd_def = IeeeSymbolFactory::CreateEarthGround(nullptr);
    assert_true(!gnd_def, "CreateEarthGround(nullptr) must evaluate to false");
    assert_true(gnd_def.pins.empty(), "Failed creation must return empty pin vector");
}

void test_circuit_breaker(ID2D1Factory* factory) {
    std::cout << "[TEST] IEEE Circuit Breaker...\n";

    SymbolDefinition def = IeeeSymbolFactory::CreateCircuitBreaker(factory);
    assert_true(static_cast<bool>(def), "Circuit breaker definition must be valid");
    assert_true(def.standard == Standard::IEEE, "Standard must be IEEE");
    assert_true(def.topology == Topology::InLine, "Topology must be InLine");

    assert_true(def.pins.size() == 2, "Circuit breaker must contain exactly 2 pins");
    if (def.pins.size() == 2) {
        assert_true(def.pins[0].name == "LINE", "Pin 0 name must be 'LINE'");
        assert_true(
            float_near(def.pins[0].position.x, 0.0f) && float_near(def.pins[0].position.y, -20.0f),
            "Pin 0 position must be (0, -20)"
        );
        assert_true(float_near(def.pins[0].normal.y, -1.0f), "Pin 0 normal must point upward (0, -1)");

        assert_true(def.pins[1].name == "LOAD", "Pin 1 name must be 'LOAD'");
        assert_true(
            float_near(def.pins[1].position.x, 0.0f) && float_near(def.pins[1].position.y, 20.0f),
            "Pin 1 position must be (0, 20)"
        );
        assert_true(float_near(def.pins[1].normal.y, 1.0f), "Pin 1 normal must point downward (0, 1)");
    }

    assert_true(def.bounds.left <= -10.0f, "Bounds left must be <= -10");
    assert_true(def.bounds.right >= 10.0f, "Bounds right must be >= 10");
    assert_true(def.bounds.top <= -20.0f, "Bounds top must be <= -20");
    assert_true(def.bounds.bottom >= 20.0f, "Bounds bottom must be >= 20");
}

void test_disconnect_switch(ID2D1Factory* factory) {
    std::cout << "[TEST] IEEE Disconnect Switch (Open/Closed)...\n";

    SymbolDefinition open_sw = IeeeSymbolFactory::CreateDisconnectSwitch(factory, SwitchState::Open);
    SymbolDefinition closed_sw = IeeeSymbolFactory::CreateDisconnectSwitch(factory, SwitchState::Closed);

    assert_true(static_cast<bool>(open_sw), "Open switch must produce valid geometry");
    assert_true(static_cast<bool>(closed_sw), "Closed switch must produce valid geometry");

    assert_true(
        open_sw.bounds.right > closed_sw.bounds.right,
        "Open switch must have a wider right bound due to the thrown blade"
    );

    assert_true(open_sw.pins.size() == 2, "Switch must contain 2 pins");
    if (open_sw.pins.size() == 2) {
        assert_true(open_sw.pins[0].name == "HINGE", "Pin 0 must be 'HINGE'");
        assert_true(open_sw.pins[1].name == "JAW", "Pin 1 must be 'JAW'");
    }
}

void test_fuse(ID2D1Factory* factory) {
    std::cout << "[TEST] IEEE Power Fuse...\n";

    SymbolDefinition def = IeeeSymbolFactory::CreateFuse(factory);
    assert_true(static_cast<bool>(def), "Fuse definition must be valid");
    assert_true(def.pins.size() == 2, "Fuse must contain 2 pins");

    assert_true(def.bounds.left < -5.0f, "Fuse bounds left must accommodate S-curve");
    assert_true(def.bounds.right > 5.0f, "Fuse bounds right must accommodate S-curve");
}

void test_transformer_2w(ID2D1Factory* factory) {
    std::cout << "[TEST] IEEE 2-Winding Transformer...\n";

    SymbolDefinition def = IeeeSymbolFactory::CreateTransformer2W(factory);
    assert_true(static_cast<bool>(def), "Transformer definition must be valid");
    assert_true(def.pins.size() == 2, "Transformer must contain 2 pins (PRI, SEC)");

    if (def.pins.size() == 2) {
        assert_true(def.pins[0].name == "PRI", "Primary pin must be 'PRI'");
        assert_true(float_near(def.pins[0].position.y, -25.0f), "PRI pin must be at y = -25");
        assert_true(def.pins[1].name == "SEC", "Secondary pin must be 'SEC'");
        assert_true(float_near(def.pins[1].position.y, 25.0f), "SEC pin must be at y = 25");
    }

    assert_true(def.bounds.left <= -6.0f, "Transformer lobes must bulge left");
}

void test_earth_ground(ID2D1Factory* factory) {
    std::cout << "[TEST] IEEE Earth Ground...\n";

    SymbolDefinition def = IeeeSymbolFactory::CreateEarthGround(factory);
    assert_true(static_cast<bool>(def), "Earth ground definition must be valid");
    assert_true(def.topology == Topology::Shunt, "Topology must be Shunt");
    assert_true(def.pins.size() == 1, "Ground must contain exactly 1 pin");

    if (!def.pins.empty()) {
        assert_true(def.pins[0].name == "GND", "Pin name must be 'GND'");
        assert_true(float_near(def.pins[0].position.y, -15.0f), "GND pin must be at y = -15");
    }

    assert_true(
        def.bounds.left <= -12.0f && def.bounds.right >= 12.0f,
        "Ground horizontal plate bounds must span at least [-12, 12]"
    );
}

void test_iec_symbols(ID2D1Factory* factory) {
    std::cout << "[TEST] IEC 60617 Symbols...\n";

    SymbolDefinition cb = IecSymbolFactory::CreateCircuitBreaker(factory);
    assert_true(static_cast<bool>(cb) && cb.standard == Standard::IEC, "IEC Circuit Breaker must be valid");

    SymbolDefinition sw_open = IecSymbolFactory::CreateDisconnectSwitch(factory, SwitchState::Open);
    SymbolDefinition sw_closed = IecSymbolFactory::CreateDisconnectSwitch(factory, SwitchState::Closed);
    assert_true(static_cast<bool>(sw_open) && static_cast<bool>(sw_closed), "IEC Disconnect Switches must be valid");

    SymbolDefinition fuse = IecSymbolFactory::CreateFuse(factory);
    assert_true(static_cast<bool>(fuse) && fuse.pins.size() == 2, "IEC Fuse must be valid with 2 pins");

    SymbolDefinition xfmr = IecSymbolFactory::CreateTransformer2W(factory);
    assert_true(static_cast<bool>(xfmr) && xfmr.pins.size() == 2, "IEC Transformer must be valid with 2 pins");

    SymbolDefinition gnd = IecSymbolFactory::CreateEarthGround(factory);
    assert_true(static_cast<bool>(gnd) && gnd.topology == Topology::Shunt, "IEC Earth Ground must be valid Shunt");
}

} /* namespace */

int main() {
    std::cout << "========================================\n";
    std::cout << " Running eda-symbols Verification Suite \n";
    std::cout << "========================================\n";

    ComPtr<ID2D1Factory> factory;
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        factory.GetAddressOf()
    );
    if (FAILED(hr)) {
        std::cerr << "[FATAL] Failed to initialize Direct2D factory. hr = 0x"
                  << std::hex << hr << "\n";
        return 1;
    }

    test_null_safety(factory.Get());
    test_circuit_breaker(factory.Get());
    test_disconnect_switch(factory.Get());
    test_fuse(factory.Get());
    test_transformer_2w(factory.Get());
    test_earth_ground(factory.Get());
    test_iec_symbols(factory.Get());

    std::cout << "========================================\n";
    if (g_failed_assertions == 0) {
        std::cout << " [PASS] All symbol verification tests passed.\n";
        std::cout << "========================================\n";
        return 0;
    }

    std::cerr << " [FAIL] " << g_failed_assertions << " test assertion(s) failed.\n";
    std::cout << "========================================\n";
    return 1;
}
