from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


repo_root = Path(__file__).resolve().parents[1]
csv_path = repo_root / "firmware" / "history-export.csv"

data = pd.read_csv(csv_path)

print(f"Rows: {len(data)}")
print(f"Columns: {list(data.columns)}")
print(f"Boot IDs: {sorted(data['boot_id'].unique())}")

print()
print(data.head())

data["temperature_f"] = data["temperature_c"] * 9 / 5 + 32
data["dew_point_f"] = data["dew_point_c"] * 9 / 5 + 32

boot_ids = sorted(data["boot_id"].unique())

elapsed_ms = []
offset_ms = 0

for boot_id in boot_ids:
    boot_data = data[data["boot_id"] == boot_id]

    boot_elapsed = boot_data["uptime_ms"] - boot_data["uptime_ms"].iloc[0]
    boot_elapsed = boot_elapsed + offset_ms

    elapsed_ms.extend(boot_elapsed)

    offset_ms = boot_elapsed.iloc[-1] + 30_000

data["elapsed_hours"] = [value / 3_600_000 for value in elapsed_ms]

print()
print(data[["boot_id", "uptime_ms", "elapsed_hours"]].head())
print(data[["boot_id", "uptime_ms", "elapsed_hours"]].tail())

# Temperature plots
# Celcius
plt.figure(figsize=(10, 5))

plt.plot(
    data["elapsed_hours"],
    data["temperature_c"],
)

plt.xlabel("Experiment time (hours)")
plt.ylabel("Temperature (°C)")
plt.title("Room temperature over 72-hour experiment")

plt.tight_layout()
plt.savefig(
    repo_root / "analysis/plots" / "temperature.png",
    dpi=150,
)
plt.show()

# Fahrenheit
plt.figure(figsize=(10, 5))

plt.plot(
    data["elapsed_hours"],
    data["temperature_f"],
)

plt.xlabel("Experiment time (hours)")
plt.ylabel("Temperature (°F)")
plt.title("Room temperature over 72-hour experiment")

plt.tight_layout()
plt.savefig(
    repo_root / "analysis/plots" / "temperature-fahrenheit.png",
    dpi=150,
)
plt.show()

# Relative humidity plot
plt.figure(figsize=(10, 5))

plt.plot(
    data["elapsed_hours"],
    data["relative_humidity_percent"],
)

plt.xlabel("Experiment time (hours)")
plt.ylabel("Relative humidity (%)")
plt.title("Room relative humidity over 72-hour experiment")

plt.tight_layout()
plt.savefig(
    repo_root / "analysis/plots" / "relative_humidity.png",
    dpi=150,
)
plt.show()

# Dew point plots
# Celcius
plt.figure(figsize=(10, 5))

plt.plot(
    data["elapsed_hours"],
    data["dew_point_c"],
)

plt.xlabel("Experiment time (hours)")
plt.ylabel("Dew point (°C)")
plt.title("Room dew point over 72-hour experiment")

plt.tight_layout()
plt.savefig(
    repo_root / "analysis/plots" / "dew_point.png",
    dpi=150,
)
plt.show()

# Fahrenheit
plt.figure(figsize=(10, 5))

plt.plot(
    data["elapsed_hours"],
    data["dew_point_f"],
)

plt.xlabel("Experiment time (hours)")
plt.ylabel("Dew point (°F)")
plt.title("Room dew point over 72-hour experiment")

plt.tight_layout()
plt.savefig(
    repo_root / "analysis/plots" / "dew-point-fahrenheit.png",
    dpi=150,
)
plt.show()


fig, axes = plt.subplots(
    3,
    1,
    figsize=(12, 10),
    sharex=True,
)

axes[0].plot(
    data["elapsed_hours"],
    data["temperature_c"],
)
axes[0].set_ylabel("Temperature (°C)")
axes[0].set_title("Room climate over 72-hour experiment")

axes[1].plot(
    data["elapsed_hours"],
    data["relative_humidity_percent"],
)
axes[1].set_ylabel("Relative humidity (%)")

axes[2].plot(
    data["elapsed_hours"],
    data["dew_point_c"],
)
axes[2].set_ylabel("Dew point (°C)")
axes[2].set_xlabel("Experiment time (hours)")

plt.tight_layout()

plt.savefig(
    repo_root / "analysis/plots" / "room-climate-combined.png",
    dpi=150,
)

plt.show()