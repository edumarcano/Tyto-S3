#include "config.h"

#include <Arduino.h>
#include <Preferences.h>

namespace {

constexpr uint32_t kDefaultMeasurementIntervalMs = 5000;
constexpr uint32_t kMinimumMeasurementIntervalMs = 2000;
constexpr uint32_t kMaximumMeasurementIntervalMs = 60000;

constexpr char kPreferencesNamespace[] = "tyto";
constexpr char kMeasurementIntervalKey[] = "measure_ms";
constexpr char kBootIdKey[] = "boot_id";

uint32_t measurementIntervalMs =
    kDefaultMeasurementIntervalMs;

uint32_t bootId = 0;

}  // namespace

bool isMeasurementIntervalValid(const uint32_t intervalMs) {
    return intervalMs >= kMinimumMeasurementIntervalMs &&
           intervalMs <= kMaximumMeasurementIntervalMs;
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

uint32_t getMeasurementIntervalMs() {
    return measurementIntervalMs;
}

uint32_t getMinimumMeasurementIntervalMs() {
    return kMinimumMeasurementIntervalMs;
}

uint32_t getMaximumMeasurementIntervalMs() {
    return kMaximumMeasurementIntervalMs;
}

bool initializeBootId() {
    Preferences preferences;

    if (!preferences.begin(kPreferencesNamespace, false)) {
        Serial.printf(
            "TYTO_BOOT uptime_ms=%lu"
            " status=nvs_open_failed\n",
            static_cast<unsigned long>(millis())
        );

        return false;
    }

    const uint32_t previousBootId =
        preferences.getUInt(kBootIdKey, 0);

    const uint32_t nextBootId =
        previousBootId + 1;

    const size_t bytesWritten =
        preferences.putUInt(
            kBootIdKey,
            nextBootId
        );

    preferences.end();

    if (bytesWritten != sizeof(uint32_t)) {
        Serial.printf(
            "TYTO_BOOT uptime_ms=%lu"
            " status=write_failed\n",
            static_cast<unsigned long>(millis())
        );

        return false;
    }

    bootId = nextBootId;

    Serial.printf(
        "TYTO_BOOT uptime_ms=%lu"
        " status=ready"
        " boot_id=%lu\n",
        static_cast<unsigned long>(millis()),
        static_cast<unsigned long>(bootId)
    );

    return true;
}

uint32_t getBootId() {
    return bootId;
}
