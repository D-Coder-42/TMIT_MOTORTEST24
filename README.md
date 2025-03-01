# Motor Test Firmware and Documentation (2024)

This repository contains the firmware and documentation developed for the motor tests by ***thrustMIT*** held in 2024. The structure and content of the repository are organized to facilitate understanding, modification, and reuse of the code for static firing systems.

## Motor Tests conducted
### *1. 7th October, 2024* - **M-Class Motor (M 1916)**
- Total Impulse: 9145.7 N-sec
- Average Thrust: 1916.2 N
- Burn Time: 4.773 sec
### *2. 8th October, 2024* - **J-Class Motor (J 590)**
- Total Impulse: 1209.6 N-sec
- Average Thrust: 590.4 N
- Burn Time: 2.049 s

---

## Repository Structure

### **1. `groundstation/`**
Contains the Arduino UNO firmware for the ground station side of the static firing system.
- **Files:**
  - `GroundCode.ino`
  - `groundstation.ino`
  - `GroundStation_FINAL.ino` - used for Static Fire Motor Test conducted on 7th and 8th October, 2024

---

### **2. `testbed/`**
Contains the Arduino UNO firmware for the testbed side of the static firing system.
- **Files:**
  - `testbed.ino`
  - `testbed_analog.ino`
  - `Testbed_FINAL.ino` - used for Static Fire Motor Test conducted on 7th and 8th October, 2024

---

### **3. `testing/`**
This directory contains the experimental codes tested for individual components of the static firing system. These are modular and can be combined as needed for use on the ground or testbed sides.

#### Subdirectories:
1. **`Load Cell/`**
   - Contains code for interfacing with a pancake load cell using:
     - **TP-400 Mini Weight Indicator**
     - **HX711 ADC Converter**

2. **`MKR Zero/`**
   - Experimental versions of the codes implemented on an **Arduino MKRZero** microcontroller for evaluating a potential upgrade from the Arduino UNO.

3. **`MLH Pressure Transducer/`**
   - Code for acquiring pressure data from the **MLH Pressure Transducer** using **bare-metal (register-level) programming**

4. **`Plotter/`**
   - Includes scripts and tools for visualizing sensor data and current state of the system, aiding in system evaluation.

5. **`RYLR/`**
   - Firmware for the **LoRa RYLR998 module**, including:
     - **Transmission code** and **Reception code** for testing
     - **Transceiver Code** for 2-way telemetry

---

## License
This repository is licensed under [MIT License](LICENSE).
