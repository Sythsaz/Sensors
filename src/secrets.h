#define SECRET_DEBUG "true"

#define SECRET_SSID "ShawEx"
#define SECRET_PASS "Ranger1204!"

/////////////////////////////////////////////////////////

#define ledBu "LED_BUILTIN"

struct intvars // struct comman vars
{
    int CO2PPM0;
    int TVOCPPB0;
};

struct boolvars // struct common bools
{
    bool CCS811Fail = true;
    bool sensorCheck;
    bool wifi_setup = false;
    bool sendData = false;
};