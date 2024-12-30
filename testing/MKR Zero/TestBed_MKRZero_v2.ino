#include <SD.h>

// GPIO definitions
#define D4184A        3
#define D4184B        5
#define LOAD          A5
#define PRESSURE      A3

// State Machine Definition
typedef enum {
  SAFE,
  ARMED,
  LAUNCHED,
  FAILURE
} STATE;
STATE currentState = SAFE;

// Object Instantiations
File logFile;
String response;

// Variable Definitions
int pressureRead, loadCellRead;
unsigned char* pressureArray;
unsigned char* loadCellArray;

const int maxSize = 6000;  // Total buffer size for each array
int pressureIndex = 0, pressureLogs = 0, pressureDelay;     // Tracks data size for each array
int loadCellIndex = 0, loadCellLogs = 0, loadCellDelay;

// Prototype Function Definitions
void dataInit() {
  analogReadResolution(12);

  // Allocate memory for the arrays
  pressureArray = (unsigned char*)malloc(maxSize * sizeof(unsigned char));
  loadCellArray = (unsigned char*)malloc(maxSize * sizeof(unsigned char));

  // Check for successful memory allocation
  if (!pressureArray || !loadCellArray) {
    Serial.println("Memory allocation failed!");
    while (1);  // Halt execution if memory allocation fails
  }

  // Initialize arrays with null terminators
  pressureArray[0] = '\0';
  loadCellArray[0] = '\0';
}

void appendData(unsigned char* buffer, int* index, int* log, int value, int* delay) {
  // Estimate space needed to store the integer and a space
  int currTime = millis();
  int neededSpace = snprintf(NULL, 0, "%d &d ", currTime - *delay, value);
  
  // Ensure enough space to add new data
  if (*index + neededSpace >= maxSize - 1) {
    Serial.println("Buffer overflow detected!");
    return;
  }

  // Append formatted data to the buffer
  snprintf((char*)(buffer + *index), maxSize - *index, "%d %d ", currTime - *delay, value);
  *index += neededSpace;
  *log += 1;
  *delay = currTime;
}

void getData() {
  // Read analog values
  pressureRead = analogRead(A3);
  loadCellRead = analogRead(A5);

  // Append data to buffers
  appendData(pressureArray, &pressureIndex, &pressureLogs, pressureRead, &pressureDelay);
  appendData(loadCellArray, &loadCellIndex, &loadCellLogs, loadCellRead, &loadCellDelay);

  // Print and reset every second
  if (pressureLogs >= 500) {
    Serial.println("Pressure Data:");
    Serial.println((char*)pressureArray);
    Serial.println("Load Cell Data:");
    Serial.println((char*)loadCellArray);
    
    logData();

    // Reset buffers for new data
    pressureIndex = 0;
    loadCellIndex = 0;
    pressureLogs = 0;
    loadCellLogs = 0;
    pressureArray[0] = '\0';  // Reset to empty string
    loadCellArray[0] = '\0';
  }
}

void logData() {
  logFile = SD.open("datalogs.txt", FILE_WRITE);
  logFile.println((char*)pressureArray);
  logFile.println((char*)loadCellArray);
  logFile.close();
}

// void sendData() {
//   String transmit = "AT+SEND=0," + String(foo.length()) + "," + foo + "\r\n";
//   Serial1.flush();
//   Serial1.print(transmit);
//   delay(10);
// }

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
  Serial1.print(currState);
  delay(10);
}

void checkInput(String receive) {
  if (receive == "ARM" && currentState == SAFE) {
    Serial.println("CURRENT STATE: ARMED. ");
    currentState = ARMED;
    sendState("ARMED");
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
      getData();
      logData();
      break;
    case LAUNCHED: {
      Serial.println("D4184s LATCHED");
      digitalWrite(D4184A, HIGH);
      digitalWrite(D4184B, HIGH);
      unsigned long int latchTime = millis();
      bool flag = false;
      while(1) {
        getData();
        logData();
        // sendData();
        if (!flag && millis() - latchTime > 10000) {
          digitalWrite(D4184A, LOW);
          digitalWrite(D4184B, LOW);
          delay(10);
          sendState("UNLATCHED");
          Serial.println("D4184s UNLATCHED");
          flag = true;
        }
      }
      break;
    }
    case FAILURE:
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial Comm. Initialised.");

  // Pin Initialization
  pinMode(D4184A, OUTPUT);
  pinMode(D4184B, OUTPUT);
  pinMode(LOAD, INPUT);
  pinMode(PRESSURE, INPUT);
  digitalWrite(D4184A, LOW);
  digitalWrite(D4184B, LOW);

  //RYLR setup
  Serial1.begin(57600);
  Serial.println("RYLR Comm. Initialised");

  //SD Card setup
  if (!SD.begin()) {
    Serial.println("SD card initialisation failed.");
    return;
  }
  logFile = SD.open("datatest.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("Couldn't open log file");
    return;
  } 
  else {
    Serial.println("Logging to SD card...\n");
  }

  sendState("SAFE");
  Serial.println("TESTBED SETUP COMPLETE.");
}

void loop() {
  if (Serial1.available()) {
    response = Serial1.readStringUntil('\n');
    response = parseRYLR(response);
    Serial.println("RESPONSE: " + response);
    checkInput(response);
  }
  performOperations();
}