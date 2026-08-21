# Tyto

Tyto is a standalone room-climate sensing device built around an ESP32-S3.

The name comes from *Tyto*, the genus that includes barn owls, reflecting the idea of a small, quiet observer of its environment.

Tyto measures temperature and relative humidity, calculates dew point and short-term temperature trends, and stores measurements locally for later analysis.

The first room-climate experiment collected 72 hours of normal room data. Offline analysis found repeated short cooling and recovery cycles, but the run did not record confirmed AC on/off times, so those cycles are not treated as confirmed AC activity.

Later work may explore additional sensors, local interaction, and TinyML for a narrowly defined task when it provides a measurable improvement over simpler rules.

Tyto is designed to remain useful without cloud services or another application. A future integration with [APEX](https://github.com/edumarcano/APEX) may provide visualization, history, and access to device data.

## Current Status

Tyto has completed the v0.5.0 room-climate experiment.

The device reads temperature and relative humidity from an SHT31 over I2C, calculates dew point and short-term temperature trends, stores validated measurements in LittleFS, and keeps the measurement interval in persistent configuration.

The 72-hour room dataset was exported and analyzed offline. The analysis found repeated short cooling and recovery cycles, with 91 complete candidate cycles using the current rule.

These detected cycles describe room-temperature behavior only. The experiment did not record confirmed AC on/off times, so they should not be treated as confirmed AC activity.

Tyto continues to operate as a standalone device without Wi-Fi, cloud services, or APEX.

See [SHT31 Sensor Setup](docs/sht31-setup.md) for wiring and sensor behavior.

See [Climate History](docs/climate-history.md) for local storage and CSV export.

See [Room Climate Experiment](docs/room-climate-experiment.md) and [Analysis](analysis/README.md) for the v0.5.0 experiment and results.

## Roadmap

See [docs/roadmap.md](docs/roadmap.md) for planned releases.
