# Room climate experiment evaluation

## Results

The 72-hour run showed repeated short temperature drops followed by recoveries.

The typical short cooling period lasted about 11 minutes and changed by about 0.9 °C. The typical recovery lasted about 10 minutes and changed by about 1.0 °C.

Using the candidate rule from the cycle analysis, the dataset contained:

- 101 candidate cooling periods
- 109 candidate recovery periods
- 91 complete candidate cycles

The dataset also contains much slower changes, including the larger day and night pattern.

## What was learned

Tyto can pick out repeated short room-climate cycles from stored temperature history with a simple offline rule.

The cycles are large and long enough to separate from small sample-to-sample fluctuations in this dataset.

The current rule is useful for describing candidate cooling and recovery periods, but it has not been validated as an AC detector.

## Uncertainty

The experiment did not record confirmed AC on/off times.

Because of that, the detected cycles cannot be treated as confirmed AC activity. False detections and missed AC events cannot be measured from this run.

Some detected cycles may come from other room conditions or from the larger daily temperature pattern.

## Sensor placement

Before the run, a placement test showed that keeping the SHT31 close to the laptop raised the measured temperature by about 2 °C. The sensor was then moved to the edge of the desk, away from the laptop and outside direct AC airflow.

The measurements still represent one point in the room. A different sensor position could show different timing or temperature changes.

## Run limitations

There was one brief power interruption during the experiment. The ESP32 restarted and continued collecting under a new boot ID.

The device does not have a real-time clock, so the exact wall-clock duration of the interruption is unknown.

The cycle analysis was run afterward in Python. No cycle-detection rule was added to the ESP32 firmware in v0.5.0.

## Next step

A future test that records the actual AC on/off times could compare those events with the detected cycles.

That would make it possible to measure false detections, missed events, and timing accuracy.
