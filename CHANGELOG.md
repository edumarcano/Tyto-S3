# Changelog

Notable changes to Tyto are documented here.

## v0.2.0 — Environmental sensing

Released: July 29, 2026

### Added

- AM2302 temperature and relative-humidity sensing on GPIO 4
- Validation for communication failures and out-of-range readings
- Structured serial output with uptime and sensor status
- AM2302 wiring, setup, and verification documentation
- PlatformIO dependency configuration for the DHT sensor library
- VS Code recommendation for the PlatformIO extension

### Changed

- Replaced the standalone heartbeat with periodic environmental
  measurement records
- Updated the README and roadmap to reflect completion of
  environmental sensing

## v0.1.0 — ESP32-S3 bring-up

Released: July 26, 2026

### Added

- Initial PlatformIO firmware for the ESP32-S3
- Serial communication and startup diagnostics
- Chip, flash, and PSRAM reporting
- Uptime and recurring heartbeat output