#include <HX711.h>

#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2

HX711 scale;
String ch;

float calWeight, calRead, calFactor;
float weight;

void setup() {
  Serial.begin(9600);
  Serial.println("\nHX711 Calibration and Read Test");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(1); //calibration_factor 1 for now
  scale.set_offset((long)(-scale.get_units()));  

  Serial.print("Put constant weight on Load Cell");
  Serial.print("Enter weight on Load Cell (in g): ");
  while(!Serial.available());
  calWeight = Serial.parseFloat();
  Serial.println(String(calWeight) + " g");
  calRead = scale.get_units();

  calFactor = calRead / calWeight;
  scale.set_scale(calFactor);

  Serial.println("Remove weight from Load Cell");
  Serial.println("Enter 'Y' when done");
  do {
    while(!Serial.available());
    ch = Serial.readStringUntil('\n');
    if (ch == "Y") {
      scale.tare();
      break;
    }
  } while (true);

  Serial.println("\n_________________________");
  Serial.println("HX711 Calibration Details");
  Serial.println("SCALE = "+ String(scale.get_scale()));
  Serial.println("OFFSET = "+ String(scale.get_offset()));
  Serial.println("_________________________\n");
}

void loop() {
  weight = scale.get_units();
  Serial.println(String(weight) + " g");
}
