#include "Arduino.h"
#include "ELMduino.h"
#include "BluetoothSerial.h"

#define MANAGED_PIDS_COUNT 17

typedef enum { 
        BATTERY_VOLTAGE,
        COMMANDEDEGR,
        EGRERROR,
        MANIFOLDPRESSURE,
        DPF_DIRT_LEVEL,
        DPF_KMS_SINCE,
        DPF_REGEN_STATUS,
        ENG_COOLANT_TEMP,
        OIL_TEMP,
        CAT_TEMP_B1S1,
        CAT_TEMP_B1S2,
        CAT_TEMP_B2S1,
        CAT_TEMP_B2S2,
        ENG_LOAD,
        TORQUE_DEMANDED,
        TORQUE_REFERENCE,
        TORQUE
        } managed_pids;


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
                float ect = -1;
                float oil = -1;
                float catB1S1 = -1;
                float catB1S2 = -1;
                float catB2S1 = -1;
                float catB2S2 = -1;
                float engLoad = -1;
                float torqueDem = -1;
                float torqueRef = -1;
                float torque = -1;

                unsigned long pidsLastGetMs[MANAGED_PIDS_COUNT];

                managed_pids currentReadingPid = BATTERY_VOLTAGE;

                managed_pids nextPidToRead();
                bool isRecentlyGet(managed_pids pid);
                bool readFloatData(String pidName, float value);
                int32_t readRegenerationStatus();
                int32_t readKmsSinceDpf();
                int32_t readDpfDirtLevel();
};
