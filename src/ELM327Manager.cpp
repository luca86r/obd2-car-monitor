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

String ELM327Manager::getNameForPID(managed_pids pid) {

  return ((PidObj)pidDefs[pid]).getName();
}

float ELM327Manager::getDataForPID(managed_pids pid, bool prefetchNext) {
  
  // Save the last time the value was read for PID
  pidsLastGetMs[pid] = millis();

  // Prefetch the next PID
  int next = -1;
  if (prefetchNext) {
    next = (abs(((int) pid) + 1)) % MANAGED_PIDS_COUNT;
    pidsLastGetMs[next] = millis();
  }

  if (DEBUG_MODE) {
    Serial.print("getDataForPID");
    Serial.print(pid);
    Serial.print(" ");
    Serial.println(next);
  }

  return ((PidObj)pidDefs[pid]).getFValue();
}

String ELM327Manager::getUnitForPID(managed_pids pid) {

  return ((PidObj)pidDefs[pid]).getUnit();
}

int ELM327Manager::getPercentageForPID(managed_pids pid) {

  PidObj pObj = pidDefs[pid];
  return (pObj.getFValue() - pObj.getFMinValue()) * 100 / (pObj.getFMaxValue() - pObj.getFMinValue());
}

int ELM327Manager::getDecimalPointForPID(managed_pids pid) {
  
  return ((PidObj)pidDefs[pid]).getFDecimalPoint();
}

managed_pids ELM327Manager::nextPidToRead() {

  bool found = false;
  int next = (int) currentReadingPid;

  while (!found) {

    next = (abs(next + 1)) % MANAGED_PIDS_COUNT;
    found = isRecentlyGet((managed_pids)next);
  
    if (DEBUG_MODE) {
      Serial.print("nextPidToRead");
      Serial.print(currentReadingPid);
      Serial.print(" ");
      Serial.print(found);
      Serial.print(" ");
      Serial.println(next);
    }
  }

  return (managed_pids)next;
}

bool ELM327Manager::isRecentlyGet(managed_pids pid) {

  return (millis() - pidsLastGetMs[pid]) < READ_ELM327_DATA_GET_LIMIT_MS;
}

void ELM327Manager::readAllData() {

  if (!isDeviceELM327Initialized) {
    Serial.println("ELM327, could not read all data because ELM327 is not initialized...");
    return;
  }

  bool isReadCompleted = false;
  PidObj pObj = pidDefs[currentReadingPid];

  float value = PID_NO_VALUE;

  switch (currentReadingPid)
  {    
    case BATTERY_VOLTAGE:
    {
      value = deviceELM327.batteryVoltage();
      break;
    }

    case COMMANDEDEGR:
    {
      value = deviceELM327.commandedEGR();
      break;
    }

    case EGRERROR:
    {
      value = deviceELM327.egrError();
      break;
    }

    case MANIFOLDPRESSURE:
    {
      value = deviceELM327.manifoldPressure();
      
      // Absolute pressure to relative pressure subtracting 100
      // Converting in bar dividing by 100 (1 bar = 100 kPa)
      value = (value - 100) / 100;
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      value = readDpfDirtLevel();
      break;
    }

    case DPF_KMS_SINCE:
    {
      value = readKmsSinceDpf();
      break;
    }

    case DPF_REGEN_STATUS:
    {
      value = readRegenerationStatus();
      break;
    }

    case ENG_COOLANT_TEMP:
    {
      value = deviceELM327.engineCoolantTemp();
      break;
    }

    case OIL_TEMP:
    {
      value = deviceELM327.oilTemp();
      break;
    }

    case CAT_TEMP_B1S1:
    {
      value = deviceELM327.catTempB1S1();
      break;
    }

    case CAT_TEMP_B1S2:
    {
      value = deviceELM327.catTempB1S2();
      break;
    }

    case CAT_TEMP_B2S1:
    {
      value = deviceELM327.catTempB2S1();
      break;
    }

    case CAT_TEMP_B2S2:
    {
      value = deviceELM327.catTempB2S2();
      break;
    }

    case ENG_LOAD:
    {
      value = deviceELM327.engineLoad();
      break;
    }

    case TORQUE_DEMANDED:
    {
      value = deviceELM327.demandedTorque();
      break;
    }

    case TORQUE_REFERENCE:
    {
      value = deviceELM327.referenceTorque();
      break;
    }

    case TORQUE:
    {
      value = deviceELM327.torque();
      break;
    }
  }

  isReadCompleted = isNonBlockingReadCompleted(getNameForPID(currentReadingPid), value);
  if (isReadCompleted) {
    if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
      value = PID_NO_VALUE;
    }
    
    pObj.setFValue(value);

    currentReadingPid = nextPidToRead();
  }
}

bool ELM327Manager::isNonBlockingReadCompleted(String pidName, float value) {

  bool readDone = false;

  if (deviceELM327.nb_rx_state == ELM_SUCCESS) {
    Serial.println(pidName + ": " + value);
    readDone = true;
  }
  else if (deviceELM327.nb_rx_state != ELM_GETTING_MSG) {
    Serial.println(pidName);
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