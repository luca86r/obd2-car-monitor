#include "BluetoothSerial.h"
#include "Arduino.h"
#include "Preferences.h"

class BluetoothManager {
	public:
		void init();
		BluetoothSerial* getBtSerial();
		bool isConnected();
		void checkOrConnect();
		void discoverDevice();
		void connectBySavedMAC();
	
	private:
	
		Preferences preferences;
		String pairedDeviceAddress = "";
		BluetoothSerial serialBT;
};