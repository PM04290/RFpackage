/*
   tested on Arduino Nano
   tested on Sparkfun Pro Micro
*/
#include <RFPackage.h>
#include <ArduinoJson.h>

#define PIN_RF_PTT       2
#define PIN_RF_OUTPUT    3
#define PIN_RF_INPUT     4

RFpackage rfpack;

unsigned long upTime = 0;
unsigned long nbRF_in = 0;
unsigned long nbRF_out = 0;
unsigned long uptimeMn = 0;

void setup() {
  Serial.begin(57600);
  rfpack.initRF(2000, PIN_RF_INPUT, PIN_RF_OUTPUT, PIN_RF_PTT, false);
}


char topic[50];
char payload[50];
String serialInput;
bool serialReady = false;
StaticJsonDocument<200> doc;

unsigned long curtime, oldtime = 0;
byte ntick = 0;
void loop() {
  curtime = millis();
  if (curtime > oldtime + 60000) {
    uptimeMn++;
    ntick++;
    if (ntick >= 5) {
      sprintf(payload,"\"payload\": {\"uptime\": %lu, \"RFin\": %lu, \"RFout\": %lu}", uptimeMn, nbRF_in, nbRF_out);
      Serial.print("{");
      Serial.print("\"topic\":\"serial2mqtt/bridge\"");
      Serial.print(", ");
      Serial.print(String(payload));
      Serial.println("}");
      ntick = 0;
    }
    oldtime = curtime;
  }

  if (rfpack.isRFforMe(0x00)) {
    nbRF_in++;
    digitalWrite(13, 1);
    byte sender = rfpack.getSenderID();
    byte child = rfpack.getChildID();
    sprintf(topic, "serial2mqtt/s%02d_c%02d", sender, child);
    strcpy(payload, "0");
    switch (rfpack.getSensorType()) {
      case S_BINARYSENSOR:
      case S_SWITCH:
      case S_LIGHT:
        strcpy(payload, String(rfpack.getValueNum()).c_str());
        strcat(topic, "/state");
        if (rfpack.getValueNum() > 0) {
          strcpy(payload, "ON");
        } else {
          strcpy(payload, "OFF");
        }
        break;
      default:
        strcat(topic, "/value");
        switch (rfpack.getDataType()) {
          case V_NUM:
            strcpy(payload, String(rfpack.getValueNum()).c_str());
            break;
          case V_FLOAT:
            strcpy(payload, String(rfpack.getValueFloat(), rfpack.getPrecision()).c_str());
            break;
          case V_TEXT:
            rfpack.getValueText(payload);
            break;
          case V_TAG:
            uint32_t v0, v1;
            v0 = rfpack.getValueTagH();
            v1 = rfpack.getValueTagL();
            sprintf(payload, "%0lX_%0lX", v0, v1);
            break;
        }
        break;
    }
    Serial.println("{\"topic\":\"" + String(topic) + "\", \"payload\":\"" + String(payload) + "\"}");
    digitalWrite(13, 0);
    
  } else
  if (serialReady) {
    doSerial(serialInput);
    serialInput = "";
    serialReady = false;

  } else
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      serialReady = true;
    } if (c == '\r') {
      //none
    } else {
      serialInput += c;
    }
  }
}

void doSerial(String json) {
  // {"topic": "serial2mqtt/s20_c03/set", "payload":"1"}
  DeserializationError error = deserializeJson(doc, json);

  strcpy(topic, doc["topic"]);
  strcpy(payload, doc["payload"]);
  //
  if (getStringPartByNr(String(topic), '/', 0) == "serial2mqtt") {
    // Node detection
    String Node = getStringPartByNr(String(topic), '/', 1);
    int sensor;
    int child;
    int n = sscanf(Node.c_str(), "s%02d_c%02d", &sensor, &child);
    // Cmd parser
    String Cmd = getStringPartByNr(String(topic), '/', 2);
    if (Cmd == "set") {
      rfp_sensor_t sensorType = S_BINARYSENSOR;
      long value = 0;
      String sval = "";
      if (payload[0] == '{') {
        // TODO
      } else if (payload[0] >= '0' && payload[0] <= '9') {
        value = atoi((char*)payload);
        sensorType = S_INPUTNUMBER;
      } else {
        sval = String((char*)payload);
        if (sval == "ON" || sval == "on" || sval == "OPEN" || sval == "open") {
          value = 1;
        } else if (sval == "OFF" || sval == "off" || sval == "CLOSE" || sval == "close") {
          value = 0;
        } else if (sval == "STOP" || sval == "stop") {
          sensorType = S_COVER;
          value = 2;
        }
      }
      rfpack.PublishNum(sensor, 0, child, sensorType, value);
      nbRF_out++;
    }
  }
}

String getStringPartByNr(String data, char separator, int index)
{
  int stringData = 0;        //variable to count data part nr
  String dataPart = "";      //variable to hole the return text

  for (int i = 0; i < data.length(); i++) { //Walk through the text one letter at a time
    if (data[i] == separator) {
      //Count the number of times separator character appears in the text
      stringData++;
    } else if (stringData == index) {
      //get the text when separator is the rignt one
      dataPart.concat(data[i]);
    } else if (stringData > index) {
      //return text and stop if the next separator appears - to save CPU-time
      return dataPart;
    }
  }
  return dataPart;
}
