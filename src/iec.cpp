/*
 * src/iec.cpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Implementation of IEC schematic symbology.
 */

#include <eda/symbols/iec.hpp>

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

/**
 * add_circle_figure() - Append a closed circle figure to a geometry sink
 * @sink:   Active Direct2D geometry sink
 * @center: Center point (cx, cy) of the circle
 * @radius: Radius of the circle in DIPs
 * @begin:  Whether the circle should be hollow or filled
 */
void add_circle_figure(
    ID2D1GeometrySink* sink,
    D2D1_POINT_2F center,
    float radius,
    D2D1_FIGURE_BEGIN begin
) noexcept {
    const D2D1_POINT_2F top{center.x, center.y - radius};
    const D2D1_POINT_2F bottom{center.x, center.y + radius};
    const D2D1_SIZE_F size{radius, radius};

    sink->BeginFigure(top, begin);

    D2D1_ARC_SEGMENT right_half{
        .point = bottom,
        .size = size,
        .rotationAngle = 0.0f,
        .sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE,
        .arcSize = D2D1_ARC_SIZE_SMALL,
    };
    sink->AddArc(&right_half);

    D2D1_ARC_SEGMENT left_half{
        .point = top,
        .size = size,
        .rotationAngle = 0.0f,
        .sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE,
        .arcSize = D2D1_ARC_SIZE_SMALL,
    };
    sink->AddArc(&left_half);

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
}

} /* namespace */

HRESULT IecSymbolFactory::BuildCircuitBreaker(
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
    sink->AddLine(D2D1_POINT_2F{0.0f, -12.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-6.0f, -12.0f}, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1_POINT_2F{6.0f, -12.0f});
    sink->AddLine(D2D1_POINT_2F{6.0f, 12.0f});
    sink->AddLine(D2D1_POINT_2F{-6.0f, 12.0f});
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);

    sink->BeginFigure(D2D1_POINT_2F{-4.0f, 6.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{4.0f, -6.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-3.5f, -4.5f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{3.5f, 2.5f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{0.0f, 12.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 20.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IecSymbolFactory::CreateCircuitBreaker(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEC;
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

HRESULT IecSymbolFactory::BuildDisconnectSwitch(
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
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->BeginFigure(D2D1_POINT_2F{-4.0f, -10.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{4.0f, -10.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    add_circle_figure(sink.Get(), D2D1_POINT_2F{0.0f, 10.0f}, 2.0f, D2D1_FIGURE_BEGIN_HOLLOW);

    if (state == SwitchState::Closed) {
        sink->BeginFigure(D2D1_POINT_2F{0.0f, 8.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddLine(D2D1_POINT_2F{0.0f, -10.0f});
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    } else {
        sink->BeginFigure(D2D1_POINT_2F{1.0f, 8.2f}, D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddLine(D2D1_POINT_2F{10.0f, -6.0f});
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    }

    sink->BeginFigure(D2D1_POINT_2F{0.0f, 12.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 20.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IecSymbolFactory::CreateDisconnectSwitch(
    ID2D1Factory* factory,
    SwitchState state
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEC;
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

HRESULT IecSymbolFactory::BuildFuse(
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

    sink->BeginFigure(D2D1_POINT_2F{-5.0f, -10.0f}, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(D2D1_POINT_2F{5.0f, -10.0f});
    sink->AddLine(D2D1_POINT_2F{5.0f, 10.0f});
    sink->AddLine(D2D1_POINT_2F{-5.0f, 10.0f});
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -20.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 20.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IecSymbolFactory::CreateFuse(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEC;
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

HRESULT IecSymbolFactory::BuildTransformer2W(
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

    sink->BeginFigure(D2D1_POINT_2F{0.0f, -25.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, -14.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    add_circle_figure(sink.Get(), D2D1_POINT_2F{0.0f, -5.0f}, 9.0f, D2D1_FIGURE_BEGIN_HOLLOW);

    add_circle_figure(sink.Get(), D2D1_POINT_2F{0.0f, 5.0f}, 9.0f, D2D1_FIGURE_BEGIN_HOLLOW);

    sink->BeginFigure(D2D1_POINT_2F{0.0f, 14.0f}, D2D1_FIGURE_BEGIN_HOLLOW);
    sink->AddLine(D2D1_POINT_2F{0.0f, 25.0f});
    sink->EndFigure(D2D1_FIGURE_END_OPEN);

    hr = sink->Close();
    if (SUCCEEDED(hr)) {
        *geometry = path.Detach();
    }

    return hr;
}

SymbolDefinition IecSymbolFactory::CreateTransformer2W(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEC;
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

HRESULT IecSymbolFactory::BuildEarthGround(
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

SymbolDefinition IecSymbolFactory::CreateEarthGround(
    ID2D1Factory* factory
) noexcept {
    SymbolDefinition def{};
    def.standard = Standard::IEC;
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
