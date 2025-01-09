#include <AltSoftSerial.h>

AltSoftSerial ucComm;
String input, response;

void setup() {
  Serial.begin(9600);
  ucComm.begin(9600);

  Serial.println("SETUP COMPLETE");
}

void loop() {
  if(Serial.available()) {
    input = Serial.readStringUntil('\n');
    ucComm.print("UNO: ");
    ucComm.println(input);
    Serial.println("UNO: " + input);
  }
  if(ucComm.available()) {
    response = ucComm.readStringUntil('\n');
    Serial.println(response);
  }
}
