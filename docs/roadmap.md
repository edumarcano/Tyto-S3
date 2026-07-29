# Roadmap

Tyto is developed in small stages. Each release should leave the device working before new hardware or features are added.

## v0.1.0 — ESP32-S3 bring-up

**Status: Complete**

- Build and upload firmware with PlatformIO
- Confirm serial communication
- Report chip, flash, and PSRAM information
- Report uptime and a recurring heartbeat

## v0.2.0 — Environmental sensing

**Status: Complete**

- Connect the temperature and humidity sensor
- Read temperature and relative humidity
- Validate sensor readings
- Report initialization and communication failures
- Produce structured serial output
- Document the wiring and exact sensor used

## v0.3.0 — Local climate monitoring

- Sample measurements at a configurable interval
- Calculate dew point
- Track basic trends and rates of change
- Report sensor health and data quality
- Separate sensing, validation, calculations, and reporting in the firmware
- Continue operating without Wi-Fi or APEX

## v0.4.0 — History and configuration

- Preserve important settings across restarts
- Store or export measurements for later analysis
- Include timestamps or reliable elapsed-time information
- Preserve raw measurements separately from derived values
- Document the measurement format

## v0.5.0 — Room-climate experiment

- Collect a multi-day room dataset
- Plot temperature, relative humidity, and dew point
- Identify cooling, warming, and stable periods
- Mark known AC activity during selected sessions
- Build a rule-based detector for possible AC cycles
- Measure event duration, temperature change, and recovery time
- Document results, uncertainty, and sensor-placement limitations

## v0.6.0 — Reliability and local interface

- Recover from temporary sensor failures
- Report reset reasons and useful error counters
- Test sensor disconnection and reconnection
- Complete an extended unattended run
- Add one simple local status or configuration interface
- Keep local sensing active during network failures

## v0.7.0 — External device interface

- Define a versioned telemetry format
- Include measurements, derived state, device health, and firmware version
- Handle temporary network and receiver outages
- Add authentication for external telemetry
- Keep all core features working while disconnected
- Document the interface and compatibility rules

## v1.0.0 — Stable standalone Tyto

- Operate reliably for an extended period
- Measure and summarize room-climate conditions
- Preserve useful settings and history
- Expose clear health and failure information
- Provide a practical local interface
- Include reproducible build, wiring, setup, and usage documentation
- Document the room-climate experiment and known limitations
- Support external integrations without requiring them

---

## Possible later work

These ideas are outside the current release path and may be explored after the standalone version is complete.

### Focused sensing experiment

Explore one additional sensing or interaction task with a clear purpose.

Possible directions include:

- A second temperature measurement point
- Room or desk presence
- Tap or gesture input
- Environmental anomaly detection

Before implementation:

- Define the event or state being detected
- Define how examples will be labeled
- Collect real data
- Establish a simple rule-based baseline

### TinyML evaluation

Only if the focused sensing experiment produces a clear problem that rules do not handle well.

- Prepare and inspect a labeled dataset
- Train and evaluate a small model
- Compare the model with the rule-based baseline
- Test on separate sessions or days
- Deploy the model to the ESP32-S3 only when it provides a useful improvement
- Measure inference time, memory use, flash use, and stability
- Document whether the model was accepted or rejected

### APEX integration

- Add authenticated Tyto telemetry ingestion to APEX
- Display current measurements and device health
- Present historical climate data and detected events
- Allow APEX tools to query Tyto state and history
- Keep Tyto fully functional when APEX is unavailable