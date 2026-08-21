# Tyto

Tyto is a standalone room-climate sensing device built around an ESP32-S3.

The name comes from *Tyto*, the genus that includes barn owls, reflecting the idea of a small, quiet observer of its environment.

Tyto measures temperature and relative humidity, calculates dew point and short-term temperature trends, and stores measurements locally for later analysis.

The first room-climate experiment collected 72 hours of normal room data. Offline analysis found repeated short cooling and recovery cycles, but the run did not record confirmed AC on/off times, so those cycles are not treated as confirmed AC activity.

Later work may explore additional sensors, local interaction, and TinyML for a narrowly defined task when it provides a measurable improvement over simpler rules.

Tyto is designed to remain useful without cloud services or another application. A future integration with [APEX](https://github.com/edumarcano/APEX) may provide visualization, history, and access to device data.

## Current Status

The v0.5.0 room-climate experiment is complete and being prepared for release.

Tyto reads temperature and relative humidity from an SHT31 over I2C at a configurable interval. It validates each reading, calculates dew point, and tracks a short-term temperature trend.

The measurement interval can be changed over serial and is preserved across resets and power cycles.

Valid measurements are stored locally on the ESP32-S3 using LittleFS. Each history row includes the sensor readings, calculated climate values, a boot ID, and elapsed time. History can be inspected over serial and exported as CSV for analysis.

Tyto continues to operate as a standalone device without Wi-Fi, cloud services, or APEX.

See [SHT31 Sensor Setup](docs/sht31-setup.md) for wiring, sensor behavior, and measurement configuration.

See [Climate History](docs/climate-history.md) for local storage, CSV format, history rotation, and export instructions.

See [Room Climate Experiment](docs/room-climate-experiment.md) for the v0.5.0 experiment setup and run record. The plots and analysis notes are in [`analysis/`](analysis/).

## Roadmap

See [docs/roadmap.md](docs/roadmap.md) for planned releases.
