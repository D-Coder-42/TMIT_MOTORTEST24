#define LOAD A0

unsigned long int read, startTime, timeStamp;
float tareValue = 0.0, loadCellReading, weight;
float calRead, calFactor = 1.0;
String weightString;

float mapVtoKG(unsigned long int read) {
  return (calFactor*read*500.0)/1024.0;
}

void getCalibrationFactor() {
  Serial.println("\nDO NOT CHANGE WEIGHT ON LOAD CELL");
  Serial.print("Enter reading on weight indicator: ");
  while(!Serial.available());
  calRead = Serial.parseFloat();
  Serial.println(calRead);
  if(calRead == 0) return;
  Serial.print("Value according to analog input: ");
  getData();
  calFactor = (float)calRead/(float)loadCellReading;
  Serial.println("Calibration factor: " + String(calFactor));
  Serial.println("CALIBRATION COMPLETE\n");
}

void getTareValue() {
  tareValue = analogRead(LOAD);
  tareValue = mapVtoKG(tareValue);
}

void getData() {
  loadCellReading = analogRead(LOAD);
  loadCellReading = mapVtoKG(loadCellReading);
  float weight = loadCellReading - tareValue;
  timeStamp = (millis() - startTime)/1000;
  weightString = String(timeStamp)+" : "+String(weight);
  Serial.println(weightString);
}

void setup() {
  Serial.begin(19200);
  pinMode(LOAD, INPUT);

  getCalibrationFactor();
  getTareValue();
  startTime = millis();

  Serial.println("DATA DISPLAYED IN FORMAT: 'TIMESTAMP : DATA'");
}

void loop() {
  getData();
}
