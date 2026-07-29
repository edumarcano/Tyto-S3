# AM2302 Sensor Setup

Tyto v0.2.0 uses a three-pin ASAIR AM2302 temperature and
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
````

The AM2302 is configured as `DHT22` because that is the sensor-family
identifier used by the library.

## Building and uploading

1. Open the `firmware` directory as the PlatformIO project.
2. Connect the ESP32-S3 through USB.
3. Build the project.
4. Upload the firmware.
5. Open the serial monitor at 115200 baud.

## Serial output

The sensor driver reports its startup state:

```text
TYTO_SENSOR uptime_ms=2000 sensor=am2302 state=driver_started gpio=4
```

A valid measurement uses this format:

```text
TYTO_ENV uptime_ms=7000 sensor=am2302 status=ok temperature_c=28.5 relative_humidity_percent=41.2
```

A communication failure uses:

```text
TYTO_ENV uptime_ms=12000 sensor=am2302 status=read_error
```

A received measurement outside the accepted sensor range uses:

```text
TYTO_ENV uptime_ms=17000 sensor=am2302 status=invalid_data temperature_c=... relative_humidity_percent=...
```

Measurements are attempted every five seconds.

## Validation behavior

A measurement is reported as valid only when:

- both sensor reads return finite values;
- temperature is between -40 °C and 80 °C;
- relative humidity is between 0% and 100%.

Failed reads are reported rather than replaced with zero, an old
measurement, or fabricated data. The firmware continues running and
tries again at the next measurement interval.

## Current limitations

- Calling the sensor driver's `begin()` method does not confirm that
  the physical sensor is present.
- Sensor availability is confirmed only when a valid read succeeds.
- v0.2.0 reports current temperature and relative humidity but does
  not calculate dew point or trends.
- The five-second measurement interval is fixed in the firmware.
