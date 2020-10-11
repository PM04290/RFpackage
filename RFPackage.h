/*
   Copyright (C) 2020  Philippe MARCEL
   
   V02.1

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   
   V02.1: tag issue
   V02  : implement Tag for RFID and  encryption
   V01  : implement VirualWireEx for ESP8266
   V00  : initial
*/
#ifndef RFPACKAGE_H_
#define RFPACKAGE_H_

#include <Arduino.h>
#include <VirtualWireEx.h>

typedef enum {
  S_BINARYSENSOR    = 0,
  S_SWITCH          = 1,
  S_LIGHT           = 2,
  S_SENSOR          = 3,
  S_DIMMER          = 4,
  S_COVER           = 5,
  S_INPUTBOOLEAN    = 6,
  S_INPUTNUMBER     = 7,
  S_INPUTTEXT       = 8,
  S_CUSTOM          = 9,
  S_TAG				= 10
} rfp_sensor_t; // limited to 32

//
typedef enum {
  V_BOOL      = 0,
  V_NUM       = 1,
  V_FLOAT     = 2,
  V_TEXT      = 3,
  V_TAG		  = 4
} rfp_data_t; // limited to 7


#define MAX_PACKET_TEXT_LEN 8
typedef struct __attribute__((packed)) {
  uint8_t destinationID;
  uint8_t senderID;
  uint8_t childID;
  uint8_t sensordataType;
  union {
    struct {
      int32_t value;
      uint16_t divider;
      uint8_t precision;
    } num;
    char text[MAX_PACKET_TEXT_LEN];
	struct {
	  uint32_t tagH;
	  uint32_t tagL;
	} tag;
  } data;
} Tpackage;

#define PACKET_SIZE sizeof(Tpackage)

class RFpackage
{
  public:
    RFpackage();
	
	void initRF(uint16_t speed = 2000, uint8_t rxPin = 2, uint8_t txPin = 3, uint8_t pttPin = 4, bool pttInverted = false);
    // Receiver
    bool RFincomming();
    bool isRFforMe(byte sensorid);
    bool DecodeData(byte* destination);

    // Transmitter
    void setTransmitter(byte outputPin);
    void PublishNum(byte destinationid, byte senderid, byte childid, rfp_sensor_t sensortype, const long value);
    void PublishFloat(byte destinationid, byte senderid, byte childid, rfp_sensor_t sensortype, const long value, const int divider, const byte precision);
    void PublishText(byte destinationid, byte senderid, byte childid, rfp_sensor_t sensortype, const char* text);
    void PublishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL);

    // General
    byte getDestinationID();
    byte getSenderID();
    byte getChildID();
    long getValueNum();
    float getValueFloat();
	uint32_t getValueTagH();
	uint32_t getValueTagL();
    byte getValueText(char* output);
    byte getPrecision();
    rfp_sensor_t getSensorType();
    rfp_data_t getDataType();

  private:
    Tpackage pack;
    Tpackage lastpack;
};

#endif
