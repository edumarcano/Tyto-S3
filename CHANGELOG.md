# Tyto-S3 Changelog

---

## v0.3.0 — Local climate monitoring

**Released:** August 10, 2026

### Added

* Configurable environmental measurement interval with non-blocking
  `millis()`-based scheduling
* Startup reporting of the configured measurement interval
* Dew-point calculation for valid temperature and relative-humidity readings
* Basic temperature trend reporting using recent valid measurements
* Warming, cooling, stable, and collecting trend states
* Clear fresh, stale, and unavailable data-status reporting
* Tracking and reporting of the age and values of the last valid measurement

### Changed

* Preserved the last valid environmental measurement when a new sensor read
  fails without presenting the retained data as fresh
* Kept failed and invalid readings out of temperature trend calculations
* Reorganized climate-monitoring responsibilities to make timing,
  validation, calculations, state tracking, and serial reporting easier
  to understand
* Updated the README and roadmap to reflect completion of local climate
  monitoring

---

## v0.2.0 — Environmental sensing

**Released:** July 29, 2026

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

---

## v0.1.0 — ESP32-S3 bring-up

**Released:** July 26, 2026

### Added

- Initial PlatformIO firmware for the ESP32-S3
- Serial communication and startup diagnostics
- Chip, flash, and PSRAM reporting
- Uptime and recurring heartbeat output