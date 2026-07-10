# Roadmap

Tyto follows semantic versioning. During early development, releases use the
`0.x.y` range while hardware behavior and interfaces may still change.

## v0.1.0 — Hardware bring-up

- Build and upload firmware to the ESP32-S3
- Add serial diagnostics
- Report basic device health

## v0.2.0 — Environmental sensing

- Read temperature and humidity
- Handle sensor initialization and read failures
- Produce structured local telemetry

## v0.3.0 — Standalone monitoring

- Add Wi-Fi connectivity
- Provide a local status page
- Detect sustained low-humidity conditions
- Support basic history or data export

## v0.4.0 — Environmental MVP

- Demonstrate reliable long-running operation
- Record real room-humidity trends
- Document the room-comfort experiment
- Add clear setup and usage instructions

## v0.5.0 — Workspace awareness

- Add presence sensing
- Track room and desk sessions
- Provide local reminders

## v0.6.0 — Physical interaction

- Add buttons or another local input
- Support workspace modes
- Implement at least one useful local automation

## v0.7.0 — Intelligent state detection

- Establish rule-based baselines
- Collect and label workspace-state data
- Evaluate TinyML where it provides measurable value
- Deploy embedded inference only if justified

## v0.8.0 — Integration-ready protocol

- Define a versioned telemetry schema
- Add device authentication
- Handle temporary receiver and network outages

## v1.0.0 — Stable standalone Tyto

- Provide reliable standalone operation
- Publish a stable, documented device interface
- Validate failure handling and long-running behavior
- Support external integrations without depending on them

## v1.1.0 — APEX integration

Integrate Tyto with [APEX](https://github.com/edumarcano/APEX), a local personal HUD and assistant built with FastAPI and React.

- Add authenticated telemetry ingestion to APEX
- Display workspace context in the APEX interface
- Expose Tyto data through APEX assistant tools