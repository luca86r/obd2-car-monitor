#include "ELM327Manager.h"
#include "config.h"

void ELM327Manager::checkOrInit(BluetoothSerial *btSerial) {

  if (isDeviceELM327Initialized) {
    return;
  }

  Serial.println("ELM327, begining...");

  if (!deviceELM327.begin(*btSerial, DEBUG_MODE, 2000)) {
      Serial.println("Couldn't connect to OBD scanner - Phase 2");
      isDeviceELM327Initialized = false;
  }

  // Set a custom header using ATSH command in order to access additional custom data
  if (deviceELM327.sendCommand_Blocking("AT SH 7E0") != ELM_SUCCESS) {
      Serial.println("Unable to set custom header 7E0");
      isDeviceELM327Initialized = false;
      delay(3000);
  }

  Serial.println("Connected to ELM327");
  isDeviceELM327Initialized = true;
}

bool ELM327Manager::isInitialized() {
    return isDeviceELM327Initialized;
}

void ELM327Manager::resetInitState() {
    isDeviceELM327Initialized = false;
}

String ELM327Manager::getNbRxStateString() {

  if (deviceELM327.nb_rx_state == ELM_SUCCESS)
    return "SUCCESS";
  else if (deviceELM327.nb_rx_state == ELM_GETTING_MSG)
    return "ELM_GETTING_MSG";
  else if (deviceELM327.nb_rx_state == ELM_MSG_RXD)
    return "ELM_MSG_RXD";
  else if (deviceELM327.nb_rx_state == ELM_NO_RESPONSE)
    return "ERROR: ELM_NO_RESPONSE";
  else if (deviceELM327.nb_rx_state == ELM_BUFFER_OVERFLOW)
    return "ERROR: ELM_BUFFER_OVERFLOW";
  else if (deviceELM327.nb_rx_state == ELM_GARBAGE)
    return "ERROR: ELM_GARBAGE";
  else if (deviceELM327.nb_rx_state == ELM_UNABLE_TO_CONNECT)
    return "ERROR: ELM_UNABLE_TO_CONNECT";
  else if (deviceELM327.nb_rx_state == ELM_NO_DATA)
    return "ERROR: ELM_NO_DATA";
  else if (deviceELM327.nb_rx_state == ELM_STOPPED)
    return "ERROR: ELM_STOPPED";
  else if (deviceELM327.nb_rx_state == ELM_TIMEOUT)
    return "ERROR: ELM_TIMEOUT";
  else if (deviceELM327.nb_rx_state == ELM_GENERAL_ERROR)
    return "ERROR: ELM_GENERAL_ERROR";
  else
    return "ERROR: UNKNOWN ELM STATUS: " + deviceELM327.nb_rx_state;
}

float ELM327Manager::getBatteryVoltage() {
  return batteryVoltage;
}

float ELM327Manager::getCommandedEGR() {
  return commandedEGR;
}

float ELM327Manager::getEgrError() {
  return egrError;
}

float ELM327Manager::getManifoldPressure() {
  return manifoldPressure;
}

int32_t ELM327Manager::getRegenerationStatus() {
  return regenerationStatus;
}

int32_t ELM327Manager::getKmsSinceDpf() {
  return kmsSinceDpf;
}

int32_t ELM327Manager::getDpfDirtLevel() {
  return dpfDirtLevel;
}

void ELM327Manager::readAllData() {

  if (!isDeviceELM327Initialized) {
    Serial.println("ELM327, could not read all data because ELM327 is not initialized...");
    return;
  }

  switch (obd_state)
  {    
    case BATTERY_VOLTAGE:
    {
      batteryVoltage = deviceELM327.batteryVoltage();

      if (readFloatData("batteryVoltage", batteryVoltage, "V")) {
        obd_state = COMMANDEDEGR;
      }
      
      break;
    }

    case COMMANDEDEGR:
    {
      commandedEGR = deviceELM327.commandedEGR();

      if (readFloatData("commandedEGR", commandedEGR, "%")) {
        obd_state = EGRERROR;
      }
      
      break;
    }

    case EGRERROR:
    {
      egrError = deviceELM327.egrError();

      if (readFloatData("egrError", egrError, "%")) {
        obd_state = MANIFOLDPRESSURE;
      }
      
      break;
    }

    case MANIFOLDPRESSURE:
    {
      manifoldPressure = deviceELM327.manifoldPressure();

      if (readFloatData("manifoldPressure", manifoldPressure, "kPa")) {
        obd_state = DPF_DIRT_LEVEL;
      }
      
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      dpfDirtLevel = readDpfDirtLevel();

      if (readFloatData("dpfDurtLevel", dpfDirtLevel, "%")) {
        obd_state = DPF_KMS_SINCE;
      }
      
      break;
    }

    case DPF_KMS_SINCE:
    {
      kmsSinceDpf = readKmsSinceDpf();

      if (readFloatData("kmsSinceDpf", kmsSinceDpf, "km")) {
        obd_state = DPF_REGEN_STATUS;
      }
      
      break;
    }

    case DPF_REGEN_STATUS:
    {
      regenerationStatus = readRegenerationStatus();

      if (readFloatData("regenerationStatus", regenerationStatus, "%")) {
        obd_state = BATTERY_VOLTAGE;
      }
      
      break;
    }
  }
}

/*
 * Gestisce la lettura di un singolo valore da ELM327
 *
 * @param pidName PID name
 * @param value PID value (get from library)
 * 
 * @return true se la lettura del valore è da considerarsi terminata (anche in caso di errore); false se la lettura 
 *         è da considerarsi come "in corso".
 */
bool ELM327Manager::readFloatData(String pidName, float value, String valueUnit) {

  bool readDone = false;

  if (deviceELM327.nb_rx_state == ELM_SUCCESS) {
    Serial.println(pidName + ": " + value);
    readDone = true;
  }
  else if (deviceELM327.nb_rx_state != ELM_GETTING_MSG) {
    deviceELM327.printError();
    readDone = true;
  }
  
  return readDone;
}

int32_t ELM327Manager::readRegenerationStatus() {
  return deviceELM327.processPID(0x22, 0x3274, 1, 1, 100.0 / 255.0);
}

int32_t ELM327Manager::readKmsSinceDpf() {
  return deviceELM327.processPID(0x22, 0x3277, 1, 3);
}

int32_t ELM327Manager::readDpfDirtLevel() {
  return deviceELM327.processPID(0x22, 0x3275, 1, 1);
}

