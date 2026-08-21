# Room climate experiment evaluation

## Results

The 72-hour run showed repeated short temperature drops followed by recoveries.

The typical short cooling period lasted about 11 minutes and changed by about 0.9 °C.

The typical recovery lasted about 10 minutes and changed by about 1.0 °C.

Using the candidate rule from the cycle analysis, the dataset contained:

- 101 candidate cooling periods
- 109 candidate recovery periods
- 91 complete candidate cycles

The dataset also contains much slower changes, including the larger day and night pattern.

## What the experiment supports

The data supports the idea that Tyto can identify repeated short room-climate cycles using only temperature history.

The cycles are large and long enough to separate from small sample-to-sample fluctuations.

The current rule is useful for describing candidate cooling and recovery periods in this dataset.

## Uncertainty

The experiment did not record confirmed AC on/off times.

Because of this, the detected cycles cannot be treated as confirmed AC activity. False detections and missed AC events cannot be measured from this run.

Some detected cycles may be caused by other room conditions or by the larger daily temperature pattern.

## Sensor placement

The sensor was placed away from the laptop and outside direct AC airflow after testing showed that nearby electronics could raise the measured temperature by about 2 °C.

The measurements still represent one point in the room. Different sensor locations may show different timing or temperature changes.

## Run limitations

There was one brief power interruption during the experiment. The ESP32 restarted and continued collecting under a new boot ID.

The device does not have an RTC, so the exact wall-clock duration of the interruption is unknown.

## Next step

A future labeled test should record known AC on/off times and compare them with the detected cycles.

That would allow the rule to be evaluated for false detections, missed events, and timing accuracy.