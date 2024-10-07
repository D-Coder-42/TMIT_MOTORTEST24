#include <SD.h>
#include <SoftwareSerial.h>

// GPIO definitions
#define RX_RYLR       3
#define TX_RYLR       4
#define ARM_SW        2
#define LCH_SW        7
// #define RYLR          Serial1

SoftwareSerial RYLR (RX_RYLR, TX_RYLR);

// State Machine Definition
typedef enum {
  SAFE,
  ARMED,
  LAUNCHED
} STATE;
STATE currentState = SAFE;

// Variable Definitions
File logFile;
String message, response;
static int safeFlag = 0, armFlag = 0, launchFlag = 0;
float calRead;

String parseRYLR(String input) {
  int start = input.indexOf(',') + 1;
  start = input.indexOf(',', start) + 1;
  int end = input.indexOf(',', start);
  String parsed = input.substring(start, end);
  parsed.trim();
  return parsed;  
}

void sendState(String data) {
  Serial.println("TRANSMIT: " + data);
  message = "AT+SEND=0,"+ String(data.length()) + "," + data + "\r\n";
  RYLR.flush();
  RYLR.print(message);
  delay(10);
}

void checkTestbed() {
  if (RYLR.available()) {
    response = RYLR.readStringUntil('\n');
    response = parseRYLR(response);
    if (response.equals("TESTBED STATE: SAFE")) {
      currentState = SAFE;
      Serial.println("GROUND STATE: SAFE");
    }
    else if (response.equals("TESTBED STATE: ARMED")) {
      currentState = ARMED;
      Serial.println("GROUND STATE: ARMED");
    }
    else if (response.equals("TESTBED STATE: LAUNCHED")) {
      currentState = LAUNCHED;
      Serial.println("GROUND STATE: LAUNCHED");
    }
    if (response.length() > 3) {
      Serial.println(response);
      logData();
    }
  }
}

void logData() {
  //SD card store
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  logFile.println(response);
  logFile.close();
}

void checkInput() {
  switch (currentState) {
    case SAFE:
      if (digitalRead(ARM_SW) == HIGH && digitalRead(LCH_SW) == LOW) {
        if(safeFlag == 0) {
          sendState("ARM");
          delay(500);
          safeFlag = 1;
          armFlag = 0;
        }
      }
      break;
    case ARMED:
      if (digitalRead(ARM_SW) == LOW) {
        if(armFlag == 0) {
          sendState("DISARM");
          delay(500);
          armFlag = 1;
          safeFlag = 0;
        }
      }
      else if(digitalRead(LCH_SW) == HIGH) {
        if(launchFlag == 0) {
          sendState("LAUNCH");
          delay(500);
          launchFlag = 1;
        }
      }
      break;
    case LAUNCHED:
        while(1) {
          checkTestbed();
        }
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(ARM_SW, INPUT);
  pinMode(LCH_SW, INPUT);
  digitalWrite(ARM_SW, LOW);
  digitalWrite(LCH_SW, LOW);

  //RYLR setup
  RYLR.begin(57600);

  //SD Card setup
  Serial.println("\nSerial Comm. Initialised.");
  if (!SD.begin()) {
    Serial.println("SD card initialisation failed.");
    // return;
  }
  logFile = SD.open("loadcell.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("Couldn't open log file");
  } 
  else {
    Serial.println("Logging to SD card...");
  }
  Serial.println("GROUNDSTATION SET UP COMPLETE.");

  // Serial.println("CALIBRATING TESTBED...");
  // Serial.println("\nDO NOT CHANGE WEIGHT ON LOAD CELL");
  // Serial.print("Enter reading on weight indicator: ");
  // while(!Serial.available());
  // calRead = Serial.parseFloat();
  // Serial.println(calRead);
  // sendState(String(calRead));
}

void loop() {
  checkInput();
  checkTestbed();
}
