#include "Arduino.h"
#include "ELMduino.h"
#include "BluetoothSerial.h"

#define MANAGED_PIDS_COUNT 7

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

                unsigned long pidsLastGetMs[MANAGED_PIDS_COUNT];

                managed_pids currentReadingPid = BATTERY_VOLTAGE;

                managed_pids nextPidToRead();
                bool isRecentlyGet(managed_pids pid);
                bool readFloatData(String pidName, float value);
                int32_t readRegenerationStatus();
                int32_t readKmsSinceDpf();
                int32_t readDpfDirtLevel();
};
