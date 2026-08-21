#include "history.h"

#include "config.h"

#include <Arduino.h>
#include <LittleFS.h>

namespace {

constexpr char kHistoryFilePath[] = "/history.csv";
constexpr char kPreviousHistoryFilePath[] = "/history-old.csv";

constexpr size_t kMaximumHistoryFileBytes =
    2UL * 1024UL * 1024UL;

bool historyStorageAvailable = false;

bool createHistoryFile() {
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
        "boot_id,uptime_ms,temperature_c,"
        "relative_humidity_percent,dew_point_c,"
        "temperature_trend,temperature_change_c"
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

bool rotateHistoryFile() {
    if (!historyStorageAvailable) {
        return false;
    }

    if (LittleFS.exists(kPreviousHistoryFilePath)) {
        if (!LittleFS.remove(kPreviousHistoryFilePath)) {
            Serial.printf(
                "TYTO_STORAGE uptime_ms=%lu"
                " status=history_old_remove_failed\n",
                static_cast<unsigned long>(millis())
            );

            return false;
        }
    }

    if (!LittleFS.rename(
            kHistoryFilePath,
            kPreviousHistoryFilePath
        )) {

        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_rotate_failed\n",
            static_cast<unsigned long>(millis())
        );

        return false;
    }

    if (!createHistoryFile()) {
        return false;
    }

    Serial.printf(
        "TYTO_STORAGE uptime_ms=%lu"
        " status=history_rotated\n",
        static_cast<unsigned long>(millis())
    );

    return true;
}

void printHistory(const char* historyFilePath) {
    if (!historyStorageAvailable) {
        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_unavailable\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    File historyFile =
        LittleFS.open(historyFilePath, FILE_READ);

    if (!historyFile) {
        Serial.printf(
            "TYTO_STORAGE uptime_ms=%lu"
            " status=history_open_failed"
            " path=%s\n",
            static_cast<unsigned long>(millis()),
            historyFilePath
        );

        return;
    }

    Serial.printf(
        "TYTO_HISTORY_BEGIN path=%s\n",
        historyFilePath
    );

    while (historyFile.available()) {
        Serial.write(historyFile.read());
    }

    Serial.printf(
        "TYTO_HISTORY_END path=%s\n",
        historyFilePath
    );

    historyFile.close();
}

}  // namespace

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

    return createHistoryFile();
}

void printCurrentHistory() {
    printHistory(kHistoryFilePath);
}

void printPreviousHistory() {
    printHistory(kPreviousHistoryFilePath);
}

bool clearHistory() {
    if (!historyStorageAvailable) {
        return false;
    }

    if (LittleFS.exists(kHistoryFilePath)) {
        if (!LittleFS.remove(kHistoryFilePath)) {
            return false;
        }
    }

    if (LittleFS.exists(kPreviousHistoryFilePath)) {
        if (!LittleFS.remove(kPreviousHistoryFilePath)) {
            return false;
        }
    }

    return createHistoryFile();
}

bool appendHistoryMeasurement(
    const uint32_t uptimeMs,
    const float temperatureC,
    const float relativeHumidityPercent,
    const float dewPointC,
    const char* temperatureTrend,
    const bool hasTemperatureTrendChange,
    const float temperatureTrendChangeC
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

    if (historyFile.size() >= kMaximumHistoryFileBytes) {
        historyFile.close();

        if (!rotateHistoryFile()) {
            return false;
        }

        historyFile =
            LittleFS.open(kHistoryFilePath, FILE_APPEND);

        if (!historyFile) {
            Serial.printf(
                "TYTO_STORAGE uptime_ms=%lu"
                " status=history_open_failed\n",
                static_cast<unsigned long>(uptimeMs)
            );

            return false;
        }
    }

    if (hasTemperatureTrendChange) {
        historyFile.printf(
            "%lu,%lu,%.1f,%.1f,%.1f,%s,%.2f\n",
            static_cast<unsigned long>(getBootId()),
            static_cast<unsigned long>(uptimeMs),
            static_cast<double>(temperatureC),
            static_cast<double>(relativeHumidityPercent),
            static_cast<double>(dewPointC),
            temperatureTrend,
            static_cast<double>(temperatureTrendChangeC)
        );
    } else {
        historyFile.printf(
            "%lu,%lu,%.1f,%.1f,%.1f,%s,\n",
            static_cast<unsigned long>(getBootId()),
            static_cast<unsigned long>(uptimeMs),
            static_cast<double>(temperatureC),
            static_cast<double>(relativeHumidityPercent),
            static_cast<double>(dewPointC),
            temperatureTrend
        );
    }

    historyFile.close();
    return true;
}
