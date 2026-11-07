# Changelog

## v0.1.0-alpha (2026-Nov-06)

- Initial release.
- CMake integration via FetchContent.
- Basic IEEE and IEC schematic symbols:

| Component Added | Topology | Build Function | Create Function |
| --- | --- | --- | --- |
| **Circuit Breaker** | InLine (2-port) | `BuildCircuitBreaker` | `CreateCircuitBreaker` |
| **Disconnect Switch (Isolator)** | InLine (2-port) | `BuildDisconnectSwitch` | `CreateDisconnectSwitch` |
| **Power Fuse** | InLine (2-port) | `BuildFuse` | `CreateFuse` |
| **2-Winding Transformer (2W)** | InLine (2-port) | `BuildTransformer2W` | `CreateTransformer2W` |
| **Earth Ground** | Shunt (1-port) | `BuildEarthGround` | `CreateEarthGround` |
