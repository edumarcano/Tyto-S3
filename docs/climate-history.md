# Climate history

Tyto stores valid climate measurements in LittleFS so they can be inspected over serial and exported for later analysis.

## Storage layout

Tyto uses two files:

```text
/history.csv
/history-old.csv
```

`/history.csv` is the active file. When it reaches 2 MiB, Tyto rotates it to `/history-old.csv` and starts a new active file. Only one previous file is kept.

## CSV format

The history header is:

```text
boot_id,uptime_ms,temperature_c,relative_humidity_percent,dew_point_c,temperature_trend,temperature_change_c
```

Example:

```text
boot_id,uptime_ms,temperature_c,relative_humidity_percent,dew_point_c,temperature_trend,temperature_change_c
34,2208,27.1,37.5,11.4,collecting,
34,4208,27.1,37.5,11.4,collecting,
34,12208,27.2,37.6,11.5,stable,0.03
35,2172,27.2,37.6,11.5,collecting,
```

### `boot_id`

A persistent ID for the current boot session. Tyto increments it during startup and stores it in ESP32 nonvolatile storage.

A boot ID may be missing from the CSV if the device restarts before it stores a valid measurement.

### `uptime_ms`

Milliseconds since the current boot. This is elapsed time, not calendar time, and it resets after every restart.

Use `boot_id` together with `uptime_ms` when working with measurements across more than one boot.

Tyto does not currently have a real-time clock, so the CSV does not contain a trustworthy wall-clock timestamp.

### `temperature_c`

Temperature reported by the SHT31 in degrees Celsius.

### `relative_humidity_percent`

Relative humidity reported by the SHT31 as a percentage.

### `dew_point_c`

Dew point calculated by Tyto from temperature and relative humidity. It is not measured directly by the sensor.

### `temperature_trend`

Short-term temperature state calculated by Tyto.

Possible values are:

```text
collecting
stable
warming
cooling
```

`collecting` means there are not yet enough valid temperature samples in the current boot to calculate a trend.

### `temperature_change_c`

The temperature change used by the trend calculation. This field is empty while the trend is still `collecting`.

## What gets stored

Only valid, fresh SHT31 measurements are written to history.

A measurement must:

- be read successfully from the SHT31;
- contain finite temperature and humidity values;
- be between -40 °C and 80 °C for temperature;
- be between 0% and 100% for relative humidity.

Failed reads and invalid measurements are still reported through live `TYTO_ENV` output, but they are not written to the CSV. Retained stale values are not written as new rows either.

The sensor readings are kept alongside the calculated values so the calculations can be checked or repeated later.

## Storage failure behavior

History storage is separate from the climate-monitoring loop.

If LittleFS cannot be mounted, a history file cannot be opened, or file rotation fails, Tyto reports a `TYTO_STORAGE` error and continues trying to read the sensor and produce live serial output.

## Serial commands

Print the current history file:

```text
history
```

Print the previous rotated history file:

```text
history old
```

If the active file has never rotated, `/history-old.csv` will not exist yet and the command will report an open failure.

Clear both history files and create a new active file:

```text
history clear
```

A history export is wrapped with markers such as:

```text
TYTO_HISTORY_BEGIN path=/history.csv
boot_id,uptime_ms,temperature_c,relative_humidity_percent,dew_point_c,temperature_trend,temperature_change_c
34,2208,27.1,37.5,11.4,collecting,
34,12208,27.2,37.6,11.5,stable,0.03
TYTO_HISTORY_END path=/history.csv
```

The markers are part of the serial output, not the CSV itself.

## Exporting history

From the `firmware` directory, start the PlatformIO serial monitor with logging enabled:

```powershell
pio device monitor -b 115200 -f log2file
```

If `pio` is not available directly in PowerShell because PlatformIO was installed through the VS Code extension, use:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" device monitor -b 115200 -f log2file
```

Once the monitor opens, enter:

```text
history
```

Wait until `TYTO_HISTORY_END` appears before closing the monitor.

PlatformIO writes the captured serial session under:

```text
firmware/logs/
```

The log contains all serial output, not only the history CSV.

### Extracting the CSV

From the `firmware` directory in PowerShell:

```powershell
$log = Get-ChildItem .\logs\*.log |
    Sort-Object LastWriteTime -Descending |
    Select-Object -First 1

$capturing = $false

Get-Content $log.FullName | ForEach-Object {
    if ($_ -like "TYTO_HISTORY_BEGIN*") {
        $capturing = $true
    }
    elseif ($_ -like "TYTO_HISTORY_END*") {
        $capturing = $false
    }
    elseif ($capturing -and $_.Trim().Length -gt 0) {
        $_
    }
} | Set-Content .\history-export.csv
```

This creates:

```text
firmware/history-export.csv
```

The generated `.log` and `history-export.csv` files are local artifacts and should not normally be committed.

## Measurement interval

The measurement interval can be changed over serial:

```text
interval 10000
```

The value is in milliseconds. The valid range is 2000 to 60000 ms.

A successful change is stored in ESP32 nonvolatile storage and survives resets and power cycles. Invalid values are rejected without changing the active setting.

Startup reports the restored value, for example:

```text
TYTO_CONFIG uptime_ms=2135 status=ok measurement_interval_ms=10000 source=persisted
```

## Storage details

Tyto uses two ESP32 storage systems for different jobs:

```text
NVS        -> measurement interval and boot ID
LittleFS   -> measurement history files
```

LittleFS is mounted with:

```cpp
LittleFS.begin(false)
```

A mount failure therefore does not automatically format the filesystem and erase stored history.
