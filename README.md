# Tyto

Tyto is a standalone room-climate sensing device built around an ESP32-S3.

The name comes from *Tyto*, the genus that includes barn owls, reflecting the idea of a small, quiet observer of its environment.

The project begins by measuring temperature and relative humidity, calculating derived values such as dew point, and recording how room conditions change over time. The first experiment will examine whether these measurements can identify and describe possible air-conditioning cycles.

Later work may explore additional sensors, local interaction, and TinyML for a narrowly defined task when it provides a measurable improvement over simpler rules.

Tyto is designed to remain useful without cloud services or another application. A future integration with [APEX](https://github.com/edumarcano/APEX) may provide visualization, history, and access to device data.

## Current Status

ESP32-S3 hardware bring-up complete. Preparing environmental sensor integration.

## Roadmap

See [docs/roadmap.md](docs/roadmap.md) for planned releases.