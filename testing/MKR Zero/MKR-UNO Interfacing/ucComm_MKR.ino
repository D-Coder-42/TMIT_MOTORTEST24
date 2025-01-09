String input, response;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  delay(5000); // for 'setup complete' to print
  Serial.println("SETUP COMPLETE");
}

void loop() {
  if(Serial.available()) {
    input = Serial.readStringUntil('\n');
    Serial1.print("MRKZERO: ");
    Serial1.println(input);
    Serial.println("MKRZERO: " + input);
  }
  if(Serial1.available()) {
    response = Serial1.readStringUntil('\n');
    Serial.println(response);
  }
}
