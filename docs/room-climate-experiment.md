# Room climate experiment

## Setup

* Sensor: SHT31
* Location: edge of the desk, near the center of the room
* The sensor is outside the direct AC airflow
* The sensor is kept away from the laptop and ESP32 to reduce local heat influence
* ESP32 powered through USB from the laptop

Placement was adjusted after testing showed that keeping the sensor close to the laptop increased the measured temperature by about 2 °C.

## Collection

* Duration: 72 hours
* Measurement interval: 30 seconds
* Room used normally during the run
* Sensor should remain in the same position
* Measurements stored locally in LittleFS

The experiment is intended to capture normal room-climate behavior rather than isolate a single cause such as AC operation.

## Run details

```text
Start date and time: 08-18-2026 10:27 AM
Boot ID: 46
```

Note any restart, sensor movement, power interruption, or other change that could affect the data.

## Analysis

After the run, export the stored history and plot temperature, relative humidity, and dew point.

The collected data will be used to decide which patterns are worth investigating further.
