#pragma once

bool isMeasurementInRange(
    float temperatureC,
    float relativeHumidityPercent
);

float calculateDewPointC(
    float temperatureC,
    float relativeHumidityPercent
);

void addTemperatureSample(float temperatureC);

bool isTemperatureTrendReady();

float calculateTemperatureTrendChangeC();

const char* getTemperatureTrend(float temperatureChangeC);
