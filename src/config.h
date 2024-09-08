
const char * const OBD_BT_DEVICE_NAME = "vLinker MC-Android";

const bool DEBUG_MODE = false;

// loop(), delay in ms
const unsigned long LOOP_DELAY = 500;

// delay between reading data from EML327Manager (in ms)
const unsigned long READ_ELM327_DATA_DELAY = 5;

// Read PID data if it was getted in the last X ms
const unsigned long READ_ELM327_DATA_GET_LIMIT_MS = 5000;

const unsigned int DISPLAY_WARNING_BLINK_COUNT = 6;
const unsigned int DISPLAY_WARNING_BLINK_DURATION = 1000;
const unsigned int DISPLAY_WARNING_VALUE_DURATION = 5000;
const unsigned int DISPLAY_PIDS_ROTATION_DELAY = 5000;
