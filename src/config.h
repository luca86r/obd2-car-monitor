
const char * const OBD_BT_DEVICE_NAME = "vLinker MC-Android";

const bool DEBUG_MODE = false;

// loop(), delay in ms
const unsigned long LOOP_DELAY = 400;

// delay between reading data from EML327Manager (in ms)
const unsigned long READ_ELM327_DATA_DELAY = 5;

// Read PID data if it was getted in the last X ms
const unsigned long READ_ELM327_DATA_GET_LIMIT_MS = 5000;

const unsigned int DISPLAY_WARNING_BLINK_COUNT = 6;
const unsigned int DISPLAY_WARNING_BLINK_DURATION = 1000;
const unsigned int DISPLAY_WARNING_VALUE_DURATION = 5000;
const unsigned int DISPLAY_PIDS_ROTATION_DELAY = 5000;

// Auto mode
const unsigned long AUTO_DISPLAY_BATTERY_ON_START_FOR_MILLIS = 30000;
const unsigned long OIL_COLD_THRESHOLD = 70;
