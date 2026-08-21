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

# Build one continuous experiment timeline across ESP32 reboots.
boot_ids = sorted(data["boot_id"].unique())

elapsed_ms = []
offset_ms = 0

for boot_id in boot_ids:
    boot_data = data[data["boot_id"] == boot_id]

    boot_elapsed = boot_data["uptime_ms"] - boot_data["uptime_ms"].iloc[0]
    boot_elapsed = boot_elapsed + offset_ms

    elapsed_ms.extend(boot_elapsed)

    # Continue the next boot after one expected 30-second sample interval.
    offset_ms = boot_elapsed.iloc[-1] + 30_000

data["elapsed_hours"] = [value / 3_600_000 for value in elapsed_ms]

print()
print(data[["boot_id", "uptime_ms", "elapsed_hours"]].head())
print(data[["boot_id", "uptime_ms", "elapsed_hours"]].tail())

# Temperature plots
# Celsius
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
    repo_root / "analysis/plots" / "relative-humidity.png",
    dpi=150,
)
plt.show()

# Dew point plots
# Celsius
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
    repo_root / "analysis/plots" / "dew-point.png",
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

# Combined climate plots
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

window = data[
    (data["elapsed_hours"] >= 0)
    & (data["elapsed_hours"] <= 72)
]

plt.figure(figsize=(12, 5))

plt.plot(
    window["elapsed_hours"],
    window["temperature_c"],
)

plt.xlabel("Experiment time (hours)")
plt.ylabel("Temperature (°C)")
plt.title("Temperature detail: hours 0–72")

plt.tight_layout()
plt.show()

temperatures = window["temperature_c"].reset_index(drop=True)
times = window["elapsed_hours"].reset_index(drop=True)

# Ignore small reversals so sensor noise does not create extra turning points.
reversal_threshold_c = 0.4

turning_points = []

trend = None
minimum_index = 0
maximum_index = 0
extreme_index = 0

# Track whether temperature is currently rising or falling and record
# a turning point once the direction reverses by the threshold amount.
for index in range(1, len(temperatures)):
    current = temperatures.iloc[index]

    if trend is None:
        if current < temperatures.iloc[minimum_index]:
            minimum_index = index

        if current > temperatures.iloc[maximum_index]:
            maximum_index = index

        if (
            temperatures.iloc[maximum_index]
            - temperatures.iloc[minimum_index]
            >= reversal_threshold_c
        ):
            if minimum_index < maximum_index:
                turning_points.append(
                    (
                        "trough",
                        times.iloc[minimum_index],
                        temperatures.iloc[minimum_index],
                    )
                )
                trend = "rising"
                extreme_index = maximum_index
            else:
                turning_points.append(
                    (
                        "peak",
                        times.iloc[maximum_index],
                        temperatures.iloc[maximum_index],
                    )
                )
                trend = "falling"
                extreme_index = minimum_index

    elif trend == "rising":
        if current > temperatures.iloc[extreme_index]:
            extreme_index = index

        elif (
            temperatures.iloc[extreme_index] - current
            >= reversal_threshold_c
        ):
            turning_points.append(
                (
                    "peak",
                    times.iloc[extreme_index],
                    temperatures.iloc[extreme_index],
                )
            )
            trend = "falling"
            extreme_index = index

    elif trend == "falling":
        if current < temperatures.iloc[extreme_index]:
            extreme_index = index

        elif (
            current - temperatures.iloc[extreme_index]
            >= reversal_threshold_c
        ):
            turning_points.append(
                (
                    "trough",
                    times.iloc[extreme_index],
                    temperatures.iloc[extreme_index],
                )
            )
            trend = "rising"
            extreme_index = index

for kind, time_hours, temperature_c in turning_points:
    print(
        f"{kind:6} "
        f"time={time_hours:.3f} h "
        f"temperature={temperature_c:.2f} °C"
    )

    print()

candidate_cooling_count = 0
candidate_recovery_count = 0
candidate_legs = []

for first, second in zip(turning_points, turning_points[1:]):
    first_kind, first_time, first_temperature = first
    second_kind, second_time, second_temperature = second

    duration_minutes = (second_time - first_time) * 60
    temperature_change_c = second_temperature - first_temperature

    if first_kind == "peak" and second_kind == "trough":
        is_candidate = (
            abs(temperature_change_c) >= 0.7
            and duration_minutes <= 30
        )

        if is_candidate:
            candidate_cooling_count += 1
            candidate_legs.append("cooling")
        else:
            candidate_legs.append(None)

        print(
            f"cooling  "
            f"duration={duration_minutes:.1f} min "
            f"change={temperature_change_c:.2f} °C"
        )

    elif first_kind == "trough" and second_kind == "peak":
        is_candidate = (
            temperature_change_c >= 0.7
            and duration_minutes <= 30
        )

        if is_candidate:
            candidate_recovery_count += 1
            candidate_legs.append("recovery")
        else:
            candidate_legs.append(None)

        print(
            f"recovery "
            f"duration={duration_minutes:.1f} min "
            f"change=+{temperature_change_c:.2f} °C"
        )

print()
print(f"Candidate cooling periods: {candidate_cooling_count}")
print(f"Candidate recovery periods: {candidate_recovery_count}")

complete_candidate_cycles = 0

for first_leg, second_leg in zip(
    candidate_legs,
    candidate_legs[1:],
):
    if (
        first_leg == "cooling"
        and second_leg == "recovery"
    ):
        complete_candidate_cycles += 1

print(
    f"Complete candidate cycles: "
    f"{complete_candidate_cycles}"
)