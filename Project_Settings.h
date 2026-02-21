#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

// --- ACS724LLCTR-2P5AB-S SPECIFIC CONSTANTS ---
const float SENSITIVITY = 800.0;    // 800 mV/A per datasheet
const float VCC_SYSTEM = 5.0; 

// --- PIN MAPPING FOR ARDUINO MEGA ---
const int sensorPins[3] = {A0, A1, A2}; 

// 18 Relays: 3 Strings, 6 Panels each
const int relayPins[3][6] = {
  {22, 23, 24, 25, 26, 27}, // String 1
  {28, 29, 30, 31, 32, 33}, // String 2
  {34, 35, 36, 37, 38, 39}  // String 3
};

#endif