# OBD2 Car Monitor
An **ESP32** project that display car PIDs trought OBD2 interface.

## Feature
Feature list:
* connect to OBD2 Bluetooth dongle with ELM327 chip
* first discover and pair with bluetooth device by name
* comunicate with chip ELM327 to query PIDs
* fast (low lag) read of PIDs values; only necessary PIDs are read
* read non standard PIDs, in particolar DPF parameter of diesel car
* TODO


## OBD2 (ELM327) interface compatibility
Needs and OBD2 bluetooth adapter with ELM327 chip.
Due to compatibility with the project (ELMduino)[https://github.com/PowerBroker2/ELMduino] only Bluetooth classic is supported; BLE (Bluetooth Low Energy) currently doesn't work.

Tested with:
* vGate vLinker MC+ (Bluetooth) throught Bluetooth classic (non BLE): (https://www.vgatemall.com/products-detail/i-5/)[https://www.vgatemall.com/products-detail/i-5/]

## Car compability
TODO
