#include "sensor.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

namespace {

constexpr uint8_t kI2cSdaPin = 8;
constexpr uint8_t kI2cSclPin = 9;
constexpr uint8_t kSht31Address = 0x44;

Adafruit_SHT31 climateSensor = Adafruit_SHT31();

}  // namespace

bool initializeClimateSensor(
    const uint32_t measurementIntervalMs
) {
    Wire.begin(kI2cSdaPin, kI2cSclPin);

    const bool sensorInitialized =
        climateSensor.begin(kSht31Address);

    Serial.printf(
        "TYTO_SENSOR uptime_ms=%lu sensor=sht31"
        " state=%s"
        " address=0x%02X"
        " sda_gpio=%u"
        " scl_gpio=%u"
        " measurement_interval_ms=%lu\n",
        static_cast<unsigned long>(millis()),
        sensorInitialized ? "ready" : "init_failed",
        static_cast<unsigned>(kSht31Address),
        static_cast<unsigned>(kI2cSdaPin),
        static_cast<unsigned>(kI2cSclPin),
        static_cast<unsigned long>(measurementIntervalMs)
    );

    return sensorInitialized;
}

ClimateSensorReading readClimateSensor() {
    ClimateSensorReading reading = {};

    reading.relativeHumidityPercent =
        climateSensor.readHumidity();

    reading.temperatureC =
        climateSensor.readTemperature();

    reading.readSucceeded =
        !isnan(reading.relativeHumidityPercent) &&
        !isnan(reading.temperatureC);

    return reading;
}
