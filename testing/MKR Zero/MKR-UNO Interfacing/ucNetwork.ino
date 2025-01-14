#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#define RX 7
#define TX 6

SoftwareSerial RYLR (RX, TX);
AltSoftSerial ucComm;

String transmit;
String input, response;

void sendRYLR(String data) {
  transmit = "AT+SEND=0," + String(data.length()) + "," + data + "\r\n";
  RYLR.print(transmit);
  delay(10); // Delay for proper transmission
}

void sendData() {
  if(Serial.available()) {
    input = Serial.readStringUntil('\n');
    ucComm.print("UNO: ");
    ucComm.println(input);
    sendRYLR(input);
    Serial.println("UNO: " + input);
  }
}

String parseRYLR(String input) {
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  String parsed = input.substring(start, end);
  parsed.trim();
  return parsed;  
}

void receiveRYLR() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    response = parseRYLR(response);
    Serial.println("DATA RECEIVED: " + response);
    ucComm.print("GROUND: ");
    ucComm.print(response);
  }
}

void receiveMKR() {
  if(ucComm.available()) {
    response = ucComm.readStringUntil('\n');
    Serial.println(response);
    sendRYLR(response);
  }
}

void setup() {
  Serial.begin(9600);
  ucComm.begin(9600);
  RYLR.begin(57600);
  Serial.println("SETUP COMPLETE");
}

void loop() {
  sendData();
  receiveRYLR();
  receiveMKR();
}
