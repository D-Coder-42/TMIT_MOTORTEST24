#include <Arduino.h>

int timer = 0;
int pressureRead/*, loadCellRead*/;

unsigned char* pressureArray;
// unsigned char* loadCellArray;

const int maxSize = 6000;  // Total buffer size for each array
int pressureIndex = 0;     // Tracks data size for each array
// int loadCellIndex = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  // Allocate memory for the arrays
  pressureArray = (unsigned char*)malloc(maxSize * sizeof(unsigned char));
  // loadCellArray = (unsigned char*)malloc(maxSize * sizeof(unsigned char));

  // Check for successful memory allocation
  if (!pressureArray /*|| !loadCellArray*/) {
    Serial.println("Memory allocation failed!");
    while (1);  // Halt execution if memory allocation fails
  }

  // Initialize arrays with null terminators
  pressureArray[0] = '\0';
  // loadCellArray[0] = '\0';
}

// Function to append data safely
void appendData(unsigned char* buffer, int* index, int value) {
  // Estimate space needed to store the integer and a space
  int neededSpace = snprintf(NULL, 0, "%d ", value);
  
  // Ensure enough space to add new data
  if (*index + neededSpace >= maxSize - 1) {
    Serial.println("Buffer overflow detected!");
    return;
  }

  // Append formatted data to the buffer
  snprintf((char*)(buffer + *index), maxSize - *index, "%d ", value);
  *index += neededSpace;
}

void loop() {
  // Read analog values
  pressureRead = analogRead(A3);
  // loadCellRead = analogRead(A5);

  // Append data to buffers
  appendData(pressureArray, &pressureIndex, pressureRead);
  // appendData(loadCellArray, &loadCellIndex, loadCellRead);

  // Print and reset every second
  if (millis() - timer >= 1000) {
    timer = millis();
    Serial.println("Pressure Data:");
    Serial.println((char*)pressureArray);
    // Serial.println("Load Cell Data:");
    // Serial.println((char*)loadCellArray);

    // Reset buffers for new data
    pressureIndex = 0;
    // loadCellIndex = 0;
    pressureArray[0] = '\0';  // Reset to empty string
    // loadCellArray[0] = '\0';
  }
}
