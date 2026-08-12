# Climate history

This document is a reference for Tyto's local climate-history system: how measurements are stored, how the files rotate, what each CSV field means, and how to export the data for analysis.

## Overview

Tyto stores validated AM2302 measurements in the ESP32-S3's LittleFS filesystem.

The history format is CSV so the stored data can be:

* inspected directly over serial;
* captured to a normal file on a computer;
* opened in spreadsheet tools;
* loaded into Python, R, or other analysis tools later.

The design intentionally keeps the stored format simple and preserves the original sensor observations alongside values derived by the firmware.

## Storage layout

Tyto uses two history files:

```text
/history.csv
/history-old.csv
```

`/history.csv` is the active file.

When it reaches the configured size limit, Tyto rotates the files:

```text
/history-old.csv   <- deleted if it already exists
/history.csv       <- renamed to /history-old.csv
/history.csv       <- recreated with a fresh CSV header
```

Only one previous history file is retained.

The production limit for the active history file is:

```text
2 MiB
```

This keeps history bounded instead of allowing one file to consume the entire LittleFS partition.

## CSV schema

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

## Field reference

### `boot_id`

Persistent identifier for the current boot session.

Tyto increments this value during startup and stores it in nonvolatile storage.

Example:

```text
boot 34
boot 35
boot 36
```

A boot ID may be skipped in the CSV if the device restarts before producing a valid stored measurement.

The value exists because `uptime_ms` resets after every restart.

### `uptime_ms`

Milliseconds since the current boot.

This is elapsed time, not calendar time.

Example:

```text
34,12208,...
35,2172,...
```

The lower `uptime_ms` on boot 35 is expected because the timer restarted.

Use the combination:

```text
boot_id + uptime_ms
```

to identify the position of a measurement within recorded device history.

Tyto does not currently have a real-time clock, so the CSV does not contain a trustworthy wall-clock timestamp.

### `temperature_c`

Raw temperature reported by the AM2302/DHT22.

Unit:

```text
degrees Celsius
```

### `relative_humidity_percent`

Raw relative humidity reported by the AM2302/DHT22.

Unit:

```text
percent
```

### `dew_point_c`

Dew point calculated by Tyto from temperature and relative humidity.

This is a derived value, not a direct sensor observation.

Unit:

```text
degrees Celsius
```

### `temperature_trend`

Short-term temperature classification calculated by Tyto.

Possible values:

```text
collecting
stable
warming
cooling
```

`collecting` means there are not yet enough temperature samples in the current boot to calculate the trend.

### `temperature_change_c`

Numeric temperature change used by the trend calculation.

When the trend is still collecting, this field is intentionally empty:

```text
34,2208,27.1,37.5,11.4,collecting,
```

Once enough samples exist:

```text
34,12208,27.2,37.6,11.5,stable,0.03
```

## Raw vs derived data

Raw sensor observations:

```text
temperature_c
relative_humidity_percent
```

Derived values:

```text
dew_point_c
temperature_trend
temperature_change_c
```

Session/timing context:

```text
boot_id
uptime_ms
```

The raw observations are kept in the history so derived values can be checked or recalculated later instead of relying only on firmware-generated results.

## What gets stored

Only valid, fresh sensor measurements are written to history.

A measurement must:

* be read successfully from the AM2302;
* contain finite temperature and humidity values;
* pass the configured valid sensor ranges.

Failed reads and invalid measurements are reported through live `TYTO_ENV` telemetry but are not persisted in the CSV.

Stale retained values are also not written as new history records.

Therefore, every CSV row represents a successful validated observation.

## Storage failure behavior

History storage is secondary to the climate-monitoring loop.

If LittleFS cannot be mounted, a history file cannot be opened, or a rotation operation fails, Tyto reports a `TYTO_STORAGE` error but continues attempting sensor measurements and producing live serial telemetry.

A storage failure should not stop the environmental monitor.

## Viewing history over serial

Current history:

```text
history
```

Previous rotated history:

```text
history old
```

Current-history output is wrapped like this:

```text
TYTO_HISTORY_BEGIN path=/history.csv
boot_id,uptime_ms,temperature_c,relative_humidity_percent,dew_point_c,temperature_trend,temperature_change_c
34,2208,27.1,37.5,11.4,collecting,
34,12208,27.2,37.6,11.5,stable,0.03
TYTO_HISTORY_END path=/history.csv
```

The markers are not part of the CSV itself. They identify the beginning and end of the export in the normal serial stream.

## Exporting history to a file

The serial monitor can be logged by PlatformIO and the history block extracted afterward.

From the `firmware` directory:

```powershell
pio device monitor -b 115200 -f default -f log2file
```

On the Windows development setup used for Tyto, `pio` may not be available directly in PowerShell because PlatformIO was installed through the VS Code extension.

In that case use:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\pio.exe" device monitor -b 115200 -f default -f log2file
```

Once the monitor opens, enter:

```text
history
```

Then exit the monitor after the complete history block has been printed.

PlatformIO writes the captured monitor session under:

```text
firmware/logs/
```

The log contains all serial traffic, not just CSV data.

## Extracting clean CSV from the monitor log

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

This produces:

```text
firmware/history-export.csv
```

containing only the CSV header and data rows.

The resulting file can be opened directly in Excel or loaded by normal CSV analysis tools.

The generated `.log` and `history-export.csv` files are local artifacts and should not normally be committed to the repository.

## Persistent measurement interval

The measurement interval is configurable over serial.

Example:

```text
interval 10000
```

The value is in milliseconds.

Valid range:

```text
2000 to 60000 ms
```

A successful update is stored in ESP32 nonvolatile storage and survives resets and power cycles.

Startup reports the restored value:

```text
TYTO_CONFIG uptime_ms=2135 status=ok measurement_interval_ms=10000 source=persisted
```

Invalid values are rejected without changing the active setting.

## Storage implementation notes

Configuration and measurement history use different storage mechanisms.

Persistent configuration such as:

```text
measurement interval
boot ID
```

is stored through ESP32 Preferences/NVS.

Growing measurement history is stored in LittleFS.

This separation is intentional:

```text
NVS        -> small persistent key/value configuration
LittleFS   -> growing file-based measurement history
```

LittleFS is mounted with:

```cpp
LittleFS.begin(false)
```

so a mount failure does not automatically format the filesystem and destroy stored history.

The filesystem was formatted explicitly during initial development, then normal operation returned to non-formatting mount behavior.

## Things to remember

* `uptime_ms` is only meaningful within one boot.
* Use `boot_id` to distinguish separate boot sessions.
* Missing boot IDs in history are normal.
* `temperature_c` and `relative_humidity_percent` are the raw sensor observations.
* Dew point and trend fields are derived.
* `temperature_change_c` is blank while the trend is still collecting.
* Failed or stale readings are not persisted.
* `/history.csv` is the active file.
* `/history-old.csv` is the single retained rotated file.
* History is bounded to 2 MiB per active file.
* Storage problems should not stop sensor telemetry.
* Export markers are not part of the CSV.
