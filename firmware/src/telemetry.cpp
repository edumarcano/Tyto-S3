#include "telemetry.h"

#include <Arduino.h>

void printUnavailableReadError(
    const unsigned long uptimeMs
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=sht31"
        " status=read_error"
        " data_status=unavailable\n",
        uptimeMs
    );
}

void printStaleReadError(
    const unsigned long uptimeMs,
    const uint32_t lastValidAgeMs,
    const float lastValidTemperatureC,
    const float lastValidRelativeHumidityPercent,
    const float lastValidDewPointC
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=sht31"
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
        "TYTO_ENV uptime_ms=%lu sensor=sht31"
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
    const uint32_t lastValidAgeMs,
    const float lastValidTemperatureC,
    const float lastValidRelativeHumidityPercent,
    const float lastValidDewPointC
) {
    Serial.printf(
        "TYTO_ENV uptime_ms=%lu sensor=sht31"
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
        static_cast<double>(
            lastValidRelativeHumidityPercent
        ),
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
        "TYTO_ENV uptime_ms=%lu sensor=sht31"
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
        "TYTO_ENV uptime_ms=%lu sensor=sht31"
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
