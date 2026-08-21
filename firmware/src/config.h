#pragma once

#include <cstdint>

void loadMeasurementInterval();

bool isMeasurementIntervalValid(uint32_t intervalMs);

bool saveMeasurementInterval(uint32_t intervalMs);

uint32_t getMeasurementIntervalMs();

uint32_t getMinimumMeasurementIntervalMs();

uint32_t getMaximumMeasurementIntervalMs();

bool initializeBootId();

uint32_t getBootId();