#pragma once

#include <cstdint>

void printUnavailableReadError(
    unsigned long uptimeMs
);

void printStaleReadError(
    unsigned long uptimeMs,
    uint32_t lastValidAgeMs,
    float lastValidTemperatureC,
    float lastValidRelativeHumidityPercent,
    float lastValidDewPointC
);

void printUnavailableInvalidData(
    unsigned long uptimeMs,
    float temperatureC,
    float relativeHumidityPercent
);

void printStaleInvalidData(
    unsigned long uptimeMs,
    float temperatureC,
    float relativeHumidityPercent,
    uint32_t lastValidAgeMs,
    float lastValidTemperatureC,
    float lastValidRelativeHumidityPercent,
    float lastValidDewPointC
);

void printCollectingMeasurement(
    unsigned long uptimeMs,
    float temperatureC,
    float relativeHumidityPercent,
    float dewPointC
);

void printClimateMeasurement(
    unsigned long uptimeMs,
    float temperatureC,
    float relativeHumidityPercent,
    float dewPointC,
    const char* temperatureTrend,
    float temperatureTrendChangeC
);
