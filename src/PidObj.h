#include <Arduino.h>

#define PID_NO_VALUE -1

class PidObj {
	public:

        PidObj(int pidId, const char* name, const char* unit, int fDecimalPoint);
        PidObj(int pidId, const char* name, const char* unit, int fDecimalPoint, float fMinValue, float fMaxValue, long minReadDelayFromEmlMs);

        int getPidId();
        String getName();
        String getUnit();
        float getFValue();
        void setFValue(float& value);
        int getFDecimalPoint();
        float getFMinValue();
        float getFMaxValue();
        long getMinReadDelayFromEmlMs();

    private:
        // ID of the PID
        int pidId = -1;

        // Name of the PID
        const char* name;

        // Unit of the float value of the PID
        const char* unit;

        // Float value of the PID
        float fValue = PID_NO_VALUE;

        // Decimal point of the float value of the PID
        int fDecimalPoint = 0;

        // Min value of the float value of the PID
        float fMinValue = -1;
        
        // Max value of the float value of the PID
        float fMaxValue = -1;

        // Min delay between two reading of this PID from EML
        long minReadDelayFromEmlMs = 1000;
};