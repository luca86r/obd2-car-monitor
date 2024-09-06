#include "ELMduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Preferences.h"
#include "config.h"
#include "BluetoothManager.h"
//#include "ELM327Manager.h"

// === Preferences ===
Preferences preferences;

// === Bluetooth ===
BluetoothManager bluetoothManager;

// === ELM327 ===
ELM327 deviceELM327;
bool isDeviceELM327Initialized = false;

typedef enum { ENG_RPM,
               BATTERY_VOLTAGE,
               COMMANDEDEGR,
               EGRERROR,
               MANIFOLDPRESSURE,
               DPF_DIRT_LEVEL,
               DPF_KMS_SINCE,
               DPF_REGEN_STATUS} obd_pid_states;

obd_pid_states obd_state = ENG_RPM;

float rpm = 0;
float batteryVoltage = 0;
float commandedEGR = 0;
float egrError = 0;
float manifoldPressure = 0;
int32_t kmsSinceDpf = 0;
int32_t dpfDirtLevel = 0;
int32_t regenerationStatus = 0;

// === Display ===
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Screen SSD1306 128x64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oled_ko = false;

void oledPrintData() {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  /*
  display.print("RPM: ");
  display.print(rpm);
  display.print("\n");
  */
  display.print("battery: ");
  display.print(batteryVoltage);
  display.print(" v");
  display.print("\n");
  
  display.print("EGR cmd: ");
  display.print(commandedEGR);
  display.print(" %");
  display.print("\n");
  
  display.print("egrError: ");
  display.print(egrError);
  display.print(" %");
  display.print("\n");
  /*
  display.print("manifoldPressure: ");
  display.print(manifoldPressure);
  display.print(" kPa");
  display.print("\n");
  */
  display.print("dpfDirtLevel: ");
  display.print(dpfDirtLevel);
  display.print(" %");
  display.print("\n");

  display.print("kmsSinceDpf: ");
  display.print(kmsSinceDpf);
  display.print(" km");
  display.print("\n");

  display.print("regenerationStatus: ");
  display.print(regenerationStatus);
  display.print(" %");

  display.display();
}

void oledPrintText(String text) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print(text);
  display.print("\n");

  display.display();
}

void oledPrintFloat(String pidName, float pidValue, String valueUnit, String error) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print(pidName + ": ");
  display.print(pidValue);
  display.print(" " + valueUnit);
  display.print("\n");
  display.print("Errore: " + error);
  display.print("\n");

  display.display();
}

void oledPrintInt(String pidName, int32_t pidValue, String error) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print(pidName + ": ");
  display.print(pidValue);
  display.print("\n");
  display.print("Errore: " + error);
  display.print("\n");

  display.display();
}

void oledClearDisplay() {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.display();
}

void oledInit() {

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    oled_ko = true;
  }
  else {

    display.clearDisplay();
    display.display();      

    Serial.println(F("SSD1306 ok"));
  }
}

void elm327CheckOrInit() {

  if (!bluetoothManager.isConnected()) {

    Serial.println("ELM327, could not be initialized because bluetooth is not connected...");
    oledPrintText("ELM327, could not be initialized because bluetooth is not connected...");
    return;
  }

  if (isDeviceELM327Initialized) {
    return;
  }

  Serial.println("ELM327, begining...");
  oledPrintText("ELM327, begining...");

  BluetoothSerial* bs = bluetoothManager.getBtSerial();
  if (!deviceELM327.begin(*bs, DEBUG_MODE, 2000)) {
      Serial.println("Couldn't connect to OBD scanner - Phase 2");
      oledPrintText("Couldn't connect to OBD scanner - Phase 2");
      isDeviceELM327Initialized = false;
  }

  // Set a custom header using ATSH command in order to access additional custom data
  if (deviceELM327.sendCommand_Blocking("AT SH 7E0") != ELM_SUCCESS) {
      Serial.println("Unable to set custom header 7E0");
      oledPrintText("Unable to set custom header 7E0");
      isDeviceELM327Initialized = false;
      delay(3000);
  }

  Serial.println("Connected to ELM327");
  oledPrintText("Connected to ELM327");
  isDeviceELM327Initialized = true;
}

String elm327GetNbRxStateString() {

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

int32_t getRegenerationStatus() {
  return deviceELM327.processPID(0x22, 0x3274, 1, 1, 100.0 / 255.0);
}

int32_t getKmsSinceDpf() {
  return deviceELM327.processPID(0x22, 0x3277, 1, 3);
}

int32_t getDpfDirtLevel() {
  return deviceELM327.processPID(0x22, 0x3275, 1, 1);
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
bool elm327ReadFloatData(String pidName, float value, String valueUnit) {

  bool readDone = false;

  if (deviceELM327.nb_rx_state == ELM_SUCCESS) {
    Serial.println(pidName + ": " + value);
    readDone = true;
  }
  else if (deviceELM327.nb_rx_state != ELM_GETTING_MSG) {
    deviceELM327.printError();
    readDone = true;
  }

  if (DEBUG_MODE) {
    oledPrintFloat(pidName, value, valueUnit, elm327GetNbRxStateString());
    
    if (readDone) {
      // For debug purpose
      delay(2000);
    }
  }
  
  return readDone;
}

void elm327ReadAllData() {

  if (!isDeviceELM327Initialized) {
    Serial.println("ELM327, could not read all data because ELM327 is not initialized...");
    oledPrintText("ELM327, could not read all data because ELM327 is not initialized...");
    return;
  }

  switch (obd_state)
  {
    case ENG_RPM:
     {
      rpm = deviceELM327.rpm();

      if (elm327ReadFloatData("RPM", rpm, "")) {
        obd_state = BATTERY_VOLTAGE;
      }
      
      break;
    }
    
    case BATTERY_VOLTAGE:
    {
      batteryVoltage = deviceELM327.batteryVoltage();

      if (elm327ReadFloatData("batteryVoltage", batteryVoltage, "V")) {
        obd_state = COMMANDEDEGR;
      }
      
      break;
    }

    case COMMANDEDEGR:
    {
      commandedEGR = deviceELM327.commandedEGR();

      if (elm327ReadFloatData("commandedEGR", commandedEGR, "%")) {
        obd_state = EGRERROR;
      }
      
      break;
    }

    case EGRERROR:
    {
      egrError = deviceELM327.egrError();

      if (elm327ReadFloatData("egrError", egrError, "%")) {
        obd_state = MANIFOLDPRESSURE;
      }
      
      break;
    }

    case MANIFOLDPRESSURE:
    {
      manifoldPressure = deviceELM327.manifoldPressure();

      if (elm327ReadFloatData("manifoldPressure", manifoldPressure, "kPa")) {
        obd_state = DPF_DIRT_LEVEL;
      }
      
      break;
    }

    case DPF_DIRT_LEVEL:
    {
      dpfDirtLevel = getDpfDirtLevel();

      if (elm327ReadFloatData("dpfDurtLevel", dpfDirtLevel, "%")) {
        obd_state = DPF_KMS_SINCE;
      }
      
      break;
    }

    case DPF_KMS_SINCE:
    {
      kmsSinceDpf = getKmsSinceDpf();

      if (elm327ReadFloatData("kmsSinceDpf", kmsSinceDpf, "km")) {
        obd_state = DPF_REGEN_STATUS;
      }
      
      break;
    }

    case DPF_REGEN_STATUS:
    {
      regenerationStatus = getRegenerationStatus();

      if (elm327ReadFloatData("regenerationStatus", regenerationStatus, "%")) {
        obd_state = ENG_RPM;
      }
      
      break;
    }
  }

  oledPrintData();
}

void setup()
{
    Serial.begin(115200);

    // Bluetooth init
    bluetoothManager.init();

    // OLED init
    oledInit();    
}

void loop() {

  if (!bluetoothManager.isConnected()) {

    oledPrintText("Initializing...");

    // If BT is not connected, forse EML327 init
    isDeviceELM327Initialized = false;
  }

  bluetoothManager.checkOrConnect();
  
  elm327CheckOrInit();

  elm327ReadAllData();

  delay(100);
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