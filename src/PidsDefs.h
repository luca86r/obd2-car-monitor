#include "PidObj.h"

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

const PidObj pidDefs[1] = {

    PidObj(BATTERY_VOLTAGE, "Battery", "v", 2, 12, 14.6)
};