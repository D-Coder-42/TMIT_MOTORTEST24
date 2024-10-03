#include <SoftwareSerial.h>

#define RX_RYLR 7
#define TX_RYLR 6

SoftwareSerial RYLR (RX_RYLR, TX_RYLR);
String data,transmit;

void sendData(String data) {
  transmit = "AT+SEND=0," + String(data.length()) + "," + data + "\r\n";
  RYLR.print(transmit);
  delay(10);
}

void setup() {
  Serial.begin(9600);
  RYLR.begin(57600);
}

void loop() {
  if(Serial.available()) {
    data = Serial.readStringUntil('\n');
    sendData(data);
    Serial.println("TRANSMITTED: " + transmit);
  }
}
