#include <Arduino.h>
#include <DHT.h>

namespace {

constexpr uint32_t kBytesPerMegabyte = 1024UL * 1024UL;
constexpr uint32_t kMeasurementIntervalMs = 5000;
constexpr uint8_t kDhtDataPin = 4;
constexpr uint8_t kDhtType = DHT22;

constexpr float kMinimumTemperatureC = -40.0F;
constexpr float kMaximumTemperatureC = 80.0F;
constexpr float kMinimumRelativeHumidityPercent = 0.0F;
constexpr float kMaximumRelativeHumidityPercent = 100.0F;

DHT climateSensor(kDhtDataPin, kDhtType);

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

    climateSensor.begin();

    Serial.printf(
        "TYTO_SENSOR uptime_ms=%lu sensor=am2302"
        " state=driver_started gpio=%u\n",
        static_cast<unsigned long>(millis()),
        static_cast<unsigned>(kDhtDataPin)
    );
}

void loop() {
    const float relativeHumidityPercent =
        climateSensor.readHumidity();
    const float temperatureC =
        climateSensor.readTemperature();

    const unsigned long uptimeMs =
        static_cast<unsigned long>(millis());

    if (isnan(relativeHumidityPercent) ||
        isnan(temperatureC)) {
        Serial.printf(
            "TYTO_ENV uptime_ms=%lu sensor=am2302"
            " status=read_error\n",
            uptimeMs
        );
    } else if (!isMeasurementInRange(
                   temperatureC,
                   relativeHumidityPercent
               )) {
        Serial.printf(
            "TYTO_ENV uptime_ms=%lu sensor=am2302"
            " status=invalid_data"
            " temperature_c=%.1f"
            " relative_humidity_percent=%.1f\n",
            uptimeMs,
            static_cast<double>(temperatureC),
            static_cast<double>(relativeHumidityPercent)
        );
    } else {
        Serial.printf(
            "TYTO_ENV uptime_ms=%lu sensor=am2302"
            " status=ok"
            " temperature_c=%.1f"
            " relative_humidity_percent=%.1f\n",
            uptimeMs,
            static_cast<double>(temperatureC),
            static_cast<double>(relativeHumidityPercent)
        );
    }

    delay(kMeasurementIntervalMs);
}