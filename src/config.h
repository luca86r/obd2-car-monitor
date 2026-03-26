
const char * const OBD_BT_DEVICE_NAME = "vLinker MC-Android";

// #define DEBUG_MODE  // Uncomment to enable serial debug output

#ifdef DEBUG_MODE
  #define DBG_PRINT(x)    Serial.print(x)
  #define DBG_PRINTLN(x)  Serial.println(x)
  #define DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
  #define DBG_CALL(x)     (x)
#else
  #define DBG_PRINT(x)
  #define DBG_PRINTLN(x)
  #define DBG_PRINTF(...)
  #define DBG_CALL(x)
#endif

// loop(), delay in ms
const unsigned long LOOP_DELAY = 300;

// delay between reading data from EML327Manager (in ms)
const unsigned long READ_ELM327_DATA_DELAY = 0;

// Read PID data if it was getted in the last X ms
const unsigned long READ_ELM327_DATA_GET_LIMIT_MS = 5000;

// Enable prefetch of PID's value
const bool PIDS_PREFETCH_ENABLED = false;

const unsigned int DISPLAY_WARNING_BLINK_COUNT = 6;
const unsigned int DISPLAY_WARNING_BLINK_DURATION = 1000;
const unsigned int DISPLAY_WARNING_VALUE_DURATION = 5000;
const unsigned int DISPLAY_PIDS_ROTATION_DELAY = 5000;

// Auto mode
const unsigned long AUTO_DISPLAY_BATTERY_ON_START_FOR_MILLIS = 30000;
const unsigned long AUTO_DISPLAY_EGR_ERROR_FLIP_FLOP_ON_ENGINE_STOP_FOR_MILLIS = 10000;
const float AUTO_DISPLAY_DPF_DIRT_LEVEL_THRESHOLD = 80.0; // in %
const unsigned long ENGINE_COOLANT_IS_COLD_THRESHOLD = 65;
