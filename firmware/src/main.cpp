#include <Arduino.h>
#include "climate.h"
#include "config.h"
#include "history.h"
#include "serial_commands.h"
#include "sensor.h"
#include "telemetry.h"

namespace {

constexpr uint32_t kBytesPerMegabyte = 1024UL * 1024UL;

uint32_t lastMeasurementMs = 0;

bool hasValidMeasurement = false;
uint32_t lastValidMeasurementMs = 0;

float lastValidTemperatureC = 0.0F;
float lastValidRelativeHumidityPercent = 0.0F;
float lastValidDewPointC = 0.0F;

bool isMeasurementDue(const uint32_t nowMs) {
    if (
        nowMs - lastMeasurementMs <
        getMeasurementIntervalMs()
    ) {
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
    initializeBootId();

    initializeHistoryStorage();
    initializeHistoryFile();

    initializeClimateSensor(
        getMeasurementIntervalMs()
    );
}

void loop() {
    processSerialInput();

    const uint32_t nowMs = millis();

    if (!isMeasurementDue(nowMs)) {
        return;
    }

    const ClimateSensorReading sensorReading =
        readClimateSensor();

    const float temperatureC =
        sensorReading.temperatureC;

    const float relativeHumidityPercent =
        sensorReading.relativeHumidityPercent;

    const unsigned long uptimeMs =
        static_cast<unsigned long>(nowMs);

    if (!sensorReading.readSucceeded) {

        if (!hasValidMeasurement) {
            printUnavailableReadError(uptimeMs);
        } else {
            const uint32_t lastValidAgeMs =
                nowMs - lastValidMeasurementMs;

            printStaleReadError(
                uptimeMs,
                lastValidAgeMs,
                lastValidTemperatureC,
                lastValidRelativeHumidityPercent,
                lastValidDewPointC
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
                lastValidAgeMs,
                lastValidTemperatureC,
                lastValidRelativeHumidityPercent,
                lastValidDewPointC
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

    if (!isTemperatureTrendReady()) {
        appendHistoryMeasurement(
            nowMs,
            temperatureC,
            relativeHumidityPercent,
            dewPointC,
            "collecting",
            false,
            0.0F
        );

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

    appendHistoryMeasurement(
        nowMs,
        temperatureC,
        relativeHumidityPercent,
        dewPointC,
        temperatureTrend,
        true,
        temperatureTrendChangeC
    );

    printClimateMeasurement(
        uptimeMs,
        temperatureC,
        relativeHumidityPercent,
        dewPointC,
        temperatureTrend,
        temperatureTrendChangeC
    );
}
