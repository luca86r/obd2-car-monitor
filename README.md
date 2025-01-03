# OBD2 Car Monitor
An **ESP32** project that displays PIDs from a car through the OBD2 interface.

- [OBD2 Car Monitor](#obd2-car-monitor)
  - [📄 1. Features](#-1-features)
  - [🎛️ 2. Working modes](#️-2-working-modes)
    - [🤏 2.1. Manual](#-21-manual)
    - [➰️ 2.2. Loop](#️-22-loop)
    - [🤖 2.3. Automatic](#-23-automatic)
    - [💨 2.4. DPF regeneration](#-24-dpf-regeneration)
  - [🔌 3. OBD2 (ELM327) Interface compatibility](#-3-obd2-elm327-interface-compatibility)
  - [🚗 4. Car compatibility](#-4-car-compatibility)
  - [📟️ 5. Supported PIDs](#️-5-supported-pids)
  - [🪛 6. Hardware](#-6-hardware)
    - [📃 6.1. Parts](#-61-parts)
    - [🪡 6.2. Wiring](#-62-wiring)

## 📄 1. Features
Features list:
* Connects to an OBD2 Bluetooth dongle equipped with ELM327/ELM329 chip
* Bluetooth discovery and pairing by device name
* Automatically reconnects if the connection is lost
* Communicates with the ELM327 chip to query car PIDs
* Auto-starts on car ignition (available only if USB power is on/off based on car ignition state)
* Fast (low-lag) reading of PID values; only necessary PIDs are read
* Reads non-standard PIDs, particularly the DPF parameters of diesel car
* Displays a warning and progress indicator when DPF regeneration starts
* Manual selection of desired PIDs to display (via button)
* Remembers last selected PID or mode
* Loop mode: displays all supported PIDs in sequence
* Automatic mode: displays specific PIDs based on conditions
* Gauge display (currently only for "MANIFOLDPRESSURE" PID)

## 🎛️ 2. Working modes
Supported modes:
* [Manual](#-manual)
* [Loop](#️-loop)
* [Automatic](#-automatic)
* [DPF regeneration](#-dpf-regeneration)

### 🤏 2.1. Manual
Manually select the desired PID to display.
Works with a single press of the main button.

### ➰️ 2.2. Loop
Cycles through all PIDs in sequence.

To activate this mode:
1. Use the main button to scroll through PIDs with a single press
2. Once the "RPM" PID is reached, press the main button once more to enable loop mode
3. The mode is active when the letter "L" appears in the top right box.
 
To deactivate the loop mode press the main button once.

### 🤖 2.3. Automatic
Display some specific PID based on some specific conditions and preferences:
* when ignition is off: display the battery voltage
* when ignition is on (RPM > 0):
  * display the battery voltage for 30 seconds
  * display the engine coolant temperature if below 65C° (see also **ENGINE_COOLANT_IS_COLD_THRESHOLD** in [config.h](https://github.com/luca86r/obd2-car-monitor/blob/master/src/config.h))
  * otherwise display the last manually selected PID.

To activate this mode, long press the main button until the letter "A" appears in the top right box.
To deactivate long press the main button again.

### 💨 2.4. DPF regeneration
This mode is automatically activated when the DPF regeneration process start.
When the process end, the last PID or mode is automatically restored.
During the regeneration process the following informations are displayed in turn:
* warning message "DPF CLEANING!" with blinking background
* DPF regeneration progress in percentage (PID "DPF_REGEN_STATUS").


## 🔌 3. OBD2 (ELM327) Interface compatibility
Requires an OBD2 Bluetooth adapter with an ELM327 (or ELM329) chip.  
Due to compatibility with the [ELMduino project](https://github.com/PowerBroker2/ELMduino), only Bluetooth Classic is supported; BLE (Bluetooth Low Energy) currently doesn't work.

Tested with:
* vGate vLinker MC+ (Bluetooth) through Bluetooth Classic (not BLE): [https://www.vgatemall.com/products-detail/i-5/](https://www.vgatemall.com/products-detail/i-5/)

## 🚗 4. Car compatibility
Currently tested with the following cars/engines:
* Opel/Vauxhall Astra J (Engine code A17DTR)

## 📟️ 5. Supported PIDs
See also the file [ELM327Manager.h](https://github.com/luca86r/obd2-car-monitor/blob/master/src/ELM327Manager.h)


|      **id**      |  **Name**  |            **Description**             |  **Unit**  | **Decimal** | **Minimun value** | **Maximun value** | **Read delay in ms** |
|------------------|------------|----------------------------------------|------------|-------------|-------------------|-------------------|----------------------|
| BATTERY_VOLTAGE  | Battery    | Battery voltage                        | Voltage    |           2 |                12 |              15.5 |                 1000 |
| COMMANDEDEGR     | Cmd EGR    | Commanded EGR                          | Percentage |           2 |                 0 |               100 |                    0 |
| EGRERROR         | EGR Error  | EGR Error                              | Percentage |           2 |              -101 |               101 |                    0 |
| MANIFOLDPRESSURE | Manifold   | Manifold pressure                      | Bar        |           2 |                 0 |               1.6 |                    0 |
| DPF_DIRT_LEVEL   | DPF level  | DPF level                              | Percentage |           0 |                 0 |               100 |                10000 |
| DPF_KMS_SINCE    | DPF last   | Km since last DPF regeneration         | Kilometer  |           0 |                 0 |               500 |                10000 |
| DPF_REGEN_STATUS | DPF Regen. | DPF regeneration progress              | Percentage |           0 |                 0 |               100 |                 5000 |
| ENG_COOLANT_TEMP | ECT        | Engine Coolant Temperature             | Celsius    |           0 |               -20 |               120 |                 1000 |
| OIL_TEMP         | Oil        | Engine oil temperature                 | Celsius    |           0 |               -20 |               120 |                 1000 |
| CAT_TEMP_B1S1    | B1 S1      | Catalyst temperature, Bank 1, Sensor 1 | Celsius    |           0 |               -20 |               800 |                 1000 |
| CAT_TEMP_B1S2    | B1 S2      | Catalyst temperature, Bank 1, Sensor 2 | Celsius    |           0 |               -20 |               800 |                 1000 |
| CAT_TEMP_B2S1    | B2 S1      | Catalyst temperature, Bank 2, Sensor 1 | Celsius    |           0 |               -20 |               800 |                 1000 |
| CAT_TEMP_B2S2    | B2 S2      | Catalyst temperature, Bank 2, Sensor 2 | Celsius    |           0 |               -20 |               800 |                 1000 |
| ENG_LOAD         | Load       | Engine load                            | Percentage |           0 |                 0 |               100 |                  500 |
| ENG_RPM          | RPM        | Engine RPM                             | -          |           0 |                 0 |              6000 |                  500 |

	
## 🪛 6. Hardware
### 📃 6.1. Parts
* n°1 ESP32 (ESP-WROOM-32) DevKitC v1
* n°1 Display SSD1306 128px * 64px
* n°1 Mini push button
* 3D printed case (ex. https://www.thingiverse.com/thing:5171826)

### 🪡 6.2. Wiring
[Fritzing](https://fritzing.org/) project: [sketch-fritzing.fzz](https://github.com/luca86r/obd2-car-monitor/blob/master/sketch-fritzing.fzz)

![Fritzing](https://github.com/luca86r/obd2-car-monitor/blob/master/sketch-fritzing.jpg?raw=true)