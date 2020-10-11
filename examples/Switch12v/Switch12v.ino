/*
   http://digistump.com/package_digistump_index.json
*/
#include <RFPackage.h>

#define SENSOR_ID       9

#define PIN_RF_OUTPUT   -1
#define PIN_RF_PTT      -1
#define PIN_RF_INPUT    0

#define PIN_RELAY_1     1
#define PIN_RELAY_3     2

RFpackage rfpack;

void setup() {
  // Initialisation des pins
  pinMode(PIN_RELAY_1, OUTPUT);

  digitalWrite(PIN_RELAY_1, LOW);

  rfpack.initRF(2000, PIN_RF_INPUT, PIN_RF_OUTPUT, PIN_RF_PTT, false);
}

uint8_t curState, oldState = 0;
void loop() {
  if (rfpack.isRFforMe(SENSOR_ID)) {
    if (rfpack.getChildID() == PIN_RELAY_1) {
      curState = rfpack.getValueNum() > 0 ? HIGH : LOW;
      digitalWrite(PIN_RELAY_1,curState);
    }
  }
}
