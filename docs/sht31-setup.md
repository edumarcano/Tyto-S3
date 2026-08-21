# SHT31 Sensor Setup

Tyto uses an SHT31 temperature and relative-humidity sensor connected over I2C.

## Hardware

- Controller: ESP32-S3 development board with 16 MB flash and 8 MB PSRAM
- Sensor: SHT31 temperature and relative-humidity breakout
- Interface: I2C
- I2C address: `0x44`
- SDA: GPIO 8
- SCL: GPIO 9
- Sensor supply: 3.3 V

## Wiring

Disconnect the ESP32-S3 from USB power before changing any wiring.

| SHT31 pin | ESP32-S3 connection |
| --------- | ------------------- |
| `VIN`     | `3V3`               |
| `GND`     | `GND`               |
| `SDA`     | `GPIO 8`            |
| `SCL`     | `GPIO 9`            |

The SHT31 breakout used for Tyto has four header pins soldered to the board.

## Firmware dependency

The firmware uses the Adafruit SHT31 library, declared in `firmware/platformio.ini`:

```ini
lib_deps =
    adafruit/Adafruit SHT31 Library
```

The ESP32-S3 I2C bus uses GPIO 8 for SDA and GPIO 9 for SCL. The sensor is initialized at address `0x44`.

## Building and uploading

1. Open the `firmware` directory as the PlatformIO project.
2. Connect the ESP32-S3 through USB.
3. Build the project.
4. Upload the firmware.
5. Open the serial monitor at 115200 baud.

## Serial output

Successful sensor initialization reports the I2C address, GPIO pins, and measurement interval:

```text
TYTO_SENSOR uptime_ms=2160 sensor=sht31 state=ready address=0x44 sda_gpio=8 scl_gpio=9 measurement_interval_ms=10000
```

A successful measurement looks like:

```text
TYTO_ENV uptime_ms=10000 sensor=sht31 status=ok data_status=fresh temperature_c=28.4 relative_humidity_percent=51.6 dew_point_c=17.4 temperature_trend=collecting
```

After enough valid readings have been collected, the output also includes the temperature trend and temperature change:

```text
TYTO_ENV uptime_ms=100000 sensor=sht31 status=ok data_status=fresh temperature_c=27.7 relative_humidity_percent=52.2 dew_point_c=17.0 temperature_trend=stable temperature_change_c=-0.11
```

Measurements are attempted at the configured interval. The interval is stored in persistent configuration and survives resets and power cycles.

## Validation behavior

A measurement is valid only when:

- both sensor readings are finite values;
- temperature is between -40 °C and 80 °C;
- relative humidity is between 0% and 100%.

Dew point and temperature trend are updated only from valid measurements.

If communication fails after a valid reading, Tyto keeps the last valid temperature, relative humidity, and dew point in memory and reports them as stale data.

For example:

```text
TYTO_ENV uptime_ms=110000 sensor=sht31 status=read_error data_status=stale last_valid_age_ms=10000 last_valid_temperature_c=27.7 last_valid_relative_humidity_percent=52.2 last_valid_dew_point_c=17.0
```

The firmware keeps running and tries the sensor again at the next configured interval.

## Hardware validation

The documented wiring has been tested on the ESP32-S3.

The SHT31 initialized successfully at address `0x44` and produced temperature and relative-humidity readings.

Communication was also tested by disconnecting SDA while the firmware was running. Tyto reported read errors and kept the previous measurement as stale data. After SDA was reconnected, fresh measurements resumed without restarting the ESP32-S3.

## Placement limitation

A single SHT31 measures conditions at one point in the room. Nearby electronics, direct airflow, sunlight, and other local effects can change the reading.

The v0.5.0 room experiment includes a placement test and documents this limitation in [Room Climate Experiment](room-climate-experiment.md).
