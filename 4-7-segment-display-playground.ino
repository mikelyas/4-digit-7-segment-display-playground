/* 4-digit 7-seg (COMMON CATHODE) with toggleable INTER-DIGIT DELAY
 * - Resistors on segment lines (a..g, dp)
 * - Per-digit multiplexing with optional delay between one digit OFF and the next digit ON
 *
 * Segment pins (a..g,dp): 2..9
 * Digit select pins (D1..D4): 10, 11, 12, 13
 */


//////////////////// CONFIG ////////////////////
const bool COMMON_CATHODE = true;

const uint8_t segmentPins[8] = {
  2,  // a
  3,  // b
  4,  // c
  5,  // d
  6,  // e
  7,  // f
  8,  // g
  9   // dp
};

const uint8_t digitPins[4] = { 10, 11, 12, 13 }; // D1..D4

const uint32_t onTimeMicros = 10000;

// INTER-DIGIT delay
const uint32_t interDigitDelayMs_ON  = 1000; // 1000 ms (very visible)
const uint32_t interDigitDelayMs_OFF = 0;    // no delay (looks continuous)
bool interDigitDelayEnabled = true;

///////////////////////////////////////////////

// Segment bit positions (a..g,dp)
enum Seg { SEG_A=0, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP };

// Segment pattern for 0..9
const uint8_t DIGITS[10] = {
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F),              // 0
  (1<<SEG_B)|(1<<SEG_C),                                                          // 1
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_G)|(1<<SEG_E)|(1<<SEG_D),                         // 2
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_G)|(1<<SEG_C)|(1<<SEG_D),                         // 3
  (1<<SEG_F)|(1<<SEG_G)|(1<<SEG_B)|(1<<SEG_C),                                    // 4
  (1<<SEG_A)|(1<<SEG_F)|(1<<SEG_G)|(1<<SEG_C)|(1<<SEG_D),                         // 5
  (1<<SEG_A)|(1<<SEG_F)|(1<<SEG_G)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E),              // 6
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C),                                               // 7
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G),   // 8
  (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_F)|(1<<SEG_G)               // 9
};

volatile uint8_t value[4] = {0, 0, 0, 0};
volatile uint8_t dpMask   = 0x0; // set bit i to enable dp on digit i

uint8_t currentDigit = 0;

void setup() {
  // IO setup
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    writeSegment(i, false);
  }
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    enableDigit(i, false);
  }
}

void loop() {
  // --- update the number every 250 ms ---
  static unsigned long lastTick = 0;
  if (millis() - lastTick >= 250) {
    lastTick = millis();
    displayNumber(1111);
  }

  serviceNextDigit();
}

// ========== core logic ==========
void serviceNextDigit() {
  // Disable the current digit
  enableDigit(currentDigit, false);

  // Optional dark gap between digits
  uint32_t gap = interDigitDelayEnabled ? interDigitDelayMs_ON
                                        : interDigitDelayMs_OFF;
  if (gap) {
    delay(gap);
  }

  // Move to the next digit (0..3)
  currentDigit = (currentDigit + 1) & 0x03;

  // Update segment lines while no digit is enabled
  uint8_t segmentPattern = DIGITS[value[currentDigit] % 10];
  bool dot = (dpMask >> currentDigit) & 0x01;
  setSegmentPattern(segmentPattern, dot);

  // Enable the new digit and hold for visible ON-time
  enableDigit(currentDigit, true);
  delayMicroseconds(onTimeMicros);
}

// ========== helpers ==========
void writeSegment(uint8_t segIndex, bool on) {
  // COMMON CATHODE: segment ON = HIGH, OFF = LOW
  digitalWrite(segmentPins[segIndex], (on ^ COMMON_CATHODE) ? LOW : HIGH);
}

void enableDigit(uint8_t idx, bool enable) {
  // COMMON CATHODE: digit ON = LOW, OFF = HIGH
  digitalWrite(digitPins[idx], (enable ^ COMMON_CATHODE) ? HIGH : LOW);
}

void setSegmentPattern(uint8_t mask, bool dot) {
  for (uint8_t s = 0; s < 7; s++) {
    bool on = mask & (1 << s);
    writeSegment(s, on);
  }
  writeSegment(SEG_DP, dot);
}

void displayNumber(int n) {
  if (n < 0) n = 0;
  if (n > 9999) n = 9999;
  value[0] = (n / 1000) % 10;
  value[1] = (n / 100)  % 10;
  value[2] = (n / 10)   % 10;
  value[3] = (n / 1)    % 10;
}
