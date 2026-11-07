/*
 * tests/export_symbol_images.cpp
 *
 * SPDX-License-Identifier: MIT
 *
 * Headless WIC and Direct2D PNG exporter for eda-symbols.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <iostream>
#include <string>
#include <vector>

#include <eda/symbols/iec.hpp>
#include <eda/symbols/ieee.hpp>

using Microsoft::WRL::ComPtr;
using namespace eda::cad::symbols;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

constexpr UINT RENDER_WIDTH = 256;
constexpr UINT RENDER_HEIGHT = 256;
constexpr float SCALE_FACTOR = 3.0f;

void RenderAndSaveSymbol(
    const std::wstring& filename,
    const SymbolDefinition& def,
    IWICImagingFactory* wicFactory,
    ID2D1Factory* d2dFactory
) {
    if (!def) {
        std::wcerr << L"[ERROR] Invalid symbol geometry for " << filename << L"\n";
        return;
    }

    HRESULT hr;

    ComPtr<IWICBitmap> wicBitmap;
    hr = wicFactory->CreateBitmap(
        RENDER_WIDTH,
        RENDER_HEIGHT,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapCacheOnDemand,
        wicBitmap.GetAddressOf()
    );
    if (FAILED(hr)) return;

    ComPtr<ID2D1RenderTarget> rt;
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        0.0f, 0.0f,
        D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT
    );

    hr = d2dFactory->CreateWicBitmapRenderTarget(wicBitmap.Get(), &rtProps, rt.GetAddressOf());
    if (FAILED(hr)) return;

    ComPtr<ID2D1SolidColorBrush> strokeBrush;
    ComPtr<ID2D1SolidColorBrush> pinBrush;
    rt->CreateSolidColorBrush(D2D1::ColorF(0xCCCCCC), strokeBrush.GetAddressOf());
    rt->CreateSolidColorBrush(D2D1::ColorF(0x00FFFF), pinBrush.GetAddressOf());

    rt->BeginDraw();

    rt->Clear(D2D1::ColorF(0x1E1E1E));

    rt->SetTransform(
        D2D1::Matrix3x2F::Scale(SCALE_FACTOR, SCALE_FACTOR) *
        D2D1::Matrix3x2F::Translation(RENDER_WIDTH / 2.0f, RENDER_HEIGHT / 2.0f)
    );

    rt->DrawGeometry(def.geometry.Get(), strokeBrush.Get(), 1.5f);

    for (const auto& pin : def.pins) {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(pin.position, 1.5f, 1.5f);
        rt->FillEllipse(&ellipse, pinBrush.Get());
    }

    hr = rt->EndDraw();
    if (FAILED(hr)) return;

    ComPtr<IWICStream> stream;
    hr = wicFactory->CreateStream(stream.GetAddressOf());
    if (FAILED(hr)) return;

    hr = stream->InitializeFromFilename(filename.c_str(), GENERIC_WRITE);
    if (FAILED(hr)) return;

    ComPtr<IWICBitmapEncoder> encoder;
    hr = wicFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf());
    if (FAILED(hr)) return;

    hr = encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache);
    if (FAILED(hr)) return;

    ComPtr<IWICBitmapFrameEncode> frame;
    hr = encoder->CreateNewFrame(frame.GetAddressOf(), nullptr);
    if (FAILED(hr)) return;

    hr = frame->Initialize(nullptr);
    if (FAILED(hr)) return;

    hr = frame->SetSize(RENDER_WIDTH, RENDER_HEIGHT);
    if (FAILED(hr)) return;

    WICPixelFormatGUID format = GUID_WICPixelFormat32bppPBGRA;
    hr = frame->SetPixelFormat(&format);
    if (FAILED(hr)) return;

    hr = frame->WriteSource(wicBitmap.Get(), nullptr);
    if (FAILED(hr)) return;

    hr = frame->Commit();
    if (FAILED(hr)) return;

    hr = encoder->Commit();
    if (FAILED(hr)) return;

    std::wcout << L"Exported: " << filename << L"\n";
}

int main() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM.\n";
        return 1;
    }

    {
        ComPtr<IWICImagingFactory> wicFactory;
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(wicFactory.GetAddressOf())
        );
        if (FAILED(hr)) {
            std::cerr << "Failed to create WIC Imaging Factory.\n";
            CoUninitialize();
            return 1;
        }

        ComPtr<ID2D1Factory> d2dFactory;
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf());
        if (FAILED(hr)) {
            std::cerr << "Failed to create Direct2D Factory.\n";
            CoUninitialize();
            return 1;
        }

        std::wcout << L"Generating SLD Symbol PNGs...\n";

        RenderAndSaveSymbol(L"ieee_circuit_breaker.png", IeeeSymbolFactory::CreateCircuitBreaker(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"ieee_disconnect_open.png", IeeeSymbolFactory::CreateDisconnectSwitch(d2dFactory.Get(), SwitchState::Open), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"ieee_disconnect_closed.png", IeeeSymbolFactory::CreateDisconnectSwitch(d2dFactory.Get(), SwitchState::Closed), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"ieee_fuse.png", IeeeSymbolFactory::CreateFuse(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"ieee_transformer_2w.png", IeeeSymbolFactory::CreateTransformer2W(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"ieee_earth_ground.png", IeeeSymbolFactory::CreateEarthGround(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());

        RenderAndSaveSymbol(L"iec_circuit_breaker.png", IecSymbolFactory::CreateCircuitBreaker(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"iec_disconnect_open.png", IecSymbolFactory::CreateDisconnectSwitch(d2dFactory.Get(), SwitchState::Open), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"iec_disconnect_closed.png", IecSymbolFactory::CreateDisconnectSwitch(d2dFactory.Get(), SwitchState::Closed), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"iec_fuse.png", IecSymbolFactory::CreateFuse(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"iec_transformer_2w.png", IecSymbolFactory::CreateTransformer2W(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
        RenderAndSaveSymbol(L"iec_earth_ground.png", IecSymbolFactory::CreateEarthGround(d2dFactory.Get()), wicFactory.Get(), d2dFactory.Get());
    }

    CoUninitialize();
    return 0;
}
