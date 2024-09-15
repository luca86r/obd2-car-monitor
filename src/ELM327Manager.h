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
                float getDataForPID(managed_pids pid, bool prefetchNext);
                String getUnitForPID(managed_pids pid);
                int getPercentageForPID(managed_pids pid);
                int getDecimalPointForPID(managed_pids pid);

                void readAllData();

	private:
                ELM327 deviceELM327;
                BluetoothSerial *btSerial;

                bool isDeviceELM327Initialized = false;

                unsigned long pidsLastGetMs[MANAGED_PIDS_COUNT];
                managed_pids currentReadingPid = BATTERY_VOLTAGE;

                managed_pids nextPidToRead();
                bool isRecentlyGet(managed_pids pid);
                bool isNonBlockingReadCompleted(String pidName, float value);
                int32_t readRegenerationStatus();
                int32_t readKmsSinceDpf();
                int32_t readDpfDirtLevel();
};
