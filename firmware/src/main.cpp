#include <Arduino.h>
#include <DHT.h>

namespace {

constexpr uint32_t kBytesPerMegabyte = 1024UL * 1024UL;
constexpr uint32_t kMeasurementIntervalMs = 5000;
constexpr uint8_t kDhtDataPin = 4;
constexpr uint8_t kDhtType = DHT22;

DHT climateSensor(kDhtDataPin, kDhtType);

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
        "AM2302 driver started on GPIO %u. \n",
        static_cast<unsigned>(kDhtDataPin)
    );
}

void loop() {
    const float relativeHumidityPercent = climateSensor.readHumidity();
    const float temperatureC = climateSensor.readTemperature();

    if (isnan(relativeHumidityPercent) || isnan(temperatureC)) {
        Serial.printf(
            "AM2302 read failed | uptime: %lu seconds\n",
            static_cast<unsigned long>(millis() / 1000UL)
        );
    } else {
        Serial.printf(
            "Tyto environment | uptime: %lu seconds"
            " | temperature: %.1f C"
            " | relative humidity: %.1f %%\n",
            static_cast<unsigned long>(millis() / 1000UL),
            static_cast<double>(temperatureC),
            static_cast<double>(relativeHumidityPercent)
        );
    }

    delay(kMeasurementIntervalMs);
}