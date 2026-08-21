# Room climate experiment

## Setup

- Sensor: SHT31
- Location: edge of the desk, near the center of the room
- Outside direct AC airflow
- Kept away from the laptop and ESP32 to reduce local heat influence
- ESP32 powered through USB from the laptop

The sensor position was adjusted before the run after a placement test showed that keeping it close to the laptop raised the measured temperature by about 2 °C.

## Collection

- Duration: 72 hours
- Measurement interval: 30 seconds
- Room used normally during the run
- Measurements stored locally in LittleFS

The goal was to capture normal room-climate behavior rather than force or isolate a specific cause such as AC operation.

## Run record

```text
Start date and time: 2026-08-18 10:27 AM EDT
Starting boot ID: 46
```

A brief power interruption during a thunderstorm restarted the ESP32 during the run. Collection continued under boot ID 47. The sensor position and measurement configuration were unchanged.

Tyto does not have a real-time clock, so the exact wall-clock length of the interruption cannot be recovered from the stored data.

## Analysis

The stored history was exported after the run and used to plot temperature, relative humidity, and dew point.

See the analysis files for the results:

- [Room climate observations](../analysis/room-climate-observations.md)
- [Temperature cycle detection](../analysis/temperature-cycle-detection.md)
- [Experiment evaluation](../analysis/room-climate-experiment-evaluation.md)
- [Generated plots](../analysis/plots/)

The run showed repeated short cooling and recovery cycles, but confirmed AC on/off times were not recorded. The detected cycles therefore describe measured room behavior and are not treated as confirmed AC activity.
