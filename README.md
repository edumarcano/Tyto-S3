# Tyto

Tyto is a standalone room-climate sensing device built around an ESP32-S3.

The name comes from *Tyto*, the genus that includes barn owls, reflecting the idea of a small, quiet observer of its environment.

The project begins by measuring temperature and relative humidity, calculating derived values such as dew point, and recording how room conditions change over time. The first experiment will examine whether these measurements can identify and describe possible air-conditioning cycles.

Later work may explore additional sensors, local interaction, and TinyML for a narrowly defined task when it provides a measurable improvement over simpler rules.

Tyto is designed to remain useful without cloud services or another application. A future integration with [APEX](https://github.com/edumarcano/APEX) may provide visualization, history, and access to device data.

## Current Status

History and configuration are complete for v0.4.0.

Tyto reads temperature and relative humidity from an AM2302 at a configurable interval, validates the measurements, and calculates derived values including dew point and short-term temperature trend.

The measurement interval can be changed over serial and is preserved across resets and power cycles.

Valid climate measurements are also stored locally on the ESP32-S3 using LittleFS. History records include raw sensor observations, derived climate values, boot-session information, and elapsed time. Stored history can be inspected over serial and exported as CSV for later analysis.

Tyto continues to operate as a standalone device without Wi-Fi, cloud services, or APEX.

See [AM2302 Sensor Setup](docs/am2302-setup.md) for wiring, sensor behavior, and measurement configuration.

See [Climate History](docs/climate-history.md) for local storage, CSV format, history rotation, and export instructions.

## Roadmap

See [docs/roadmap.md](docs/roadmap.md) for planned releases.