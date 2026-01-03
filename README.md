# 4-digit-7-segment-display-playground
This is a bit rough program which you could use to understand how 4 digit 7 segment display is working.

## Usefull instructions
1. **segmentPins** represent pins on Arduino, so either connect in similar manner or change the numbers appropriately based on the comments for segments.
2. **digitPins** also represent pins on Arduino for digits from 1 to 4.
3. Change **COMMON_CATHODE** to false if you are having a common anode display, meaning that digit pins on your display are anodes.
4. You don't need to change **interDigitDelayMs_ON** nor **interDigitDelayMs_OFF**. You just need to change **interDigitDelayEnabled** boolean value if you want to see how 4 digit displays are working. Set it to true if you want to see how they are in fact working and set it to false for normal behaviour.
5. Use **displayNumber** function to output your desired number.