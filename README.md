# OBD2 Car Monitor
An **ESP32** project that displays the PIDs of a car through the OBD2 interface.

## Features
Feature list:
* Connects to OBD2 Bluetooth dongle equipped with ELM327/ELM329 chip
* Bluetooth discovery and pairing by device name
* Reconnects automatically if the connection is lost
* Communicates with the ELM327 chip to query car PIDs
* Autostarts on car ignition (available only if USB power is on/off based on car ignition state)
* Fast (low lag) reading of PID values; only necessary PIDs are read
* Reads non-standard PIDs, particularly the DPF parameters of a diesel car
* Displays a warning and a progress indicator when the DPF starts the regeneration process
* Manual selection of the desired PIDs to be displayed (through buttons)
* Displays all supported PIDs in a loop

## OBD2 (ELM327) Interface Compatibility
Requires an OBD2 Bluetooth adapter with an ELM327 chip.  
Due to compatibility with the [ELMduino project](https://github.com/PowerBroker2/ELMduino), only Bluetooth Classic is supported; BLE (Bluetooth Low Energy) currently doesn't work.

Tested with:
* vGate vLinker MC+ (Bluetooth) through Bluetooth Classic (not BLE): [https://www.vgatemall.com/products-detail/i-5/](https://www.vgatemall.com/products-detail/i-5/)

## Car Compatibility
Currently tested with the following cars/engines:
* Opel/Vauxhall Astra J (Engine code A17DTR)