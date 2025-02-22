# Temperature Control Hardware

## How It Works

### Components and Their Parameters

🔹 **BC547 Transistor**
- Acts as an intermediate driver.
- Receives a 3.3V control signal from the ESP32 through a resistor.
- Activates the circuit, allowing 5V to flow through the LED of the MOC3052 optotriac.

🔹 **MOC3052 Optotriac**
- Triggered by the BC547 transistor.
- Connected via a 1kΩ resistor to the BTA24 (or BT138) triac.
- Provides safe 220V switching.

🔹 **BTA24 (or BT138) Triac**
- Ideal for resistive loads like heaters.
- High current handling capabilities.

🔹 **PC817 Optotransistor**
- Used for zero-cross detection.
- Connected with two 51kΩ resistors to the AC mains.
- Allows the ESP32 to detect the exact moment when voltage crosses zero and synchronize the triac switching accordingly.

## Hardware Implementation

🛠 **Components:**
- **ESP32** from Espressif Systems as the main controller.
- **BC547 Transistor** for optotriac triggering.
- **MOC3052 Optotriac** for galvanic isolation.
- **PC817 Optotransistor** for zero-cross detection.
- **BTA24 or BT138 Triac** for handling high-current loads.
- **Rectifier Circuit** to power the ESP32 from 220V AC.
- **Fuse** for additional protection.