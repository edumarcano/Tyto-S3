# Roadmap

Tyto is developed in small stages. Each release should leave the device working before new hardware or features are added.

## v0.1.0 - ESP32-S3 bring-up

**Status: Complete**

- Build and upload firmware with PlatformIO
- Confirm serial communication
- Report chip, flash, and PSRAM information
- Report uptime and a recurring heartbeat

## v0.2.0 - Environmental sensing

**Status: Complete**

- Connect the temperature and humidity sensor
- Read temperature and relative humidity
- Validate sensor readings
- Report initialization and communication failures
- Produce structured serial output
- Document the wiring and exact sensor used

## v0.3.0 - Local climate monitoring

**Status: Complete**

- Sample measurements at a configurable interval
- Calculate dew point
- Track basic trends and temperature change over a recent window
- Report sensor health and data quality
- Separate sensing, validation, calculations, and reporting in the firmware
- Continue operating without Wi-Fi or APEX

## v0.4.0 - History and configuration

**Status: Complete**

- Preserve important settings across restarts
- Store or export measurements for later analysis
- Include reliable elapsed-time information
- Preserve raw measurements separately from calculated values
- Document the measurement format

## v0.5.0 - Room-climate experiment

**Status: Complete**

- Collect a 72-hour room dataset during normal use
- Plot temperature, relative humidity, and dew point
- Inspect cooling, warming, and stable periods
- Define simple rules for candidate cooling and recovery cycles
- Measure cycle duration, temperature change, and recovery time
- Document the results, uncertainty, and sensor-placement limitations
- Record that the run did not include confirmed AC on/off times, so detected cycles are not treated as confirmed AC activity

## v0.6.0 - Reliability and local interface

- Report reset reasons and useful error counters
- Extend testing of sensor failure and recovery behavior
- Complete an extended unattended run
- Add one simple local status or configuration interface
- Keep local sensing independent of later network features

## v0.7.0 - External device interface

- Define a versioned telemetry format
- Include measurements, calculated state, device health, and firmware version
- Handle temporary network and receiver outages
- Add authentication for external telemetry
- Keep all core features working while disconnected
- Document the interface and compatibility rules

## v1.0.0 - Stable standalone Tyto

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
- Detecting unusual environmental changes

Before implementation:

- Define the event or state being detected
- Decide how examples will be labeled
- Collect real data
- Establish a simple rules-based comparison

### TinyML evaluation

Only if the focused sensing experiment produces a clear problem that rules do not handle well.

- Prepare and inspect a labeled dataset
- Train and evaluate a small model
- Compare the model with the simple rules
- Test on separate sessions or days
- Deploy the model to the ESP32-S3 only when it provides a useful improvement
- Measure inference time, memory use, flash use, and stability
- Document whether the model was accepted or rejected

### APEX integration

- Send authenticated Tyto telemetry to APEX
- Display current measurements and device health
- Present historical climate data and detected events
- Allow APEX tools to query Tyto state and history
- Keep Tyto fully functional when APEX is unavailable
