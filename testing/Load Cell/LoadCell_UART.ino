#include <SoftwareSerial.h>

#define RX_RS232 8
#define TX_RS232 9

float tareValue = 0;
float read, weight;

SoftwareSerial RS232 (RX_RS232, TX_RS232);

void setup() 
{
  Serial.begin(19200);
  RS232.begin(19200);
  Serial.println("\nSETUP COMPLETE.");
  Serial.println("=========================\n");
}

void loop() 
{
  if (RS232.available())
  {
    read = RS232.parseFloat();
    weight = read - tareValue;
    Serial.println("Load Cell Data: "+ String(weight) + " kg");
  }
  if (Serial.available())
  {
    if (Serial.readStringUntil('\n') == "TARE")
    {
      // This is for software tare (does not reflect on weight indicator)
      tareValue = read;
      Serial.println("\n______________________________");
      Serial.println("Tare value updated to: "+ String(tareValue) + " kg");
      Serial.println("______________________________\n");
    }
  }
 }
