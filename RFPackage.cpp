#include "RFPackage.h"

RFpackage::RFpackage()
{

}

void RFpackage::initRF(uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted)
{
  vw_set_rx_pin(rxPin);
  vw_set_tx_pin(txPin);
  vw_set_ptt_pin(pttPin);
  vw_set_ptt_inverted(pttInverted);
  vw_setup(speed);
  vw_rx_start();
}

bool RFpackage::RFincomming() {
  return vw_have_message();
}

bool RFpackage::DecodeData(byte* destination) {
  static long nextallowed = 0;
  byte buf[PACKET_SIZE];
  byte len = PACKET_SIZE;

  if (vw_get_message(buf, &len)) {
    if (len == PACKET_SIZE) {
	  if ( (memcmp(buf,&pack,PACKET_SIZE) != 0) || (millis() > nextallowed) ) {
        memcpy(&pack, buf, len);
        *destination = pack.destinationID;
		nextallowed = millis()+500;
        return true;
	  }
    }
  }
  return false;
}

bool RFpackage::isRFforMe(byte sensorid) {
  byte dest = 0;
  if (vw_have_message()) {
	  if (DecodeData(&dest)) {
		  return dest == sensorid;
	  }
  }
  return false;
}

byte RFpackage::getDestinationID() {
  return pack.destinationID;
}

byte RFpackage::getSenderID() {
  return pack.senderID;
}

byte RFpackage::getChildID() {
  return pack.childID;
}

rfp_sensor_t RFpackage::getSensorType() {
  return (rfp_sensor_t)(pack.sensordataType >> 3);
}

rfp_data_t RFpackage::getDataType() {
  return (rfp_data_t)(pack.sensordataType & 0x07);
}

long RFpackage::getValueNum() {
  return pack.data.num.value;
}

float RFpackage::getValueFloat() {
  if (pack.data.num.divider != 0) {
    return (float)pack.data.num.value / (float)pack.data.num.divider;
  } else {
    return 0;
  }
}

byte RFpackage::getValueText(char* output) {
  pack.data.text[MAX_PACKET_TEXT_LEN-1] = 0;
  strcpy(output, pack.data.text);
  return strlen(output);
}

uint32_t RFpackage::getValueTagH() {
  return pack.data.tag.tagH;
}

uint32_t RFpackage::getValueTagL() {
  return pack.data.tag.tagL;
}

byte RFpackage::getPrecision() {
  return pack.data.num.precision;
}

void RFpackage::PublishNum(byte destinationid, byte senderid, byte childid, rfp_sensor_t sensortype, const long value) {
  pack.destinationID = destinationid;
  pack.senderID = senderid;
  pack.childID = childid;
  pack.sensordataType = (sensortype << 3) + V_NUM;
  pack.data.num.value = value;
  pack.data.num.divider = 1;

  uint8_t n = 2;
  do {
	if (vw_send((uint8_t*)&pack, PACKET_SIZE)) {
		vw_wait_tx();
	}
  } while (n-- > 0);
}

void RFpackage::PublishFloat(byte destinationid, byte senderid, byte childid, rfp_sensor_t sensortype, const long value, const int divider, const byte precision) {
  pack.destinationID = destinationid;
  pack.senderID = senderid;
  pack.childID = childid;
  pack.sensordataType = (sensortype << 3) + V_FLOAT;
  pack.data.num.value = value;
  pack.data.num.divider = divider;
  pack.data.num.precision = precision;

  uint8_t n = 2;
  do {
	  if (vw_send((uint8_t*)&pack, PACKET_SIZE)) {
		vw_wait_tx();
	  }
  } while (n-- > 0);
}
void RFpackage::PublishText(byte destinationid, byte senderid, byte childid, rfp_sensor_t sensortype, const char* text) {
  byte l = strlen(text);
  if (l > MAX_PACKET_TEXT_LEN) {
    l = MAX_PACKET_TEXT_LEN;
  }
  pack.destinationID = destinationid;
  pack.senderID = senderid;
  pack.childID = childid;
  pack.sensordataType = (sensortype << 3) + V_TEXT;
  strncpy(pack.data.text, text, l);

  uint8_t n = 2;
  do {
	  if (vw_send((uint8_t*)&pack, PACKET_SIZE)) {
		vw_wait_tx();
	  }
  } while (n-- > 0);
}

void RFpackage::PublishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL) {
  pack.destinationID = destinationid;
  pack.senderID = senderid;
  pack.childID = childid;
  pack.sensordataType = (S_TAG << 3) + V_TAG;
  pack.data.tag.tagH = tagH;
  pack.data.tag.tagL = tagL;

  uint8_t n = 2;
  do {
	if (vw_send((uint8_t*)&pack, PACKET_SIZE)) {
		vw_wait_tx();
	}
  } while (n-- > 0);
}

