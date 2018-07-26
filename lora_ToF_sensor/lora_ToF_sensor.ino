#include "Adafruit_VL53L0X.h"
#include <MKRWAN.h>

// 868MHz
_lora_band region = EU868;

// LoRa modem of the MKR WAN 1300
LoRaModem modem(Serial1);

// the VL53L0X object
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// credentials for joining the TTN network
const char *appEui = "*************";
const char *appKey = "****************************";

// send and max range values
int send_value = 0;
int max_range = 1200;

// the delay value in seconds
int delay_value = 30;

void setup()
{
  // Beginning Serial for debugging
  Serial.begin(115200);

  Serial.println("Starting...");

  // starting the modem up
  if(!modem.begin(region)) {
    Serial.println("Failed to start module LoRa");
    while(1);
  };

  // print the EUI for setup debug
  Serial.print("EUI: ");Serial.println(modem.deviceEUI());

  // connect and get the status of the connection
  int connected = modem.joinOTAA(appEui, appKey);
  if(!connected) {
    Serial.println("Something went wrong...");
    while(1);
  }

  Serial.println("Joined the network!");

  // enabling ADR and setting low spreading factor
  Serial.println("Enabling ADR and setting low spreading factor");
  modem.setADR(true);
  modem.dataRate(5);

  // connecting to the VL53L0X
  if (!lox.begin()) {
    Serial.println("Failed to boot VL53L0X");
    while(1);
  }
}

void loop()
{
  // value where the VL53L0X measurement will be stored
  VL53L0X_RangingMeasurementData_t measure;

  // reading the sensor
  lox.rangingTest(&measure, false);

  // if it is out of range
  if (measure.RangeStatus != 4) { 
    send_value = measure.RangeMilliMeter; // setting the send_value to the read measurement in milimeters
  } else {
    Serial.println("Out of range!!! Sending max range value!");
    send_value = max_range;               // setting the send value to max range
  }

  // debugging 
  Serial.println(send_value);

  // beginning packet and setting the send_value into it
  modem.beginPacket();
  modem.print(send_value);
  
  int err = modem.endPacket(false);

  // checking for error
  if (err > 0) {
    Serial.println("Success!");
  } else {
    Serial.println("Error");
  }

  // the delay (1000ms * 30) -> 30 seconds
  delay(1000 * delay_value);
}
