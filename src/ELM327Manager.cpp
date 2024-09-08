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

  String name = "";
  
  switch (pid) {    
    case BATTERY_VOLTAGE:
    {
      name = "Battery";
      break;
    }

    case COMMANDEDEGR:
    {
      name = "Cmd EGR";
      break;
    }

    case EGRERROR:
    {
      name = "EGR Error";
      break;
    }

    case MANIFOLDPRESSURE:
    {
      name = "Manifold";
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      name = "DPF level";
      break;
    }

    case DPF_KMS_SINCE:
    {
      name = "DPF last";
      break;
    }

    case DPF_REGEN_STATUS:
    {
      name = "DPF Regen.";
      break;
    }

    case ENG_COOLANT_TEMP:
    {
      name = "ECT";
      break;
    }

    case OIL_TEMP:
    {
      name = "Oil";
      break;
    }

    case CAT_TEMP_B1S1:
    {
      name = "Cat. B1 S1";
      break;
    }

    case CAT_TEMP_B1S2:
    {
      name = "Cat. B1 S2";
      break;
    }

    case CAT_TEMP_B2S1:
    {
      name = "Cat. B2 S1";
      break;
    }

    case CAT_TEMP_B2S2:
    {
      name = "Cat. B2 S2";
      break;
    }

    case ENG_LOAD:
    {
      name = "Load";
      break;
    }

    case TORQUE_DEMANDED:
    {
      name = "Torque Dem.";
      break;
    }

    case TORQUE_REFERENCE:
    {
      name = "Torque Ref.";
      break;
    }

    case TORQUE:
    {
      name = "Torque";
      break;
    }
  }

  return name;
}

float ELM327Manager::getDataForPID(managed_pids pid) {
  
  // Save the last time the value was read for PID
  pidsLastGetMs[pid] = millis();

  // Prefetch the next PID
  int next = (abs(((int) pid) + 1)) % MANAGED_PIDS_COUNT;
  pidsLastGetMs[next] = millis();

  if (DEBUG_MODE) {
    Serial.print("getDataForPID");
    Serial.print(pid);
    Serial.print(" ");
    Serial.println(next);
  }

  float value = -1;
  
  switch (pid) {    
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

    case ENG_COOLANT_TEMP:
    {
      value = ect;
      break;
    }

    case OIL_TEMP:
    {
      value = oil;
      break;
    }

    case CAT_TEMP_B1S1:
    {
      value = catB1S1;
      break;
    }

    case CAT_TEMP_B1S2:
    {
      value = catB1S2;
      break;
    }

    case CAT_TEMP_B2S1:
    {
      value = catB2S1;
      break;
    }

    case CAT_TEMP_B2S2:
    {
      value = catB2S2;
      break;
    }

    case ENG_LOAD:
    {
      value = engLoad;
      break;
    }

    case TORQUE_DEMANDED:
    {
      value = torqueDem;
      break;
    }

    case TORQUE_REFERENCE:
    {
      value = torqueRef;
      break;
    }

    case TORQUE:
    {
      value = torque;
      break;
    }
  }

  return value;
}

String ELM327Manager::getUnitForPID(managed_pids pid) {

  String unit = "";
  
  switch (pid) {    
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
      unit = "bar";
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

    case ENG_COOLANT_TEMP:
    {
      unit = "C";
      break;
    }

    case OIL_TEMP:
    {
      unit = "C";
      break;
    }

    case CAT_TEMP_B1S1:
    {
      unit = "C";
      break;
    }

    case CAT_TEMP_B1S2:
    {
      unit = "C";
      break;
    }

    case CAT_TEMP_B2S1:
    {
      unit = "C";
      break;
    }

    case CAT_TEMP_B2S2:
    {
      unit = "C";
      break;
    }

    case ENG_LOAD:
    {
      unit = "%";
      break;
    }

    case TORQUE_DEMANDED:
    {
      unit = "%";
      break;
    }

    case TORQUE_REFERENCE:
    {
      unit = "Nm";
      break;
    }

    case TORQUE:
    {
      unit = "%";
      break;
    }
  }

  return unit;
}

int ELM327Manager::getPercentageForPID(managed_pids pid) {

  int perc = -1;
  
  switch (pid) {    
    case BATTERY_VOLTAGE:
    {
      const float min = 12;
      const float max = 14.6;
      perc = (batteryVoltage - min) * 100 / (max - min);
      break;
    }

    case COMMANDEDEGR:
    {
      const float min = 0;
      const float max = 100;
      perc = (commandedEGR - min) * 100 / (max - min);
      break;
    }

    case EGRERROR:
    {
      const float min = -100;
      const float max = 100;
      perc = (egrError - min) * 100 / (max - min);
      break;
    }

    case MANIFOLDPRESSURE:
    {
      const float min = 0;
      const float max = 1.6;
      perc = (manifoldPressure - min) * 100 / (max - min);
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      const float min = 0;
      const float max = 100;
      perc = (dpfDirtLevel - min) * 100 / (max - min);
      break;
    }

    case DPF_KMS_SINCE:
    {
      const float min = 0;
      const float max = 500;
      perc = (kmsSinceDpf - min) * 100 / (max - min);
      break;
    }

    case DPF_REGEN_STATUS:
    {
      const float min = 0;
      const float max = 100;
      perc = (regenerationStatus - min) * 100 / (max - min);
      break;
    }

    case ENG_COOLANT_TEMP:
    {
      const float min = -20;
      const float max = 120;
      perc = (ect - min) * 100 / (max - min);
      break;
    }

    case OIL_TEMP:
    {
      const float min = -20;
      const float max = 120;
      perc = (oil - min) * 100 / (max - min);
      break;
    }

    case CAT_TEMP_B1S1:
    {
      const float min = -20;
      const float max = 800;
      perc = (catB1S1 - min) * 100 / (max - min);
      break;
    }

    case CAT_TEMP_B1S2:
    {
      const float min = -20;
      const float max = 800;
      perc = (catB1S2 - min) * 100 / (max - min);
      break;
    }

    case CAT_TEMP_B2S1:
    {
      const float min = -20;
      const float max = 800;
      perc = (catB2S1 - min) * 100 / (max - min);
      break;
    }

    case CAT_TEMP_B2S2:
    {
      const float min = -20;
      const float max = 800;
      perc = (catB2S2 - min) * 100 / (max - min);
      break;
    }

    case ENG_LOAD:
    {
      const float min = 0;
      const float max = 100;
      perc = (engLoad - min) * 100 / (max - min);
      break;
    }

    case TORQUE_DEMANDED:
    {
      const float min = 0;
      const float max = 100;
      perc = (torqueDem - min) * 100 / (max - min);
      break;
    }

    case TORQUE_REFERENCE:
    {
      const float min = 0;
      const float max = 200;
      perc = (torqueRef - min) * 100 / (max - min);
      break;
    }

    case TORQUE:
    {
      const float min = 0;
      const float max = 100;
      perc = (torque - min) * 100 / (max - min);
      break;
    }
  }

  return perc;
}

int ELM327Manager::getDecimalPointForPID(managed_pids pid) {
  
  int value = 0;
  
  switch (pid) {    
    case BATTERY_VOLTAGE:
    case COMMANDEDEGR:
    case EGRERROR:
    case MANIFOLDPRESSURE:
    {
      value = 2;
      break;
    }

    case DPF_DIRT_LEVEL:
    case DPF_KMS_SINCE:
    case DPF_REGEN_STATUS:
    case ENG_COOLANT_TEMP:
    case OIL_TEMP:
    case CAT_TEMP_B1S1:
    case CAT_TEMP_B1S2:
    case CAT_TEMP_B2S1:
    case CAT_TEMP_B2S2:
    case ENG_LOAD:
    case TORQUE_DEMANDED:
    case TORQUE_REFERENCE:
    case TORQUE:
    {
      value = 0;
      break;
    }
  }

  return value;
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

  switch (currentReadingPid)
  {    
    case BATTERY_VOLTAGE:
    {
      float value = deviceELM327.batteryVoltage();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        batteryVoltage = value;
      }
      
      break;
    }

    case COMMANDEDEGR:
    {
      float value = deviceELM327.commandedEGR();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        commandedEGR = value;
      }
      
      break;
    }

    case EGRERROR:
    {
      float value = deviceELM327.egrError();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        egrError = value;
      }
      
      break;
    }

    case MANIFOLDPRESSURE:
    {
      float value = deviceELM327.manifoldPressure();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        // Absolute pressure to relative pressure subtracting 100
        // Converting in bar dividing by 100 (1 bar = 100 kPa)
        manifoldPressure = (value - 100) / 100; 
      }
      
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      int32_t value = readDpfDirtLevel();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        dpfDirtLevel = value;
      }
      
      break;
    }

    case DPF_KMS_SINCE:
    {
      int32_t value = readKmsSinceDpf();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        kmsSinceDpf = value;
      }
      
      break;
    }

    case DPF_REGEN_STATUS:
    {
      int32_t value = readRegenerationStatus();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        regenerationStatus = value;
      }
      
      break;
    }

    case ENG_COOLANT_TEMP:
    {
      float value = deviceELM327.engineCoolantTemp();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        ect = value;
      }

      break;
    }

    case OIL_TEMP:
    {
      float value = deviceELM327.oilTemp();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        oil = value;
      }

      break;
    }

    case CAT_TEMP_B1S1:
    {
      float value = deviceELM327.catTempB1S1();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        catB1S1 = value;
      }

      break;
    }

    case CAT_TEMP_B1S2:
    {
      float value = deviceELM327.catTempB1S2();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        catB1S2 = value;
      }

      break;
    }

    case CAT_TEMP_B2S1:
    {
      float value = deviceELM327.catTempB2S1();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        catB2S1 = value;
      }

      break;
    }

    case CAT_TEMP_B2S2:
    {
      float value = deviceELM327.catTempB2S2();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        catB2S2 = value;
      }

      break;
    }

    case ENG_LOAD:
    {
      float value = deviceELM327.engineLoad();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        engLoad = value;
      }

      break;
    }

    case TORQUE_DEMANDED:
    {
      float value = deviceELM327.demandedTorque();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        torqueDem = value;
      }

      break;
    }

    case TORQUE_REFERENCE:
    {
      float value = deviceELM327.referenceTorque();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        torqueRef = value;
      }

      break;
    }

    case TORQUE:
    {
      float value = deviceELM327.torque();
      isReadCompleted = readFloatData(getNameForPID(currentReadingPid), value);

      if (isReadCompleted) {
        if(deviceELM327.nb_rx_state != ELM_SUCCESS) {
          value = PID_NO_VALUE;
        }
        
        torque = value;
      }

      break;
    }
  }

  if (isReadCompleted) {
    currentReadingPid = nextPidToRead();
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