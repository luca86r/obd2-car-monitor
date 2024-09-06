#include "Arduino.h"
#include "ELMduino.h"
#include "BluetoothSerial.h"

class ELM327Manager {
	public:
		void checkOrInit(BluetoothSerial *btSerial);
        bool isInitialized();
        void resetInitState();

        String getNbRxStateString();

        float getBatteryVoltage();
        float getCommandedEGR();
        float getEgrError();
        float getManifoldPressure();
        int32_t getRegenerationStatus();
        int32_t getKmsSinceDpf();
        int32_t getDpfDirtLevel();

        void readAllData();
        
	
	private:
        ELM327 deviceELM327;
        BluetoothSerial *btSerial;

        bool isDeviceELM327Initialized = false;
        float batteryVoltage = 0;
        float commandedEGR = 0;
        float egrError = 0;
        float manifoldPressure = 0;
        int32_t kmsSinceDpf = 0;
        int32_t dpfDirtLevel = 0;
        int32_t regenerationStatus = 0;

        typedef enum { 
               BATTERY_VOLTAGE,
               COMMANDEDEGR,
               EGRERROR,
               MANIFOLDPRESSURE,
               DPF_DIRT_LEVEL,
               DPF_KMS_SINCE,
               DPF_REGEN_STATUS} obd_pid_states;

        obd_pid_states obd_state = BATTERY_VOLTAGE;

        bool readFloatData(String pidName, float value, String valueUnit);
        int32_t readRegenerationStatus();
        int32_t readKmsSinceDpf();
        int32_t readDpfDirtLevel();
};