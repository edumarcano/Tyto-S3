#include "climate.h"

#include <cmath>
#include <cstddef>

namespace {

constexpr size_t kTemperatureTrendSampleCount = 6;
constexpr float kTemperatureTrendThresholdC = 0.2F;

constexpr float kMinimumTemperatureC = -40.0F;
constexpr float kMaximumTemperatureC = 80.0F;
constexpr float kMinimumRelativeHumidityPercent = 0.0F;
constexpr float kMaximumRelativeHumidityPercent = 100.0F;

float recentTemperaturesC[kTemperatureTrendSampleCount] = {};
size_t temperatureSampleCount = 0;

}  // namespace

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
        std::log(relativeHumidityPercent / 100.0F) +
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

bool isTemperatureTrendReady() {
    return temperatureSampleCount >= kTemperatureTrendSampleCount;
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
