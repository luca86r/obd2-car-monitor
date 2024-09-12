#include <Arduino.h>

#define PID_NO_VALUE -1

class PidObj {
	public:

        PidObj(int pidId, String name, String unit, int fDecimalPoint);
        PidObj(int pidId, String name, String unit, int fDecimalPoint, float fMinValue, float fMaxValue);

        int getPidId();
        String getName();
        String getUnit();
        float getFValue();
        void setFValue(float value, unsigned long lastReadMs);
        int getFDecimalPoint();
        float getFMinValue();
        float getFMaxValue();
        unsigned long getLastReadMs();

    private:
        // ID of the PID
        int pidId = -1;

        // Name of the PID
        String name = "";

        // Unit of the float value of the PID
        String unit = "";

        // Float value of the PID
        float fValue = PID_NO_VALUE;

        // Decimal point of the float value of the PID
        int fDecimalPoint = 0;

        // Min value of the float value of the PID
        float fMinValue = -1;
        
        // Max value of the float value of the PID
        float fMaxValue = -1;

        // Ms of the last read operation from the car
        unsigned long lastReadMs = 0;

};