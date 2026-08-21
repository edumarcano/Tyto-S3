# Tyto-S3 Changelog

---

## v0.5.0 - Room-climate experiment

**Unreleased**

### Added

- SHT31 temperature and relative-humidity sensing over I2C
- SHT31 wiring, setup, and hardware-validation documentation
- A 72-hour room-climate experiment with a 30-second measurement interval
- Python plots for temperature, relative humidity, and dew point
- Offline temperature-cycle analysis using measured room behavior
- Experiment notes covering observations, candidate cycles, uncertainty, and sensor-placement limits

### Changed

- Replaced the AM2302 sensor with an SHT31
- Split the firmware into smaller climate, configuration, history, serial-command, sensor, and telemetry modules
- Updated the README, roadmap, and reference documentation to match the completed v0.5.0 work
- Clarified that candidate cooling and recovery cycles are offline analysis results, not confirmed AC activity or an on-device detector

---

## v0.4.0 - History and configuration

**Released:** August 11, 2026

### Added

- Persistent measurement interval configuration using ESP32 NVS
- Serial command for changing the measurement interval at runtime
- Boot IDs for distinguishing measurements across restarts
- LittleFS-backed local climate history
- CSV history with temperature, relative humidity, dew point, and temperature trend data
- Bounded history rotation with one previous history file retained
- Serial commands for viewing and exporting stored history
- Climate-history documentation covering storage, CSV format, configuration, and export
- Repository MIT license

### Changed

- Restored the configured measurement interval after resets and power cycles
- Stored only fresh, validated sensor measurements in history
- Kept raw sensor values separate from derived climate values in stored history
- Added boot-session context to elapsed-time history records

---

## v0.3.0 - Local climate monitoring

**Released:** August 10, 2026

### Added

- Configurable environmental measurement interval with non-blocking
  `millis()`-based scheduling
- Startup reporting of the configured measurement interval
- Dew-point calculation for valid temperature and relative-humidity readings
- Basic temperature trend reporting using recent valid measurements
- Warming, cooling, stable, and collecting trend states
- Clear fresh, stale, and unavailable data-status reporting
- Tracking and reporting of the age and values of the last valid measurement

### Changed

- Preserved the last valid environmental measurement when a new sensor read
  fails without presenting the retained data as fresh
- Kept failed and invalid readings out of temperature trend calculations
- Reorganized climate-monitoring responsibilities to make timing,
  validation, calculations, state tracking, and serial reporting easier
  to understand
- Updated the README and roadmap to reflect completion of local climate
  monitoring

---

## v0.2.0 - Environmental sensing

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

## v0.1.0 - ESP32-S3 bring-up

**Released:** July 26, 2026

### Added

- Initial PlatformIO firmware for the ESP32-S3
- Serial communication and startup diagnostics
- Chip, flash, and PSRAM reporting
- Uptime and recurring heartbeat output
