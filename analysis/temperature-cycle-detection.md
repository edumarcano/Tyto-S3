# Temperature cycle detection

The 72-hour dataset contains repeated short temperature drops followed by recoveries.

To find these cycles, the analysis tracks changes in direction rather than reacting to individual samples.

A temperature reversal of 0.4 °C is used to ignore small fluctuations and identify larger turning points.

A candidate cooling period is defined as:

- a temperature drop of at least 0.7 °C
- completed within 30 minutes

A candidate recovery period uses the same limits in the opposite direction.

Using those rules, the dataset contains:

- 101 candidate cooling periods
- 109 candidate recovery periods
- 91 complete cooling and recovery cycles

The typical short cycle is around 10 minutes in each direction, with about 0.9-1.0 °C of temperature change.

Longer temperature changes also appear in the dataset. These are likely part of the larger room and day/night pattern rather than the shorter repeating cycles.

These results only describe measured room-climate behavior. The experiment did not record confirmed AC on/off times, so the detected cycles should not be treated as confirmed AC activity.