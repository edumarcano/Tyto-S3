#pragma once

#include <cstdint>

void initializeHistoryStorage();

bool initializeHistoryFile();

void printCurrentHistory();

void printPreviousHistory();

bool clearHistory();

bool appendHistoryMeasurement(
    uint32_t uptimeMs,
    float temperatureC,
    float relativeHumidityPercent,
    float dewPointC,
    const char* temperatureTrend,
    bool hasTemperatureTrendChange,
    float temperatureTrendChangeC
);
