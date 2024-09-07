#include "Arduino.h"
#include "ELMduino.h"
#include "BluetoothSerial.h"

typedef enum { 
        BATTERY_VOLTAGE,
        COMMANDEDEGR,
        EGRERROR,
        MANIFOLDPRESSURE,
        DPF_DIRT_LEVEL,
        DPF_KMS_SINCE,
        DPF_REGEN_STATUS} managed_pids;

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

                String getNameForPID(managed_pids pid);
                float getDataForPID(managed_pids pid);
                String getUnitForPID(managed_pids pid);
                int getDecimalPointForPID(managed_pids pid);

                void readAllData();

	private:
                ELM327 deviceELM327;
                BluetoothSerial *btSerial;

                bool isDeviceELM327Initialized = false;
                float batteryVoltage = -1;
                float commandedEGR = -1;
                float egrError = -1;
                float manifoldPressure = -1;
                int32_t kmsSinceDpf = -1;
                int32_t dpfDirtLevel = -1;
                int32_t regenerationStatus = -1;

                managed_pids currentReadingPid = BATTERY_VOLTAGE;

                bool readFloatData(String pidName, float value);
                int32_t readRegenerationStatus();
                int32_t readKmsSinceDpf();
                int32_t readDpfDirtLevel();
};
