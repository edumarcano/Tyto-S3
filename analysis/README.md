# Room climate analysis

The files in this directory contain the offline analysis for the v0.5.0 room-climate experiment.

`plot_room_climate.py` expects an exported history file at:

```text
firmware/history-export.csv
```

The CSV is a local experiment artifact and is not committed to the repository.

The script uses Python with `pandas` and `matplotlib`. Run it from a clone of the repository after exporting the history data.

The generated plots are saved under `analysis/plots/`.

The analysis notes are:

- [Room climate observations](room-climate-observations.md)
- [Temperature cycle detection](temperature-cycle-detection.md)
- [Room climate experiment evaluation](room-climate-experiment-evaluation.md)

The cycle detection is offline analysis only. It is not part of the ESP32 firmware.
