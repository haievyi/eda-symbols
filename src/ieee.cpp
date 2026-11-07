/*
 * src/ieee.cpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Implementation of IEEE / ANSI schematic symbology.
 */

#include <eda/symbols/ieee.hpp>

#include <array>

using Microsoft::WRL::ComPtr;

namespace eda::cad::symbols {

namespace {

/**
 * populate_bounds() - Query geometry bounding box including hollow figures
 * @geometry: Direct2D path geometry instance
 * @bounds:   Output target receiving the computed rectangle
 */
void populate_bounds(ID2D1PathGeometry* geometry, D2D1_RECT_F& bounds) noexcept {
    if (!geometry) {
        bounds = {0.0f, 0.0f, 0.0f, 0.0f};
        return;
    }

    if (FAILED(geometry->GetWidenedBounds(0.0f, nullptr, nullptr, &bounds))) {
        bounds = {0.0f, 0.0f, 0.0f, 0.0f};
    }
}

} /* namespace */

HRESULT IeeeSymbolFactory::BuildCircuitBreaker(
    ID2D1Factory* factory,
    ID2D1PathGeometry** geometry
) noexcept {
    if (!factory || !geometry) {
        return E_INVALIDARG;
    }

    *geometry = nullptr;

    ComPtr<ID2D1PathGeometry> path;
    HRESULT hr = factory->CreatePathGeometry(path.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<ID2D1GeometrySink> sink;
    hr = path->Open(sink.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -20.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, -10.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-10.0f, -10.0f}, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1_POINT_2F{10.0f, -10.0f});
    sink->AddLine(D2D1_POINT_2F{10.0f, 10.0f});
    sink->AddLine(D2D1_POINT_2F{-10.0f, 10.0f});
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);

    sink->BeginFigure(D2D1_POINT_2F{0.0f, 10.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 20.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IeeeSymbolFactory::CreateCircuitBreaker(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEEE;
    def.topology = Topology::InLine;

    HRESULT hr = BuildCircuitBreaker(factory, def.geometry.GetAddressOf());
    if (FAILED(hr)) {
        return def;
    }

    populate_bounds(def.geometry.Get(), def.bounds);

    def.pins = {
        TerminalPin{
            .name = "LINE",
            .position = {0.0f, -20.0f},
            .normal = {0.0f, -1.0f},
        },
        TerminalPin{
            .name = "LOAD",
            .position = {0.0f, 20.0f},
            .normal = {0.0f, 1.0f},
        },
    };

    return def;
}

HRESULT IeeeSymbolFactory::BuildDisconnectSwitch(
    ID2D1Factory* factory,
    SwitchState state,
    ID2D1PathGeometry** geometry
) noexcept {
    if (!factory || !geometry) {
        return E_INVALIDARG;
    }

    *geometry = nullptr;

    ComPtr<ID2D1PathGeometry> path;
    HRESULT hr = factory->CreatePathGeometry(path.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<ID2D1GeometrySink> sink;
    hr = path->Open(sink.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -20.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, -10.0f});

    if (state == SwitchState::Closed) {
        sink->AddLine(D2D1_POINT_2F{0.0f, 10.0f});
    } else {
        sink->AddLine(D2D1_POINT_2F{10.0f, 6.0f});
    }
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-4.0f, 10.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{4.0f, 10.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{0.0f, 10.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 20.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IeeeSymbolFactory::CreateDisconnectSwitch(
    ID2D1Factory* factory,
    SwitchState state
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEEE;
    def.topology = Topology::InLine;

    HRESULT hr = BuildDisconnectSwitch(factory, state, def.geometry.GetAddressOf());
    if (FAILED(hr)) {
        return def;
    }

    populate_bounds(def.geometry.Get(), def.bounds);

    def.pins = {
        TerminalPin{
            .name = "HINGE",
            .position = {0.0f, -20.0f},
            .normal = {0.0f, -1.0f},
        },
        TerminalPin{
            .name = "JAW",
            .position = {0.0f, 20.0f},
            .normal = {0.0f, 1.0f},
        },
    };

    return def;
}

HRESULT IeeeSymbolFactory::BuildFuse(
    ID2D1Factory* factory,
    ID2D1PathGeometry** geometry
) noexcept {
    if (!factory || !geometry) {
        return E_INVALIDARG;
    }

    *geometry = nullptr;

    ComPtr<ID2D1PathGeometry> path;
    HRESULT hr = factory->CreatePathGeometry(path.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<ID2D1GeometrySink> sink;
    hr = path->Open(sink.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -20.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, -10.0f});

    /*
     * Smooth S-curve transition representing the IEEE fuse element.
     * Control points at x = +/-18.0f produce a true curve excursion of +/-5.20 DIPs.
     */
    D2D1_BEZIER_SEGMENT fuse_curve{
        .point1 = D2D1_POINT_2F{-18.0f, -5.0f},
        .point2 = D2D1_POINT_2F{18.0f, 5.0f},
        .point3 = D2D1_POINT_2F{0.0f, 10.0f},
    };
    sink->AddBezier(&fuse_curve);

    sink->AddLine(D2D1_POINT_2F{0.0f, 20.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IeeeSymbolFactory::CreateFuse(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEEE;
    def.topology = Topology::InLine;

    HRESULT hr = BuildFuse(factory, def.geometry.GetAddressOf());
    if (FAILED(hr)) {
        return def;
    }

    populate_bounds(def.geometry.Get(), def.bounds);

    def.pins = {
        TerminalPin{
            .name = "LINE",
            .position = {0.0f, -20.0f},
            .normal = {0.0f, -1.0f},
        },
        TerminalPin{
            .name = "LOAD",
            .position = {0.0f, 20.0f},
            .normal = {0.0f, 1.0f},
        },
    };

    return def;
}

HRESULT IeeeSymbolFactory::BuildTransformer2W(
    ID2D1Factory* factory,
    ID2D1PathGeometry** geometry
) noexcept {
    if (!factory || !geometry) {
        return E_INVALIDARG;
    }

    *geometry = nullptr;

    ComPtr<ID2D1PathGeometry> path;
    HRESULT hr = factory->CreatePathGeometry(path.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<ID2D1GeometrySink> sink;
    hr = path->Open(sink.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    constexpr std::array<float, 5> kLobeX = {-12.0f, -6.0f, 0.0f, 6.0f, 12.0f};
    constexpr float kPriBaseY = -8.0f;
    constexpr float kPriBulgeY = -2.0f;
    constexpr float kSecBaseY = 8.0f;
    constexpr float kSecBulgeY = 2.0f;

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -25.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, kPriBaseY});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{kLobeX[0], kPriBaseY}, D2D1_FIGURE_BEGIN_HOLLOW);
    for (std::size_t i = 0; i < 4; ++i) {
        D2D1_BEZIER_SEGMENT lobe{
            .point1 = D2D1_POINT_2F{kLobeX[i], kPriBulgeY},
            .point2 = D2D1_POINT_2F{kLobeX[i + 1], kPriBulgeY},
            .point3 = D2D1_POINT_2F{kLobeX[i + 1], kPriBaseY},
        };
        sink->AddBezier(&lobe);
    }
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{kLobeX[0], kSecBaseY}, D2D1_FIGURE_BEGIN_HOLLOW);
    for (std::size_t i = 0; i < 4; ++i) {
        D2D1_BEZIER_SEGMENT lobe{
            .point1 = D2D1_POINT_2F{kLobeX[i], kSecBulgeY},
            .point2 = D2D1_POINT_2F{kLobeX[i + 1], kSecBulgeY},
            .point3 = D2D1_POINT_2F{kLobeX[i + 1], kSecBaseY},
        };
        sink->AddBezier(&lobe);
    }
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{0.0f, kSecBaseY}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 25.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IeeeSymbolFactory::CreateTransformer2W(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEEE;
    def.topology = Topology::InLine;

    HRESULT hr = BuildTransformer2W(factory, def.geometry.GetAddressOf());
    if (FAILED(hr)) {
        return def;
    }

    populate_bounds(def.geometry.Get(), def.bounds);

    def.pins = {
        TerminalPin{
            .name = "PRI",
            .position = {0.0f, -25.0f},
            .normal = {0.0f, -1.0f},
        },
        TerminalPin{
            .name = "SEC",
            .position = {0.0f, 25.0f},
            .normal = {0.0f, 1.0f},
        },
    };

    return def;
}

HRESULT IeeeSymbolFactory::BuildEarthGround(
    ID2D1Factory* factory,
    ID2D1PathGeometry** geometry
) noexcept {
    if (!factory || !geometry) {
        return E_INVALIDARG;
    }

    *geometry = nullptr;

    ComPtr<ID2D1PathGeometry> path;
    HRESULT hr = factory->CreatePathGeometry(path.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<ID2D1GeometrySink> sink;
    hr = path->Open(sink.GetAddressOf());
    if (FAILED(hr)) {
        return hr;
    }

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -15.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 0.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-12.0f, 0.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{12.0f, 0.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-7.0f, 4.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{7.0f, 4.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-2.0f, 8.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{2.0f, 8.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IeeeSymbolFactory::CreateEarthGround(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEEE;
    def.topology = Topology::Shunt;

    HRESULT hr = BuildEarthGround(factory, def.geometry.GetAddressOf());
    if (FAILED(hr)) {
        return def;
    }

    populate_bounds(def.geometry.Get(), def.bounds);

    def.pins = {
        TerminalPin{
            .name = "GND",
            .position = {0.0f, -15.0f},
            .normal = {0.0f, -1.0f},
        },
    };

    return def;
}

} /* namespace eda::cad::symbols */
