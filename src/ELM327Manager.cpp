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

String ELM327Manager::getNameForPID(managed_pids pid) {

  String name = "";
  
  switch (currentReadingPid) {    
    case BATTERY_VOLTAGE:
    {
      name = "batteryVoltage";
      break;
    }

    case COMMANDEDEGR:
    {
      name = "commandedEGR";
      break;
    }

    case EGRERROR:
    {
      name = "egrError";
      break;
    }

    case MANIFOLDPRESSURE:
    {
      name = "manifoldPressure";
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      name = "dpfDurtLevel";
      break;
    }

    case DPF_KMS_SINCE:
    {
      name = "kmsSinceDpf";
      break;
    }

    case DPF_REGEN_STATUS:
    {
      name = "regenerationStatus";
      break;
    }
  }

  return name;
}

float ELM327Manager::getDataForPID(managed_pids pid) {
  
  float value = -1;
  
  switch (currentReadingPid) {    
    case BATTERY_VOLTAGE:
    {
      value = batteryVoltage;
      break;
    }

    case COMMANDEDEGR:
    {
      value = commandedEGR;
      break;
    }

    case EGRERROR:
    {
      value = egrError;
      break;
    }

    case MANIFOLDPRESSURE:
    {
      value = manifoldPressure;
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      value = dpfDirtLevel;
      break;
    }

    case DPF_KMS_SINCE:
    {
      value = kmsSinceDpf;
      break;
    }

    case DPF_REGEN_STATUS:
    {
      value = regenerationStatus;
      break;
    }
  }

  return value;
}

String ELM327Manager::getUnitForPID(managed_pids pid) {

  String unit = "";
  
  switch (currentReadingPid) {    
    case BATTERY_VOLTAGE:
    {
      unit = "v";
      break;
    }

    case COMMANDEDEGR:
    {
      unit = "%";
      break;
    }

    case EGRERROR:
    {
      unit = "%";
      break;
    }

    case MANIFOLDPRESSURE:
    {
      unit = "kPa";
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      unit = "%";
      break;
    }

    case DPF_KMS_SINCE:
    {
      unit = "km";
      break;
    }

    case DPF_REGEN_STATUS:
    {
      unit = "%";
      break;
    }
  }

  return unit;
}

int ELM327Manager::getDecimalPointForPID(managed_pids pid) {
  
  int value = 0;
  
  switch (currentReadingPid) {    
    case BATTERY_VOLTAGE:
    {
      value = 2;
      break;
    }

    case COMMANDEDEGR:
    {
      value = 2;
      break;
    }

    case EGRERROR:
    {
      value = 2;
      break;
    }

    case MANIFOLDPRESSURE:
    {
      value = 2;
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      value = 0;
      break;
    }

    case DPF_KMS_SINCE:
    {
      value = 0;
      break;
    }

    case DPF_REGEN_STATUS:
    {
      value = 0;
      break;
    }
  }

  return value;
}

void ELM327Manager::readAllData() {

  if (!isDeviceELM327Initialized) {
    Serial.println("ELM327, could not read all data because ELM327 is not initialized...");
    return;
  }

  switch (currentReadingPid)
  {    
    case BATTERY_VOLTAGE:
    {
      float value = deviceELM327.batteryVoltage();

      if (readFloatData("batteryVoltage", value)) {
        batteryVoltage = value;
        currentReadingPid = COMMANDEDEGR;
      }
      
      break;
    }

    case COMMANDEDEGR:
    {
      float value = deviceELM327.commandedEGR();

      if (readFloatData("commandedEGR", value)) {
        commandedEGR = value;
        currentReadingPid = EGRERROR;
      }
      
      break;
    }

    case EGRERROR:
    {
      float value = deviceELM327.egrError();

      if (readFloatData("egrError", value)) {
        egrError = value;
        currentReadingPid = MANIFOLDPRESSURE;
      }
      
      break;
    }

    case MANIFOLDPRESSURE:
    {
      float value = deviceELM327.manifoldPressure();

      if (readFloatData("manifoldPressure", value)) {
        manifoldPressure = value;
        currentReadingPid = DPF_DIRT_LEVEL;
      }
      
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      int32_t value = readDpfDirtLevel();

      if (readFloatData("dpfDurtLevel", value)) {
        dpfDirtLevel = value;
        currentReadingPid = DPF_KMS_SINCE;
      }
      
      break;
    }

    case DPF_KMS_SINCE:
    {
      int32_t value = readKmsSinceDpf();

      if (readFloatData("kmsSinceDpf", value)) {
        kmsSinceDpf = value;
        currentReadingPid = DPF_REGEN_STATUS;
      }
      
      break;
    }

    case DPF_REGEN_STATUS:
    {
      int32_t value = readRegenerationStatus();

      if (readFloatData("regenerationStatus", value)) {
        regenerationStatus = value;
        currentReadingPid = BATTERY_VOLTAGE;
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
bool ELM327Manager::readFloatData(String pidName, float value) {

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


/*


Altri da provare
"Accumulo di fuliggine DPF Astra-J","Fuliggine DPF","223275","A","0","100","%","7E0"
"Distanza Astra-J dopo l'ultima sostituzione DPF","Dis.ultima sostituzione DPF","223276","A*65536+B*256+C","0","100000","km","7E0"
"Distanza Astra-J dall'ultima rigenerazione DPF","Dist.DPF reg","223277","A*65536+B*256+C","0","100000","km","7E0"
"Stato di rigenerazione Astra-J DPF","Stato DPF","223274","A*100/255","0","100","%","7E0"
"Temperatura media di ingresso DPF Astra-J durante la rigenerazione","Ingresso DPF temp","223279","A*5-40","-40","1235","grado C","7E0" 
"Durata media di rigenerazione DPF Astra-J","Aver.dur.DPF","22327A","A*256+B","0","65535","S","7E0"
"Contatore rigenerazioni interrotte DPF Astra-J","Coun.inter.DPF","223047","A*256+B","0","65535","conteggi","7E0"

PID: 223278
Voller Name: Durchschnittliche Distanz zwischen Reerationen
Abkürzung: Avg Dist between Regenerations
Maximum: 10000
Gerätetyp: km
Gleichung: (((A<8)+B )<8)+C

PID: 223279
Nome completo: temperatura DPF stimata
Abbreviazione: Temp. DPF
Massimo: 1000,0
Tipo di dispositivo °C
Equazione: A*5-40
Il valore era costante a 132 (132*5 -40 = 620) prima della combustione, poi costante a 121 (121*5-40 = 565). Il valore per "Temperatura catalizzatore (banco 1 sensore 2)" era 580 - 620°C durante la cottura. Devo tenere d'occhio una cosa.



*/

//int32_t v;
  //v = deviceELM327.processPID(SERVICE_22, 0x3278, 1, 1);
  //elm327ReadFloatData("0x3278", v, ""); // 53 km (Avg Dist between Regenerations)

  //v = deviceELM327.processPID(SERVICE_22, 0x3279, 1, 1, 5, -40);
  //elm327ReadFloatData("0x3279", v, ""); // 535 C°()

  //v = deviceELM327.processPID(SERVICE_22, 0x327A, 1, 1);
  //elm327ReadFloatData("0x327A", v, ""); // 509 s(Durata media di rigenerazione DPF)

  //v = deviceELM327.processPID(SERVICE_22, 0x3047, 1, 1);
  //elm327ReadFloatData("0x3047", v, ""); // non ritorna nulla (Contatore rigenerazioni interrotte DPF)