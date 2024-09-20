// LINES 108 & 109 ARE UNTESTED IN INTEGRATED TEST

#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <SD.h>

// GPIO definitions
#define RX_RYLR       7
#define TX_RYLR       6
#define RX_RS232      8
#define TX_RS232      9
#define D4184A        4
#define D4184B        5
#define INBUILT_LED   13

// State Machine Definition
typedef enum {
  SAFE,
  ARMED,
  LAUNCHED,
  FAILURE
} STATE;
STATE currentState = SAFE;

// Object Instantiations
SoftwareSerial RYLR(RX_RYLR, TX_RYLR);
AltSoftSerial RS232;

// Variable Definitions
File logFile;
float loadCellReading;
float tareValue = 0, startTime = 0, timeStamp;
String currState, response, weightString;

// Prototype Function Definitions
void getTareValue() {
  if (RS232.available()) {
    tareValue = RS232.parseFloat();
  }
}

void getData() {
  loadCellReading = RS232.parseFloat();
  float weight = loadCellReading - tareValue;
  timeStamp = (millis() - startTime)/1000;
  weightString = String(timeStamp)+":"+String(weight);
  Serial.println(weightString);
}

void logData() {
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  logFile.println(weightString);
  logFile.close();
}

void sendData() {
  String transmit = "AT+SEND=0," + String(weightString.length()) + "," + weightString + "\r\n";
  RYLR.print(transmit);
  delay(10);
}

String parseRYLR(String input) {
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  String parsed = input.substring(start, end);
  parsed.trim();
  return parsed;
}

void sendState(String currState) {
  currState = "TESTBED STATE: " + currState;
  currState = "AT+SEND=0,"+ String(currState.length()) + "," + currState + "\r\n";
  RYLR.print(currState);
  delay(10);
}

void checkInput(String receive) {
  if (receive == "ARM" && currentState == SAFE) {
    Serial.println("CURRENT STATE: ARMED.");
    currentState = ARMED;
    sendState("ARMED");
    getTareValue();
    startTime = millis();
    return;
  }
  else if (receive == "DISARM" && currentState == ARMED) {
    Serial.println("CURRENT STATE: SAFE. ");
    currentState = SAFE;
    sendState("SAFE");
    return;
  }
  else if (receive == "LAUNCH" && currentState == ARMED) {
    Serial.println("CURRENT STATE: LAUNCHED. ");
    currentState = LAUNCHED;
    sendState("LAUNCHED");
    return;
  }
  else {
    //Serial.println("INVALID INPUT PROVIDED. ");
    return;
  }
}

void performOperations() {
  switch(currentState) {
    case SAFE:
      break;
    case ARMED:
      getData(); // These 2 lines are untested
      logData(); // Test before test day
      digitalWrite(INBUILT_LED, ~digitalRead(INBUILT_LED));
      delay(100);
      break;
    case LAUNCHED:
      digitalWrite(INBUILT_LED, HIGH);
      digitalWrite(D4184A, HIGH);
      digitalWrite(D4184B, HIGH);
      delay(2000);
      digitalWrite(D4184A, LOW);
      digitalWrite(D4184B, LOW);

      while(1) {
        getData();
        logData();
        sendData();
      }
      break;
    case FAILURE:
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(D4184A, OUTPUT);
  pinMode(D4184B, OUTPUT);
  digitalWrite(D4184A, LOW);
  digitalWrite(D4184B, LOW);

  pinMode(INBUILT_LED, OUTPUT);
  digitalWrite(INBUILT_LED, LOW);

  //RYLR setup
  RYLR.begin(57600);

  //Load Cell setup
  RS232.begin(9600);
  Serial.println("\nLoad Cell Comm. Initialised.");

  //SD Card setup
  Serial.println("Serial Comm. Initialised.");
  if (!SD.begin()) {
    Serial.println("SD card initialisation failed.");
    return;
  }
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("Couldn't open log file");
  } 
  else {
    Serial.println("Logging to SD card...\n");
  }

  //Tare the Loadcell
  getTareValue();

  Serial.println("TESTBED SETUP COMPLETE.");

}

void loop() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    response = parseRYLR(response);
    Serial.println("RESPONSE: " + response);
    checkInput(response);
    performOperations();
  }
}
