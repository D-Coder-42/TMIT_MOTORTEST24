#include <RTCZero.h>
#include <SdFat.h>

#define ADC_PIN_1 A4
#define ADC_PIN_2 A5

#define SAMPLE_RATE_HZ 500
#define SAMPLE_INTERVAL_MS (1000 / SAMPLE_RATE_HZ)

#define BUFFER_SIZE 1024
#define SD_CS 4  // SD CS pin for MKR Zero

RTCZero rtc;
SdFat sd;
File logFile;

typedef enum STATE {
  STANDBY,
  ACQUIRE,
  FINISH,
  FAILURE
};
STATE currentState = STANDBY;

// ---------- State Transition ----------
// void stateTransition() {
//   switch (currentState) {
//     case STANDBY:
//       if (digitalRead(N_ENABLE) == LOW) {
//         if (dataFile.isOpen()) {
//           digitalWrite(ACK, LOW);
//           currentState = ACQUIRE;
//           Serial.println("CURRENT MKR STATE: ACQUIRE");
//         } else {
//           Serial.println("Failed to open log file.");
//           currentState = FAILURE;
//           Serial.println("CURRENT MKR STATE: FAILURE");
//         }
//       }
//       break;

//     case ACQUIRE:
//       if (digitalRead(N_ENABLE) == HIGH) {
//         digitalWrite(ACK, HIGH);
//         currentState = FINISH;
//         Serial.println("CURRENT MKR STATE: FINISHED");
//       }
//       break;
//   }
// }

void stateTransition() {
  static String inputString = "";
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '\n') {
      inputString.trim();
      
      if (inputString.equalsIgnoreCase("ACQUIRE")) {
        currentState = ACQUIRE;
        Serial.println("STATE: ACQUIRE");
      } else if (inputString.equalsIgnoreCase("DONE")) {
        currentState = FINISH;
        Serial.println("STATE: FINISH");
      } else {
        Serial.print("Unknown command: ");
        Serial.println(inputString);
      }
      
      inputString = "";
    } else {
      inputString += inChar;
    }
  }
}

struct Sample {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t millis;
  uint16_t adc1;
  uint16_t adc2;
};

Sample buffer1[BUFFER_SIZE];
Sample buffer2[BUFFER_SIZE];
volatile bool buffer1_full = false;
volatile bool buffer2_full = false;
volatile Sample* active_buffer = buffer1;
volatile int buffer_index = 0;

unsigned long last_sample_time = 0;
unsigned long rtc_millis_offset = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  analogReadResolution(12);  // 12-bit ADC

  rtc.begin();
  rtc.setTime(0, 0, 0);  // Set RTC time (UTC)
  rtc_millis_offset = millis();

  // Init SD
  if (!sd.begin(SD_CS)) {
    Serial.println("SD init failed!");
    // while (1);
  }

  logFile = sd.open("data.csv", O_WRITE | O_CREAT | O_TRUNC);
  if (!logFile) {
    Serial.println("Failed to open log file!");
    // while (1);
  }

  logFile.println("Time,ADC1,ADC2");  // CSV header
  Serial.println("Time,ADC1,ADC2");
}

void loop() {
  if (currentState == ACQUIRE) {
    unsigned long now = millis();

    if (now - last_sample_time >= SAMPLE_INTERVAL_MS) {
      last_sample_time = now;

      // Get time
      uint32_t rtc_millis = now - rtc_millis_offset;
      Sample s;
      s.hour = rtc.getHours();
      s.minute = rtc.getMinutes();
      s.second = rtc.getSeconds();
      s.millis = rtc_millis % 1000;

      s.adc1 = analogRead(ADC_PIN_1);
      s.adc2 = analogRead(ADC_PIN_2);

      Sample* buf = (Sample*)active_buffer;
      buf[buffer_index++] = s;

      if (buffer_index >= BUFFER_SIZE) {
        if (active_buffer == buffer1) {
          buffer1_full = true;
          active_buffer = buffer2;
        } else {
          buffer2_full = true;
          active_buffer = buffer1;
        }
        buffer_index = 0;
      }
    }

    if (buffer1_full) {
      buffer1_full = false;
      logBuffer(buffer1);
    }

    if (buffer2_full) {
      buffer2_full = false;
      logBuffer(buffer2);
    }
  }
  else if (currentState == FINISH) {
    while(1);
  }
}

void logBuffer(Sample* buf) {
  char line[40];  // CSV line buffer

  for (int i = 0; i < BUFFER_SIZE; i++) {
    snprintf(line, sizeof(line), "%02u:%02u:%02u.%03u,%u,%u",
             buf[i].hour, buf[i].minute, buf[i].second,
             buf[i].millis, buf[i].adc1, buf[i].adc2);

    Serial.println(line);
    logFile.println(line);
  }

  logFile.flush();  // Commit to SD to avoid data loss
}
