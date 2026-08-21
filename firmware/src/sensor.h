#pragma once

#include <cstdint>

struct ClimateSensorReading {
    bool readSucceeded;
    float temperatureC;
    float relativeHumidityPercent;
};

bool initializeClimateSensor(uint32_t measurementIntervalMs);

ClimateSensorReading readClimateSensor();