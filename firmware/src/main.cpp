#include <Arduino.h>
#include <DHT.h>
#include <Preferences.h>
#include <LittleFS.h>

namespace {

constexpr uint32_t kBytesPerMegabyte = 1024UL * 1024UL;
constexpr uint32_t kDefaultMeasurementIntervalMs = 5000;
constexpr uint32_t kMinimumMeasurementIntervalMs = 2000;
constexpr uint32_t kMaximumMeasurementIntervalMs = 60000;

constexpr char kPreferencesNamespace[] = "tyto";
constexpr char kMeasurementIntervalKey[] = "measure_ms";

constexpr char kHistoryFilePath[] = "/history.csv";

constexpr size_t kSerialCommandBufferSize = 32;
constexpr char kIntervalCommandPrefix[] = "interval ";

constexpr uint8_t kDhtDataPin = 4;
constexpr uint8_t kDhtType = DHT22;

// Compare averages of the oldest and newest three samples in a
// six-sample window. At the current 5 s sampling interval, the
// window spans about 25 s. The 0.2 C threshold is provisional
// and is intended only for basic trend reporting.
constexpr size_t kTemperatureTrendSampleCount = 6;
constexpr float kTemperatureTrendThresholdC = 0.2F;

constexpr float kMinimumTemperatureC = -40.0F;
constexpr float kMaximumTemperatureC = 80.0F;
constexpr float kMinimumRelativeHumidityPercent = 0.0F;
constexpr float kMaximumRelativeHumidityPercent = 100.0F;

DHT climateSensor(kDhtDataPin, kDhtType);

uint32_t measurementIntervalMs =
    kDefaultMeasurementIntervalMs;

uint32_t lastMeasurementMs = 0;

bool historyStorageAvailable = false;

char serialCommandBuffer[kSerialCommandBufferSize] = {};
size_t serialCommandLength = 0;

float recentTemperaturesC[kTemperatureTrendSampleCount] = {};
size_t temperatureSampleCount = 0;

bool hasValidMeasurement = false;
uint32_t lastValidMeasurementMs = 0;

float lastValidTemperatureC = 0.0F;
float lastValidRelativeHumidityPercent = 0.0F;
float lastValidDewPointC = 0.0F;

bool isMeasurementInRange(
    const float temperatureC,
    const float relativeHumidityPercent
) {
    return temperatureC >= kMinimumTemperatureC &&
           temperatureC <= kMaximumTemperatureC &&
           relativeHumidityPercent >=
               kMinimumRelativeHumidityPercent &&
           relativeHumidityPercent <=
               kMaximumRelativeHumidityPercent;
}

bool isMeasurementIntervalValid(const uint32_t intervalMs) {
    return intervalMs >= kMinimumMeasurementIntervalMs &&
           intervalMs <= kMaximumMeasurementIntervalMs;
}

// Calculate dew point using the Magnus approximation
// with constants a = 17.62 and b = 243.12 C.
float calculateDewPointC(
    const float temperatureC,
    const float relativeHumidityPercent
) {
    if (relativeHumidityPercent <= 0.0F) {
        return NAN;
    }

    constexpr float kMagnusA = 17.62F;
    constexpr float kMagnusB = 243.12F;

    const float gamma =
        log(relativeHumidityPercent / 100.0F) +
        (kMagnusA * temperatureC) /
            (kMagnusB + temperatureC);

    return (kMagnusB * gamma) /
           (kMagnusA - gamma);
}

void addTemperatureSample(const float temperatureC) {
    if (temperatureSampleCount < kTemperatureTrendSampleCount) {
        recentTemperaturesC[temperatureSampleCount] = temperatureC;
        ++temperatureSampleCount;
        return;
    }

    for (size_t i = 1; i < kTemperatureTrendSampleCount; ++i) {
        recentTemperaturesC[i - 1] = recentTemperaturesC[i];
    }

    recentTemperaturesC[kTemperatureTrendSampleCount - 1] =
        temperatureC;
}

float calculateTemperatureTrendChangeC() {
    float olderTotalC = 0.0F;
    float newerTotalC = 0.0F;

    for (size_t i = 0; i < 3; ++i) {
        olderTotalC += recentTemperaturesC[i];
        newerTotalC += recentTemperaturesC[i + 3];
    }

    const float olderAverageC = olderTotalC / 3.0F;
    const float newerAverageC = newerTotalC / 3.0F;

    return newerAverageC - olderAverageC;
}

const char* getTemperatureTrend(const float temperatureChangeC) {
    if (temperatureChangeC <= -kTemperatureTrendThresholdC) {
        return "cooling";
    }

    if (temperatureChangeC >= kTemperatureTrendThresholdC) {
        return "warming";
    }

    return "stable";
}

bool isMeasurementDue(const uint32_t nowMs) {
    if (nowMs - lastMeasurementMs < measurementIntervalMs) {
        return false;
    }

    lastMeasurementMs = nowMs;
    return true;
}

void updateLastValidMeasurement(
    const uint32_t nowMs,
    const float temperatureC,
    const float relativeHumidityPercent,
    const float dewPointC
) {
    hasValidMeasurement = true;
    lastValidMeasurementMs = nowMs;

    lastValidTemperatureC = temperatureC;
    lastValidRelativeHumidityPercent =
        relativeHumidityPercent;
    lastValidDewPointC = dewPointC;
}

void printUnavailableReadError(
    const unsigned long uptimeMs
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=am2302"
        " status=read_error"
        " data_status=unavailable\n",
        uptimeMs
    );
}

void printStaleReadError(
    const unsigned long uptimeMs,
    const uint32_t lastValidAgeMs
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=am2302"
        " status=read_error"
        " data_status=stale"
        " last_valid_age_ms=%lu"
        " last_valid_temperature_c=%.1f"
        " last_valid_relative_humidity_percent=%.1f"
        " last_valid_dew_point_c=%.1f\n",
        uptimeMs,
        static_cast<unsigned long>(lastValidAgeMs),
        static_cast<double>(lastValidTemperatureC),
        static_cast<double>(
            lastValidRelativeHumidityPercent
        ),
        static_cast<double>(lastValidDewPointC)
    );
}

void printUnavailableInvalidData(
    const unsigned long uptimeMs,
    const float temperatureC,
    const float relativeHumidityPercent
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=am2302"
        " status=invalid_data"
        " data_status=unavailable"
        " temperature_c=%.1f"
        " relative_humidity_percent=%.1f\n",
        uptimeMs,
        static_cast<double>(temperatureC),
        static_cast<double>(relativeHumidityPercent)
    );
}

void printStaleInvalidData(
    const unsigned long uptimeMs,
    const float temperatureC,
    const float relativeHumidityPercent,
    const uint32_t lastValidAgeMs
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=am2302"
        " status=invalid_data"
        " data_status=stale"
        " temperature_c=%.1f"
        " relative_humidity_percent=%.1f"
        " last_valid_age_ms=%lu"
        " last_valid_temperature_c=%.1f"
        " last_valid_relative_humidity_percent=%.1f"
        " last_valid_dew_point_c=%.1f\n",
        uptimeMs,
        static_cast<double>(temperatureC),
        static_cast<double>(relativeHumidityPercent),
        static_cast<unsigned long>(lastValidAgeMs),
        static_cast<double>(lastValidTemperatureC),
        static_cast<double>(lastValidRelativeHumidityPercent),
        static_cast<double>(lastValidDewPointC)
    );
}

void printCollectingMeasurement(
    const unsigned long uptimeMs,
    const float temperatureC,
    const float relativeHumidityPercent,
    const float dewPointC
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=am2302"
        " status=ok"
        " data_status=fresh"
        " temperature_c=%.1f"
        " relative_humidity_percent=%.1f"
        " dew_point_c=%.1f"
        " temperature_trend=collecting\n",
        uptimeMs,
        static_cast<double>(temperatureC),
        static_cast<double>(relativeHumidityPercent),
        static_cast<double>(dewPointC)
    );
}

void printClimateMeasurement(
    const unsigned long uptimeMs,
    const float temperatureC,
    const float relativeHumidityPercent,
    const float dewPointC,
    const char* temperatureTrend,
    const float temperatureTrendChangeC
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=am2302"
        " status=ok"
        " data_status=fresh"
        " temperature_c=%.1f"
        " relative_humidity_percent=%.1f"
        " dew_point_c=%.1f"
        " temperature_trend=%s"
        " temperature_change_c=%.2f\n",
        uptimeMs,
        static_cast<double>(temperatureC),
        static_cast<double>(relativeHumidityPercent),
        static_cast<double>(dewPointC),
        temperatureTrend,
        static_cast<double>(temperatureTrendChangeC)
    );
}

void loadMeasurementInterval() {
    Preferences preferences;

    if (!preferences.begin(kPreferencesNamespace, false)) {
        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=nvs_open_failed"
            " measurement_interval_ms=%lu"
            " source=default\n",
            static_cast<unsigned long>(millis()),
            static_cast<unsigned long>(
                measurementIntervalMs
            )
        );

        return;
    }

    const bool hasStoredInterval =
        preferences.isKey(kMeasurementIntervalKey);

    const char* source = "persisted";
    const char* status = "ok";

    if (hasStoredInterval) {
        const uint32_t storedIntervalMs =
            preferences.getUInt(
                kMeasurementIntervalKey,
                kDefaultMeasurementIntervalMs
            );

        if (isMeasurementIntervalValid(storedIntervalMs)) {
            measurementIntervalMs = storedIntervalMs;
        } else {
            measurementIntervalMs =
                kDefaultMeasurementIntervalMs;

            source = "default";
            status = "invalid_persisted_value";
        }

    } else {
        const size_t bytesWritten =
            preferences.putUInt(
                kMeasurementIntervalKey,
                kDefaultMeasurementIntervalMs
            );

        if (bytesWritten == sizeof(uint32_t)) {
            source = "default_initialized";
        } else {
            source = "default";
            status = "write_failed";
        }
    }

    preferences.end();

    Serial.printf(
        "TYTO_CONFIG uptime_ms=%lu"
        " status=%s"
        " measurement_interval_ms=%lu"
        " source=%s\n",
        static_cast<unsigned long>(millis()),
        status,
        static_cast<unsigned long>(
            measurementIntervalMs
        ),
        source
    );
}

bool saveMeasurementInterval(
    const uint32_t intervalMs
) {
    if (!isMeasurementIntervalValid(intervalMs)) {
        return false;
    }

    Preferences preferences;

    if (!preferences.begin(kPreferencesNamespace, false)) {
        return false;
    }

    const size_t bytesWritten =
        preferences.putUInt(
            kMeasurementIntervalKey,
            intervalMs
        );

    preferences.end();

    if (bytesWritten != sizeof(uint32_t)) {
        return false;
    }

    measurementIntervalMs = intervalMs;
    return true;
}

void printHistory() {
    if (!historyStorageAvailable) {
        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_unavailable\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    File historyFile =
        LittleFS.open(kHistoryFilePath, FILE_READ);

    if (!historyFile) {
        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_open_failed\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    Serial.println("TYTO_HISTORY_BEGIN");

    while (historyFile.available()) {
        Serial.write(historyFile.read());
    }

    Serial.println("TYTO_HISTORY_END");

    historyFile.close();
}

void handleSerialCommand(const char* command) {
    if (strcmp(command, "history") == 0) {
        printHistory();
        return;
    }

    const size_t prefixLength =
        strlen(kIntervalCommandPrefix);

    if (strncmp(
            command,
            kIntervalCommandPrefix,
            prefixLength
        ) != 0) {

        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=unknown_command\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    const char* valueText =
        command + prefixLength;

    char* endPointer = nullptr;

    const unsigned long parsedValue =
        strtoul(
            valueText,
            &endPointer,
            10
        );

    if (endPointer == valueText ||
        *endPointer != '\0') {

        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=invalid_command_value\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    const uint32_t intervalMs =
        static_cast<uint32_t>(parsedValue);

    if (!isMeasurementIntervalValid(intervalMs)) {
        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=interval_out_of_range"
            " minimum_ms=%lu"
            " maximum_ms=%lu\n",
            static_cast<unsigned long>(millis()),
            static_cast<unsigned long>(
                kMinimumMeasurementIntervalMs
            ),
            static_cast<unsigned long>(
                kMaximumMeasurementIntervalMs
            )
        );

        return;
    }

    if (!saveMeasurementInterval(intervalMs)) {
        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=save_failed\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    Serial.printf(
        "TYTO_CONFIG uptime_ms=%lu"
        " status=updated"
        " measurement_interval_ms=%lu\n",
        static_cast<unsigned long>(millis()),
        static_cast<unsigned long>(
            measurementIntervalMs
        )
    );
}

void processSerialInput() {
    while (Serial.available() > 0) {
        const char character =
            static_cast<char>(Serial.read());

        if (character == '\r') {
            continue;
        }

        if (character == '\n') {
            serialCommandBuffer[serialCommandLength] =
                '\0';

            if (serialCommandLength > 0) {
                handleSerialCommand(
                    serialCommandBuffer
                );
            }

            serialCommandLength = 0;
            return;
        }

        if (serialCommandLength <
            kSerialCommandBufferSize - 1) {

            serialCommandBuffer[serialCommandLength] =
                character;

            ++serialCommandLength;
        }
    }
}

void initializeHistoryStorage() {
    if (!LittleFS.begin(false)) {
        historyStorageAvailable = false;

        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=mount_failed\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    historyStorageAvailable = true;

    Serial.printf(
        "TYTO_STORAGE uptime_ms=%lu"
        " status=ready"
        " total_bytes=%llu"
        " used_bytes=%llu\n",
        static_cast<unsigned long>(millis()),
        static_cast<unsigned long long>(
            LittleFS.totalBytes()
        ),
        static_cast<unsigned long long>(
            LittleFS.usedBytes()
        )
    );
}

bool initializeHistoryFile() {
    if (!historyStorageAvailable) {
        return false;
    }

    if (LittleFS.exists(kHistoryFilePath)) {
        return true;
    }

    File historyFile =
        LittleFS.open(kHistoryFilePath, FILE_WRITE);

    if (!historyFile) {
        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_create_failed\n",
            static_cast<unsigned long>(millis())
        );

        return false;
    }

    historyFile.println(
        "uptime_ms,temperature_c,relative_humidity_percent"
    );

    historyFile.close();

    Serial.printf(
        "TYTO_STORAGE uptime_ms=%lu"
        " status=history_created"
        " path=%s\n",
        static_cast<unsigned long>(millis()),
        kHistoryFilePath
    );

    return true;
}

bool appendHistoryMeasurement(
    const uint32_t uptimeMs,
    const float temperatureC,
    const float relativeHumidityPercent
) {
    if (!historyStorageAvailable) {
        return false;
    }

    File historyFile =
        LittleFS.open(kHistoryFilePath, FILE_APPEND);

    if (!historyFile) {
        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_open_failed\n",
            static_cast<unsigned long>(uptimeMs)
        );

        return false;
    }

    historyFile.printf(
        "%lu,%.1f,%.1f\n",
        static_cast<unsigned long>(uptimeMs),
        static_cast<double>(temperatureC),
        static_cast<double>(relativeHumidityPercent)
    );

    historyFile.close();

    return true;
}

void printBoardInformation() {
    const String chipModel = ESP.getChipModel();

    Serial.println();
    Serial.println("================================");
    Serial.println("Tyto ESP32-S3 board bring-up");
    Serial.println("================================");

    Serial.printf("Chip model:       %s\n", chipModel.c_str());
    Serial.printf("Chip revision:    %u\n",
                  static_cast<unsigned>(ESP.getChipRevision()));
    Serial.printf("CPU cores:        %u\n",
                  static_cast<unsigned>(ESP.getChipCores()));
    Serial.printf("CPU frequency:    %u MHz\n",
                  static_cast<unsigned>(ESP.getCpuFreqMHz()));

    Serial.printf(
        "Flash detected:   %u MB\n",
        static_cast<unsigned>(
            ESP.getFlashChipSize() / kBytesPerMegabyte
        )
    );

    const bool hasPsram = psramFound();
    const uint32_t psramBytes = ESP.getPsramSize();

    Serial.printf(
        "PSRAM detected:   %s\n",
        hasPsram ? "yes" : "no"
    );

    Serial.printf(
        "PSRAM available:  %lu bytes (%.2f MiB)\n",
        static_cast<unsigned long>(psramBytes),
        static_cast<double>(psramBytes) / (1024.0 * 1024.0)
    );

    Serial.println("--------------------------------");

    constexpr uint32_t kMinimumExpectedPsram =
        7UL * kBytesPerMegabyte;

    if (ESP.getFlashChipSize() == 16UL * kBytesPerMegabyte &&
        hasPsram &&
        psramBytes >= kMinimumExpectedPsram) {
        Serial.println("RESULT: Board configuration looks correct.");
    } else {
        Serial.println("RESULT: Memory configuration needs checking.");
    }

    Serial.println("================================");
}

}  // namespace

void setup() {
    Serial.begin(115200);

    // Give the serial connection time to become available.
    delay(2000);

    printBoardInformation();

    loadMeasurementInterval();

    initializeHistoryStorage();
    initializeHistoryFile();

    climateSensor.begin();

    Serial.printf(
        "TYTO_SENSOR uptime_ms=%lu sensor=am2302"
        " state=driver_started gpio=%u"
        " measurement_interval_ms=%lu\n",
        static_cast<unsigned long>(millis()),
        static_cast<unsigned>(kDhtDataPin),
        static_cast<unsigned long>(measurementIntervalMs)
    );
}

void loop() {
    processSerialInput();

    const uint32_t nowMs = millis();

    if (!isMeasurementDue(nowMs)) {
        return;
    }

    const float relativeHumidityPercent =
        climateSensor.readHumidity();
    const float temperatureC =
        climateSensor.readTemperature();

    const unsigned long uptimeMs =
        static_cast<unsigned long>(nowMs);

    if (isnan(relativeHumidityPercent) ||
        isnan(temperatureC)) {

        if (!hasValidMeasurement) {
            printUnavailableReadError(uptimeMs);
        } else {
            const uint32_t lastValidAgeMs =
                nowMs - lastValidMeasurementMs;

            printStaleReadError(
                uptimeMs,
                lastValidAgeMs
            );
        }

        return;
    }

    if (!isMeasurementInRange(
            temperatureC,
            relativeHumidityPercent
        )) {

        if (!hasValidMeasurement) {
            printUnavailableInvalidData(
                uptimeMs,
                temperatureC,
                relativeHumidityPercent
            );
        } else {
            const uint32_t lastValidAgeMs =
                nowMs - lastValidMeasurementMs;

            printStaleInvalidData(
                uptimeMs,
                temperatureC,
                relativeHumidityPercent,
                lastValidAgeMs
            );
        }

        return;
    }

    addTemperatureSample(temperatureC);

    const float dewPointC =
        calculateDewPointC(
            temperatureC,
            relativeHumidityPercent
        );

    updateLastValidMeasurement(
        nowMs,
        temperatureC,
        relativeHumidityPercent,
        dewPointC
    );

    appendHistoryMeasurement(
        nowMs,
        temperatureC,
        relativeHumidityPercent
    );

    if (temperatureSampleCount < kTemperatureTrendSampleCount) {
        printCollectingMeasurement(
            uptimeMs,
            temperatureC,
            relativeHumidityPercent,
            dewPointC
        );

        return;
    }

    const float temperatureTrendChangeC =
        calculateTemperatureTrendChangeC();

    const char* temperatureTrend =
        getTemperatureTrend(temperatureTrendChangeC);

    printClimateMeasurement(
        uptimeMs,
        temperatureC,
        relativeHumidityPercent,
        dewPointC,
        temperatureTrend,
        temperatureTrendChangeC
    );
}
