#ifndef FAULT_ALGORITHM_H
#define FAULT_ALGORITHM_H

#include <Arduino.h>
#include "Project_Settings.h"

// Exists in Ver_2.ino
extern float qvo_voltages[3];

// Forward declarations for functions defined in Ver_2.ino
float readStringCurrent(int stringIdx);
void setRelay(int string, int panel, bool bypass);

// Tracks which panels have been permanently bypassed across loop iterations
bool faultedPanels[3][6] = {};

/**
 * @brief Isolates which panel(s) are causing a zero-current fault.
 *        Phase 1: Tests each panel individually (bypass one, check current).
 *        Phase 2: If Phase 1 fails, tests all pairs of panels (C(6,2) = 15 combos).
 *        Faulty panels are left bypassed permanently for the session.
 * @param stringIdx The index (0-2) of the string that lost current.
 */
void runFaultDetection(int stringIdx) {
  Serial.print("CRITICAL: Zero current detected on String ");
  Serial.println(stringIdx + 1);
  Serial.println("  Starting fault isolation...");

  // --- PHASE 1: Single panel bypass ---
  for (int p = 0; p < 6; p++) {
    if (faultedPanels[stringIdx][p]) continue; // Already bypassed, skip

    Serial.print("  Testing Panel ");
    Serial.print(p + 1);
    Serial.print("...");

    setRelay(stringIdx, p, true);
    delay(300);
    float current = readStringCurrent(stringIdx);

    if (current >= 0.01) {
      faultedPanels[stringIdx][p] = true;
      Serial.print(" RESTORED (");
      Serial.print(current, 3);
      Serial.println(" A)");
      Serial.print("FAULT ISOLATED: String ");
      Serial.print(stringIdx + 1);
      Serial.print(", Panel ");
      Serial.print(p + 1);
      Serial.println(" is FAULTY. Bypass engaged permanently.");
      return;
    }

    setRelay(stringIdx, p, false);
    Serial.print(" No change (");
    Serial.print(current, 3);
    Serial.println(" A)");
    delay(100);
  }

  // --- PHASE 2: Panel pair bypass ---
  Serial.println("  Phase 1 failed. Testing panel pairs...");

  for (int p1 = 0; p1 < 5; p1++) {
    if (faultedPanels[stringIdx][p1]) continue;

    for (int p2 = p1 + 1; p2 < 6; p2++) {
      if (faultedPanels[stringIdx][p2]) continue;

      Serial.print("  Testing Panels ");
      Serial.print(p1 + 1);
      Serial.print("+");
      Serial.print(p2 + 1);
      Serial.print("...");

      setRelay(stringIdx, p1, true);
      setRelay(stringIdx, p2, true);
      delay(300);
      float current = readStringCurrent(stringIdx);

      if (current >= 0.01) {
        faultedPanels[stringIdx][p1] = true;
        faultedPanels[stringIdx][p2] = true;
        Serial.print(" RESTORED (");
        Serial.print(current, 3);
        Serial.println(" A)");
        Serial.print("FAULT ISOLATED: String ");
        Serial.print(stringIdx + 1);
        Serial.print(", Panels ");
        Serial.print(p1 + 1);
        Serial.print("+");
        Serial.print(p2 + 1);
        Serial.println(" are FAULTY. Both bypassed permanently.");
        return;
      }

      setRelay(stringIdx, p1, false);
      setRelay(stringIdx, p2, false);
      Serial.print(" No change (");
      Serial.print(current, 3);
      Serial.println(" A)");
      delay(100);
    }
  }

  // --- FALLBACK: Could not isolate ---
  Serial.print("ERROR: Could not isolate fault on String ");
  Serial.println(stringIdx + 1);
  Serial.println("  Wiring or 3+ panel failure suspected. Manual inspection required.");
}

#endif
