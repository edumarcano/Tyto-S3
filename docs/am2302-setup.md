# AM2302 Sensor Setup

Tyto v0.3.0 uses a three-pin ASAIR AM2302 temperature and
relative-humidity module.

## Hardware

- Controller: ESP32-S3 development board with 16 MB flash and
  8 MB-class PSRAM
- Sensor: ASAIR AM2302 three-pin module
- Sensor protocol: DHT22-compatible single-wire communication
- Data GPIO: GPIO 4
- Sensor supply: 3.3 V

## Wiring

Disconnect the ESP32-S3 from USB before changing any wiring.

| AM2302 pin | ESP32-S3 connection |
| --- | --- |
| `+` | `3V3` |
| `out` | `GPIO 4` |
| `-` | `GND` |

When the AM2302 text is upright and its pins point downward, the
three pins are ordered from left to right as `+`, `out`, and `-`.

The three-pin module includes its own pull-up resistor, so no
external pull-up resistor is used in this setup.

## Firmware dependency

The firmware uses the Adafruit DHT sensor library. It is declared in
`firmware/platformio.ini`:

```ini
lib_deps =
    adafruit/DHT sensor library
```

The AM2302 is configured as `DHT22` because that is the sensor-family
identifier used by the library.

## Building and uploading

1. Open the `firmware` directory as the PlatformIO project.
2. Connect the ESP32-S3 through USB.
3. Build the project.
4. Upload the firmware.
5. Open the serial monitor at 115200 baud.

## Serial output

The sensor driver reports its startup state and configured measurement
interval:

```text
TYTO_SENSOR uptime_ms=2125 sensor=am2302 state=driver_started gpio=4 measurement_interval_ms=5000
```

While the firmware is collecting enough valid readings for temperature
trend analysis, a successful measurement uses this format:

```text
TYTO_ENV uptime_ms=5000 sensor=am2302 status=ok data_status=fresh temperature_c=28.2 relative_humidity_percent=43.3 dew_point_c=14.5 temperature_trend=collecting
```

After enough valid readings have been collected, successful measurements
also report the current temperature trend and the calculated temperature
change across the recent comparison window:

```text
TYTO_ENV uptime_ms=30000 sensor=am2302 status=ok data_status=fresh temperature_c=28.1 relative_humidity_percent=43.3 dew_point_c=14.5 temperature_trend=stable temperature_change_c=-0.03
```

The possible temperature-trend states are `warming`, `cooling`, `stable`,
and `collecting`.

If sensor communication fails before any valid measurement has been
received, no usable environmental data is available:

```text
TYTO_ENV uptime_ms=5000 sensor=am2302 status=read_error data_status=unavailable
```

If communication fails after a valid measurement has already been
received, the previous valid values are preserved but clearly identified
as stale:

```text
TYTO_ENV uptime_ms=35000 sensor=am2302 status=read_error data_status=stale last_valid_age_ms=5000 last_valid_temperature_c=28.1 last_valid_relative_humidity_percent=43.3 last_valid_dew_point_c=14.5
```

A received measurement outside the accepted sensor range is reported as
`invalid_data`. If no previous valid measurement exists, its data status
is `unavailable`. If a previous valid measurement exists, that retained
measurement is reported separately as stale.

Measurements are attempted every five seconds by default.

The measurement interval can be changed at runtime through the serial
monitor using:

```text
interval 10000
```
The value is specified in milliseconds and must be between 2000 ms and
60000 ms. A successful update is stored in ESP32 nonvolatile storage and
survives resets and power cycles.

At startup, Tyto reports the active interval and whether it came from
persistent configuration:

```text
TYTO_CONFIG uptime_ms=2127 status=ok measurement_interval_ms=10000 source=persisted
```

Invalid values are rejected without changing the active interval.

## Validation behavior

A measurement is accepted as valid only when:

* both sensor reads return finite values;
* temperature is between -40 °C and 80 °C;
* relative humidity is between 0% and 100%.

Dew point and temperature-trend history are updated only from valid
measurements. Failed or invalid readings are not added to the trend
history.

When a read fails after a previous valid measurement, the firmware keeps
the last valid temperature, relative humidity, and dew point in memory.
Those values are reported with `last_valid_*` field names and
`data_status=stale` so they are not presented as new measurements. The
reported `last_valid_age_ms` value shows how long it has been since the
last successful reading.

The firmware continues running after sensor failures and attempts another
measurement at the next configured interval. Normal fresh-data reporting
resumes when valid sensor readings return.


## Current limitations

- Calling the sensor driver's `begin()` method does not confirm that
  the physical sensor is present.
- Sensor availability is confirmed only when a valid read succeeds.
