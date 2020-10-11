/*
   http://digistump.com/package_digistump_index.json

   PIN config for Digispark (ATTiny85)
*/
#include <RFPackage.h>

#define SENSOR_ID       0x09

#define PIN_RF_OUTPUT   0
#define PIN_RF_PTT      1
#define PIN_RF_INPUT    3

RFpackage rfpack;


#define DEFAULT_PERIOD  50
#define INITIAL_PERIOD  5

// Définition du nombre d'échantillons
// pour le calcul de la moyenne glissante
const int nEchantillons = 20;

int echantillon[nEchantillons];    // un tableau pour stocker les échantillons lus
int indice = 0;                    // l'indice de l'échantillon courant
long total = 0;                    // la somme des échantillons mémorisés
int periode = INITIAL_PERIOD;

void setup() {
  // Initialisation des pins
  pinMode(A1, INPUT);
  rfpack.initRF(2000, PIN_RF_INPUT, PIN_RF_OUTPUT, PIN_RF_PTT, false);

  for (int i = 0; i < nEchantillons; i++) {
    echantillon[i] = 0;
  }
}

int counter = 0;
byte oldstate = 99;
byte ledstate = 0;
int AnaValue = 0;
int OldAna = -1;

void loop() {
  // receive activation switch
  if (rfpack.isRFforMe(SENSOR_ID)) {
    if (rfpack.getChildID() == 1) {
      ledstate = rfpack.getValueNum() > 0 ? 1 : 0;
    }
  }
  
  delay(100);
  counter++;
  // Every 5s
  if ((counter % periode) == 0) {
    total = total - echantillon[indice];
    // Lecture du capteur
    echantillon[indice] = analogRead(A1);
    // Ajout du dernier echantillon
    total = total + echantillon[indice];

    // Incrémentation de l'indice
    indice++;
    // si on est à la fin du tableau ...
    if (indice >= nEchantillons) {
      // ...retour au début
      indice = 0;
      periode = DEFAULT_PERIOD;
    }

    // calcul de la moyenne
    AnaValue = total / nEchantillons;
    //AnaValue = analogRead(1);
  }
  if (oldstate != ledstate) {
    rfpack.PublishNum(0, SENSOR_ID, 1, S_BINARYSENSOR, ledstate);
    oldstate = ledstate;
  }
  if ((AnaValue != OldAna) && (periode == DEFAULT_PERIOD)) {
    rfpack.PublishNum(0, SENSOR_ID, 2, S_SENSOR, AnaValue);
    OldAna = AnaValue;
  }
}
