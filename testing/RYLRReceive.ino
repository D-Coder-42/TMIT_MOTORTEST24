#include <SoftwareSerial.h>

#define RX_RYLR 7
#define TX_RYLR 6

SoftwareSerial RYLR (RX_RYLR, TX_RYLR);
String response;

String parseRYLR(String input) {
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  String parsed = input.substring(start, end);
  parsed.trim();
  return parsed;  
}

void receiveData() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    response = parseRYLR(response);
  }
}

void setup() {
  Serial.begin(9600);
  RYLR.begin(57600);
}

void loop() {
  receiveData();
  Serial.println("DATA RECEIVED: " + response);
}
