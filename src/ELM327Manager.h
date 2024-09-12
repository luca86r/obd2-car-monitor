#include "Arduino.h"
#include "ELMduino.h"
#include "BluetoothSerial.h"
#include "PidsDefs.h"

class ELM327Manager {
	public:
		void checkOrInit(BluetoothSerial *btSerial);
                bool isInitialized();
                void resetInitState();

                String getNbRxStateString();

                String getNameForPID(managed_pids pid);
                float getDataForPID(managed_pids pid);
                String getUnitForPID(managed_pids pid);
                int getPercentageForPID(managed_pids pid);
                int getDecimalPointForPID(managed_pids pid);

                void readAllData();

	private:
                ELM327 deviceELM327;
                BluetoothSerial *btSerial;

                bool isDeviceELM327Initialized = false;
                float batteryVoltage = PID_NO_VALUE;
                float commandedEGR = PID_NO_VALUE;
                float egrError = PID_NO_VALUE;
                float manifoldPressure = PID_NO_VALUE;
                int32_t kmsSinceDpf = PID_NO_VALUE;
                int32_t dpfDirtLevel = PID_NO_VALUE;
                int32_t regenerationStatus = PID_NO_VALUE;
                float ect = PID_NO_VALUE;
                float oil = PID_NO_VALUE;
                float catB1S1 = PID_NO_VALUE;
                float catB1S2 = PID_NO_VALUE;
                float catB2S1 = PID_NO_VALUE;
                float catB2S2 = PID_NO_VALUE;
                float engLoad = PID_NO_VALUE;
                float torqueDem = PID_NO_VALUE;
                float torqueRef = PID_NO_VALUE;
                float torque = PID_NO_VALUE;

                unsigned long pidsLastGetMs[MANAGED_PIDS_COUNT];

                managed_pids currentReadingPid = BATTERY_VOLTAGE;

                managed_pids nextPidToRead();
                bool isRecentlyGet(managed_pids pid);
                bool readFloatData(String pidName, float value);
                int32_t readRegenerationStatus();
                int32_t readKmsSinceDpf();
                int32_t readDpfDirtLevel();
};
