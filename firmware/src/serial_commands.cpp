#include "serial_commands.h"

#include "config.h"
#include "history.h"

#include <Arduino.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {

constexpr size_t kSerialCommandBufferSize = 32;
constexpr char kIntervalCommandPrefix[] = "interval ";

char serialCommandBuffer[kSerialCommandBufferSize] = {};
size_t serialCommandLength = 0;

void handleSerialCommand(const char* command) {
    if (strcmp(command, "history") == 0) {
        printCurrentHistory();
        return;
    }

    if (strcmp(command, "history old") == 0) {
        printPreviousHistory();
        return;
    }

    if (strcmp(command, "history clear") == 0) {
        if (clearHistory()) {
            Serial.printf(
                "TYTO_STORAGE uptime_ms=%lu"
                " status=history_cleared\n",
                static_cast<unsigned long>(millis())
            );
        } else {
            Serial.printf(
                "TYTO_STORAGE uptime_ms=%lu"
                " status=history_clear_failed\n",
                static_cast<unsigned long>(millis())
            );
        }

        return;
    }

    const size_t prefixLength =
        strlen(kIntervalCommandPrefix);

    if (strncmp(
            command,
            kIntervalCommandPrefix,
            prefixLength
        ) != 0) {

        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=unknown_command\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    const char* valueText =
        command + prefixLength;

    char* endPointer = nullptr;

    const unsigned long parsedValue =
        strtoul(
            valueText,
            &endPointer,
            10
        );

    if (endPointer == valueText ||
        *endPointer != '\0') {

        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=invalid_command_value\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    const uint32_t intervalMs =
        static_cast<uint32_t>(parsedValue);

    if (!isMeasurementIntervalValid(intervalMs)) {
        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=interval_out_of_range"
            " minimum_ms=%lu"
            " maximum_ms=%lu\n",
            static_cast<unsigned long>(millis()),
            static_cast<unsigned long>(
                getMinimumMeasurementIntervalMs()
            ),
            static_cast<unsigned long>(
                getMaximumMeasurementIntervalMs()
            )
        );

        return;
    }

    if (!saveMeasurementInterval(intervalMs)) {
        Serial.printf(
            "TYTO_CONFIG uptime_ms=%lu"
            " status=save_failed\n",
            static_cast<unsigned long>(millis())
        );

        return;
    }

    Serial.printf(
        "TYTO_CONFIG uptime_ms=%lu"
        " status=updated"
        " measurement_interval_ms=%lu\n",
        static_cast<unsigned long>(millis()),
        static_cast<unsigned long>(
            getMeasurementIntervalMs()
        )
    );
}

}  // namespace

void processSerialInput() {
    while (Serial.available() > 0) {
        const char character =
            static_cast<char>(Serial.read());

        if (character == '\r') {
            continue;
        }

        if (character == '\n') {
            serialCommandBuffer[serialCommandLength] =
                '\0';

            if (serialCommandLength > 0) {
                handleSerialCommand(
                    serialCommandBuffer
                );
            }

            serialCommandLength = 0;
            return;
        }

        if (
            serialCommandLength <
            kSerialCommandBufferSize - 1
        ) {
            serialCommandBuffer[serialCommandLength] =
                character;

            ++serialCommandLength;
        }
    }
}
