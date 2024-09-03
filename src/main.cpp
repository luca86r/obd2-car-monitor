#include "BluetoothSerial.h"
#include "ELMduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Preferences.h"

BluetoothSerial SerialBT;
#define ELM_PORT SerialBT
#define DEBUG_PORT Serial

#define BT_DISCOVER_TIME  10000
esp_spp_sec_t sec_mask=ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role=ESP_SPP_ROLE_MASTER; // or ESP_SPP_ROLE_MASTER

Preferences preferences;
String pairedDeviceAddress = "";
const String BT_DEVICE_NAME = "vLinker MC-Android";

bool serialBT_connected = false;

ELM327 myELM327;

bool myELM327_initialized = false;

typedef enum { ENG_RPM,
               BATTERY_VOLTAGE,
               COMMANDEDEGR,
               EGRERROR,
               MANIFOLDPRESSURE} obd_pid_states;

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

void oledPrintFloat(float f) {

  if (oled_ko) {
    return;  
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print("Valore: ");
  display.print(f);
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

void discover() {

  Serial.println("Starting discoverAsync...");

  BTScanResults* btDeviceList = SerialBT.getScanResults();  // maybe accessing from different threads!
  if (SerialBT.discoverAsync([](BTAdvertisedDevice* pDevice) {
      // BTAdvertisedDeviceSet*set = reinterpret_cast<BTAdvertisedDeviceSet*>(pDevice);
      // btDeviceList[pDevice->getAddress()] = * set;
      Serial.printf(">>>>>>>>>>>Found a new device asynchronously: %s\n", pDevice->toString().c_str());
    } )
    ) {
    delay(BT_DISCOVER_TIME);

    Serial.print("Stopping discoverAsync... ");
    SerialBT.discoverAsyncStop();
    Serial.println("discoverAsync stopped");
    delay(15000);

    if(btDeviceList->getCount() > 0) {
      
      BTAddress addr;
      int channel=0;
      String *deviceName;

      Serial.println("Found devices:");
      for (int i=0; i < btDeviceList->getCount(); i++) {
        
        BTAdvertisedDevice *device=btDeviceList->getDevice(i);

        deviceName = new String(device->getName().c_str());
        if (!deviceName->equals(BT_DEVICE_NAME)) {
          continue;
        } 

        
        
        Serial.printf(" ----- %s  %s %d\n", device->getAddress().toString().c_str(), device->getName().c_str(), device->getRSSI());
        std::map<int,std::string> channels=SerialBT.getChannels(device->getAddress());
        Serial.printf("scanned for services, found %d\n", channels.size());
        for(auto const &entry : channels) {
          Serial.printf("     channel %d (%s)\n", entry.first, entry.second.c_str());
        }

        addr = device->getAddress();

        if(channels.size() > 0) {
          channel=channels.begin()->first;
        }
      }

      if(addr) {
        Serial.printf("connecting to %s - %d\n", addr.toString().c_str(), channel);
        
        if (SerialBT.connect(addr, channel, sec_mask, role)) {
          Serial.println("Connesso alla periferica trovata; salvo il MAC" + addr.toString());
          preferences.putString("MACAddress", addr.toString());
          SerialBT.disconnect();
        }
      }
      
    } else {
      Serial.println("Didn't find any devices");
    }
  } else {
    Serial.println("Error on discoverAsync f.e. not workin after a \"connect\"");
  }
}

void connectByMAC() {

  // Recupera l'indirizzo MAC salvato nella memoria NVS
    pairedDeviceAddress = preferences.getString("MACAddress", "");

    if (pairedDeviceAddress != "") {
      Serial.println("MAC Address salvato: " + pairedDeviceAddress);
      oledPrintText("MAC Address salvato: " + pairedDeviceAddress);

      Serial.println("Connetting by MAC...");
      oledPrintText("Connetting by MAC...");

      BTAddress a = BTAddress(pairedDeviceAddress);

      if (ELM_PORT.connect(a))
      {
          serialBT_connected = true;
          Serial.println("Connected by MAC!");
          oledPrintText("Connected by MAC!");

      }
      else {
        DEBUG_PORT.println("Couldn't connect to OBD scanner by MAC - Phase 1");
        oledPrintText("Couldn't connect to OBD scanner by MAC - Phase 1");
      }
    }
}



void setup()
{
    DEBUG_PORT.begin(115200);
    ELM_PORT.begin("ArduHUD", true);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      oled_ko = true;
    }
    else {

      display.clearDisplay();
      display.display();      

      Serial.println(F("SSD1306 ok"));
    }

    // Inizializza la memoria NVS
    preferences.begin("Bluetooth", false);

    connectByMAC();

    if (!serialBT_connected) {

      discover();
      connectByMAC();

      /*

      Serial.println("Connetting by name...");
      oledPrintText("Connetting by name...");

      if (ELM_PORT.connect(BT_DEVICE_NAME))
      {
          serialBT_connected = true;
          Serial.println("Connected by name!");
          oledPrintText("Connected by name!");
      }
      else {
        DEBUG_PORT.println("Couldn't connect to OBD scanner by name - Phase 1");
        oledPrintText("Couldn't connect to OBD scanner by name - Phase 1");
        while (1)
            ;
      }
*/
    }


    /*Serial.println(ELM_PORT.connected(10000));
    Serial.println("Connetting...");
    oledPrintText("Connetting...");

    if (!ELM_PORT.connect(BT_DEVICE_NAME))
    {
        DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
        oledPrintText("Couldn't connect to OBD scanner - Phase 1");
        while (1)
            ;
    }

    Serial.println(ELM_PORT.connected(10000));

    */
    
    Serial.println("ELM327, begining...");
    oledPrintText("ELM327, begining...");

    if (!myELM327.begin(ELM_PORT, true, 2000))
    {
        DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 2");
        oledPrintText("Couldn't connect to OBD scanner - Phase 2");
        while (1)
            ;
    }

    DEBUG_PORT.println("Connected to ELM327");
    oledPrintText("Connected to ELM327");
}

void loop()
{
  switch (obd_state)
  {
    case ENG_RPM:
    {
      rpm = myELM327.rpm();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        DEBUG_PORT.print("rpm: ");
        DEBUG_PORT.println(rpm);
        oledPrintData();
        obd_state = BATTERY_VOLTAGE;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        myELM327.printError();
        obd_state = BATTERY_VOLTAGE;
        oledPrintText("RPM ELM_GETTING_MSG");
      }
      
      break;
    }
    
    case BATTERY_VOLTAGE:
    {
      batteryVoltage = myELM327.batteryVoltage();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        DEBUG_PORT.print("batteryVoltage: ");
        DEBUG_PORT.println(batteryVoltage);
        oledPrintData();
        obd_state = COMMANDEDEGR;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        myELM327.printError();
        obd_state = COMMANDEDEGR;
        oledPrintText("BATTERY_VOLTAGE ELM_GETTING_MSG");
      }
      
      break;
    }

    case COMMANDEDEGR:
    {
      commandedEGR = myELM327.commandedEGR();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        DEBUG_PORT.print("commandedEGR: ");
        DEBUG_PORT.println(commandedEGR);
        oledPrintData();
        obd_state = EGRERROR;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        myELM327.printError();
        obd_state = EGRERROR;
        oledPrintText("COMMANDED_EGR ELM_GETTING_MSG");
      }
      
      break;
    }

    case EGRERROR:
    {
      egrError = myELM327.egrError();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        DEBUG_PORT.print("egrError: ");
        DEBUG_PORT.println(egrError);
        oledPrintData();
        obd_state = MANIFOLDPRESSURE;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        myELM327.printError();
        obd_state = MANIFOLDPRESSURE;
        oledPrintText("EGR_ERROR ELM_GETTING_MSG");
      }
      
      break;
    }

    case MANIFOLDPRESSURE:
    {
      manifoldPressure = myELM327.manifoldPressure();
      
      if (myELM327.nb_rx_state == ELM_SUCCESS)
      {
        DEBUG_PORT.print("manifoldPressure: ");
        DEBUG_PORT.println(manifoldPressure);
        oledPrintData();
        obd_state = ENG_RPM;
      }
      else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      {
        myELM327.printError();
        obd_state = ENG_RPM;
        oledPrintText("MANIFOLDPRESSURE ELM_GETTING_MSG");
      }
      
      break;
    }

  }
}