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

const PidObj pidDefs[MANAGED_PIDS_COUNT] = {

    //     id                   name            unit    decimal min     max
    PidObj(BATTERY_VOLTAGE,     "Battery",      "v",    2,      12,     14.6),
    PidObj(COMMANDEDEGR,        "Cmd EGR",      "%",    2,      0,      100),
    PidObj(EGRERROR,            "EGR Error",    "%",    2,      -100,   100),
    PidObj(MANIFOLDPRESSURE,    "Manifold",     "bar",  2,      0,      1.6),
    PidObj(DPF_DIRT_LEVEL,      "DPF level",    "%",    0,      0,      100),
    PidObj(DPF_KMS_SINCE,       "DPF last",     "km",   0,      0,      500),
    PidObj(DPF_REGEN_STATUS,    "DPF Regen.",   "%",    0,      0,      100),
    PidObj(ENG_COOLANT_TEMP,    "ECT",          "C",    0,      -20,    120),
    PidObj(OIL_TEMP,            "Oil",          "C",    0,      -20,    120),
    PidObj(CAT_TEMP_B1S1,       "Cat. B1 S1",   "C",    0,      -20,    800),
    PidObj(CAT_TEMP_B1S2,       "Cat. B1 S2",   "C",    0,      -20,    800),
    PidObj(CAT_TEMP_B2S1,       "Cat. B2 S1",   "C",    0,      -20,    800),
    PidObj(CAT_TEMP_B2S2,       "Cat. B2 S2",   "C",    0,      -20,    800),
    PidObj(ENG_LOAD,            "Load",         "%",    0,      0,      100),
    PidObj(TORQUE_DEMANDED,     "Torque Dem.",  "%",    0,      0,      100),
    PidObj(TORQUE_REFERENCE,    "Torque Ref.",  "Nm",   0,      0,      200),
    PidObj(TORQUE,              "Torque",       "%",    0,      0,      100)
};