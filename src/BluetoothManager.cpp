#include "BluetoothManager.h"
#include "config.h"

#define BT_DISCOVER_TIME  10000
esp_spp_sec_t bt_sec_mask=ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t bt_role=ESP_SPP_ROLE_MASTER; // or ESP_SPP_ROLE_MASTER

void BluetoothManager::init() {

    serialBT.begin("obd2-car-monitor", true);

    // Init NVS memory for preferences
    this->preferences.begin("Bluetooth", false);
}

BluetoothSerial* BluetoothManager::getBtSerial() {
    return &serialBT;
}

bool BluetoothManager::isConnected() {
  return serialBT.connected(1000);
}


void BluetoothManager::checkOrConnect() {

  if (isConnected()) {
    return;
  }

  connectBySavedMAC();

  if (!isConnected()) {

    discoverDevice();
    connectBySavedMAC();
  }
}

void BluetoothManager::discoverDevice() {

  Serial.println("BT - Starting discoverAsync...");

  BTScanResults* btDeviceList = serialBT.getScanResults();  // maybe accessing from different threads!
  if (serialBT.discoverAsync([](BTAdvertisedDevice* pDevice) {
      // BTAdvertisedDeviceSet*set = reinterpret_cast<BTAdvertisedDeviceSet*>(pDevice);
      // btDeviceList[pDevice->getAddress()] = * set;
      Serial.printf("BT - >>>>>>>>>>>Found a new device asynchronously: %s\n", pDevice->toString().c_str());
    } )
    ) {
    delay(BT_DISCOVER_TIME);

    Serial.print("BT - Stopping discoverAsync... ");
    serialBT.discoverAsyncStop();
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
        std::map<int,std::string> channels=serialBT.getChannels(device->getAddress());
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
        
        if (serialBT.connect(addr, channel, bt_sec_mask, bt_role)) {
          Serial.println("BT - Connected!; Saving MAC address" + addr.toString());
          this->preferences.putString("MACAddress", addr.toString());
          serialBT.disconnect();
        }
      }
      
    } else {
      Serial.println("BT - Didn't find any devices");
    }
  } else {
    Serial.println("BT - Error on discoverAsync f.e. not workin after a \"connect\"");
  }
}

void BluetoothManager::connectBySavedMAC() {

  // Recupera l'indirizzo MAC salvato nella memoria NVS
  pairedDeviceAddress = this->preferences.getString("MACAddress", "");

  if (pairedDeviceAddress != "") {
    Serial.println("BT - MAC Address salvato: " + pairedDeviceAddress);
    Serial.println("BT - Connetting by MAC...");

    BTAddress addr = BTAddress(pairedDeviceAddress);

    if (serialBT.connect(addr)) {
        Serial.println("BT - Connected by MAC!");
    }
    else {
      Serial.println("BT - Couldn't connect to BT OBD scanner by MAC - Phase 1");
    }
  }
}