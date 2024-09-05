#include "BluetoothSerial.h"
#include "ELMduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Preferences.h"

// === Preferences ===
Preferences preferences;
String pairedDeviceAddress = "";

// === Bluetooth ===
BluetoothSerial SerialBT;

#define BT_DISCOVER_TIME  10000
esp_spp_sec_t sec_mask=ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role=ESP_SPP_ROLE_MASTER; // or ESP_SPP_ROLE_MASTER

const String OBD_BT_DEVICE_NAME = "vLinker MC-Android";

// === ELM327 ===
ELM327 deviceELM327;
bool isDeviceELM327Initialized = false;

typedef enum { ENG_RPM,
               BATTERY_VOLTAGE,
               COMMANDEDEGR,
               EGRERROR,
               MANIFOLDPRESSURE,
               CUSTOMRPM,
               DPFDURTLEVEL} obd_pid_states;

obd_pid_states obd_state = ENG_RPM;

float rpm = 0;
float batteryVoltage = 0;
float commandedEGR = 0;
float egrError = 0;
float manifoldPressure = 0;

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
  
  display.print("RPM: ");
  display.print(rpm);
  display.print("\n");
  display.print("batteryVoltage: ");
  display.print(batteryVoltage);
  display.print("\n");
  display.print("commandedEGR: ");
  display.print(commandedEGR);
  display.print("\n");
  display.print("egrError: ");
  display.print(egrError);
  display.print("\n");
  display.print("manifoldPressure: ");
  display.print(manifoldPressure);
  display.print("\n");

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

void oledPrintFloat(String pidName, float pidValue, String error) {

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

void btDiscoverDevice() {

  Serial.println("BT - Starting discoverAsync...");
  oledPrintText("BT - Searching device...");

  BTScanResults* btDeviceList = SerialBT.getScanResults();  // maybe accessing from different threads!
  if (SerialBT.discoverAsync([](BTAdvertisedDevice* pDevice) {
      // BTAdvertisedDeviceSet*set = reinterpret_cast<BTAdvertisedDeviceSet*>(pDevice);
      // btDeviceList[pDevice->getAddress()] = * set;
      Serial.printf("BT - >>>>>>>>>>>Found a new device asynchronously: %s\n", pDevice->toString().c_str());
    } )
    ) {
    delay(BT_DISCOVER_TIME);

    Serial.print("BT - Stopping discoverAsync... ");
    SerialBT.discoverAsyncStop();
    Serial.println("BT - discoverAsync stopped");
    delay(5000);

    if(btDeviceList->getCount() > 0) {
      
      BTAddress addr;
      int channel=0;
      String *deviceName;

      Serial.println("BT - Found devices:");
      for (int i=0; i < btDeviceList->getCount(); i++) {
        
        BTAdvertisedDevice *device=btDeviceList->getDevice(i);

        deviceName = new String(device->getName().c_str());
        if (!deviceName->equals(OBD_BT_DEVICE_NAME)) {
          Serial.printf("BT ----- %s  %s %d [SKIPPING]\n", device->getAddress().toString().c_str(), device->getName().c_str(), device->getRSSI());
          continue;
        } 

        Serial.printf("BT ----- %s  %s %d [FOUND]\n", device->getAddress().toString().c_str(), device->getName().c_str(), device->getRSSI());
        std::map<int,std::string> channels=SerialBT.getChannels(device->getAddress());
        Serial.printf("BT - scanned for services, found %d\n", channels.size());
        for(auto const &entry : channels) {
          Serial.printf("BT -      channel %d (%s)\n", entry.first, entry.second.c_str());
        }

        addr = device->getAddress();
        if(channels.size() > 0) {
          channel=channels.begin()->first;
        }
      }

      if(addr) {
        Serial.printf("BT - connecting to %s - %d\n", addr.toString().c_str(), channel);
        
        if (SerialBT.connect(addr, channel, sec_mask, role)) {
          Serial.println("BT - Connected!; Saving MAC address" + addr.toString());
          preferences.putString("MACAddress", addr.toString());
          SerialBT.disconnect();
        }
      }
      
    } else {
      Serial.println("BT - Didn't find any devices");
      oledPrintText("BT - Didn't find any devices");
    }
  } else {
    Serial.println("BT - Error on discoverAsync f.e. not workin after a \"connect\"");
  }
}

void btConnectBySavedMAC() {

  // Recupera l'indirizzo MAC salvato nella memoria NVS
  pairedDeviceAddress = preferences.getString("MACAddress", "");

  if (pairedDeviceAddress != "") {
    Serial.println("BT - MAC Address salvato: " + pairedDeviceAddress);
    oledPrintText("BT - MAC Address salvato: " + pairedDeviceAddress);

    Serial.println("BT - Connetting by MAC...");
    oledPrintText("BT - Connetting by MAC...");

    BTAddress addr = BTAddress(pairedDeviceAddress);

    if (SerialBT.connect(addr)) {
        Serial.println("BT - Connected by MAC!");
        oledPrintText("BT - Connected by MAC!");
    }
    else {
      Serial.println("BT - Couldn't connect to BT OBD scanner by MAC - Phase 1");
      oledPrintText("BT - Couldn't connect to BT OBD scanner by MAC - Phase 1");
    }
  }
}

bool btIsConnected() {
  return SerialBT.connected(1000);
}

void btCheckOrConnect() {

  if (btIsConnected()) {
    return;
  }

  // Force ELM327 init
  isDeviceELM327Initialized = false;

  btConnectBySavedMAC();

  if (!btIsConnected()) {

    btDiscoverDevice();
    btConnectBySavedMAC();
  }
}

void elm327CheckOrInit() {

  if (!btIsConnected()) {

    Serial.println("ELM327, could not be initialized because bluetooth is not connected...");
    oledPrintText("ELM327, could not be initialized because bluetooth is not connected...");
    return;
  }

  if (isDeviceELM327Initialized) {
    return;
  }

  Serial.println("ELM327, begining...");
  oledPrintText("ELM327, begining...");

  if (!deviceELM327.begin(SerialBT, true, 2000)) {
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

int32_t elm327QueryPID(uint8_t service, uint16_t pid) {
  Serial.print("Response for ");
  Serial.print(pid);
  Serial.print(": ");
  /*if (deviceELM327.queryPID(service, pid)) {
    int32_t response = deviceELM327.findResponse(service, pid);
    if (deviceELM327.nb_rx_state == ELM_SUCCESS) {
      return response;
    }
    else {
      return -3;
    }
  }
  else {
    return -2;
  }
  return -1;*/


  return deviceELM327.processPID(service, pid, 1, 1);
}

int32_t getRegenerationStatus() {
  return elm327QueryPID(0x22, 0x3274);
}

int32_t getKmsSinceDpf() {
  return elm327QueryPID(0x22, 0x3277);
}

int32_t getDpfDirtLevel() {
  return elm327QueryPID(0x22, 0x3275);
}

int32_t getRpmCustom() {
  return elm327QueryPID(SERVICE_01, ENGINE_RPM);
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
bool elm327ReadFloatData(String pidName, float value) {

  bool readDone = false;

  if (deviceELM327.nb_rx_state == ELM_SUCCESS) {
    Serial.println(pidName + ": " + value);
    readDone = true;
  }
  else if (deviceELM327.nb_rx_state != ELM_GETTING_MSG) {
    deviceELM327.printError();
    readDone = true;
  }

  oledPrintFloat(pidName, value, elm327GetNbRxStateString());
  
  if (readDone) {
    // For debug purpose
    delay(2000);
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

      if (elm327ReadFloatData("RPM", rpm)) {
        obd_state = BATTERY_VOLTAGE;
      }
      
      break;
    }
    
    case BATTERY_VOLTAGE:
    {
      batteryVoltage = deviceELM327.batteryVoltage();

      if (elm327ReadFloatData("batteryVoltage", batteryVoltage)) {
        obd_state = COMMANDEDEGR;
      }
      
      break;
    }

    case COMMANDEDEGR:
    {
      commandedEGR = deviceELM327.commandedEGR();

      if (elm327ReadFloatData("commandedEGR", commandedEGR)) {
        obd_state = EGRERROR;
      }
      
      break;
    }

    case EGRERROR:
    {
      egrError = deviceELM327.egrError();

      if (elm327ReadFloatData("egrError", egrError)) {
        obd_state = MANIFOLDPRESSURE;
      }
      
      break;
    }

    case MANIFOLDPRESSURE:
    {
      manifoldPressure = deviceELM327.manifoldPressure();

      if (elm327ReadFloatData("manifoldPressure", manifoldPressure)) {
        obd_state = DPFDURTLEVEL;
      }
      
      break;
    }

    case DPFDURTLEVEL:
    {
      int32_t dpfDurtLevel = getDpfDirtLevel();

      if (elm327ReadFloatData("dpfDurtLevel", dpfDurtLevel)) {
        obd_state = ENG_RPM;
      }
      
      break;
    }

    /*case CUSTOMRPM:
    {
      int32_t custom_rpm = getRpmCustom();

      if (elm327ReadFloatData("custom_rpm", custom_rpm)) {
        obd_state = ENG_RPM;
      }
      
      break;
    }*/
  }

  //oledPrintData();
}

void setup()
{
    Serial.begin(115200);

    // Bluetooth init
    SerialBT.begin("obd2-car-monitor", true);

    // OLED init
    oledInit();    

    // Init NVS memory for preferences
    preferences.begin("Bluetooth", false);
}

void loop() {

  btCheckOrConnect();
  
  elm327CheckOrInit();

  elm327ReadAllData();
  
  delay(100);
}