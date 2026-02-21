#include "Project_Settings.h"
#include "Fault_Algorithm.h"

float qvo_voltages[3] = {2.54, 2.54, 2.54}; //

void setup() {
  Serial.begin(9600);
  initializeRelays();
  calibrateSensors(); // Establish zero baseline
  Serial.println("--- Mega System Online: 18 Panels ---");
}

void loop() {
  for (int s = 0; s < 3; s++) {
    float current = readStringCurrent(s);
    
    Serial.print("Str_"); Serial.print(s + 1);
    Serial.print(":"); Serial.print(current, 3);
    Serial.print(" ");

    // If current drops below 10mA, trigger the algorithm
    if (current < 0.01) { 
      runFaultDetection(s); 
    }
  }
  Serial.println();
  delay(1000);
}

// --- HARDWARE HELPERS ---
void initializeRelays() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 6; j++) {
      pinMode(relayPins[i][j], OUTPUT);
      digitalWrite(relayPins[i][j], LOW); // Panels active by default
    }
  }
}

void calibrateSensors() {
  Serial.println("Calibrating sensors...");
  for (int s = 0; s < 3; s++) {
    long sum = 0;
    for (int i = 0; i < 200; i++) {
      sum += analogRead(sensorPins[s]);
      delay(2);
    }
    qvo_voltages[s] = ((float)sum / 200 / 1024.0) * VCC_SYSTEM;
  }
}

float readStringCurrent(int stringIdx) {
  long rawSum = 0;
  for (int i = 0; i < 50; i++) {
    rawSum += analogRead(sensorPins[stringIdx]);
    delayMicroseconds(500); 
  }
  float voltage = ((float)rawSum / 50 / 1024.0) * VCC_SYSTEM;
  return (voltage - qvo_voltages[stringIdx]) / (SENSITIVITY / 1000.0);
}

void setRelay(int string, int panel, bool bypass) {
  digitalWrite(relayPins[string][panel], bypass ? HIGH : LOW);
}