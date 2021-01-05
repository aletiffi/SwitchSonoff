#define SONOFF_BUTTON             0
#define SONOFF_RELAY              12
#define SONOFF_LED                13
#define SONOFF_SWITCH             14
//----------------------------------------------------------------------------
#define OFF_RELAY                 LOW
#define OFF_LED                   HIGH
//----------------------------------------------------------------------------
#define BLINK_TIME                1000        // Delay cambio programma
#define TIME_FLASH_BLINK          100
#define PLACING_TIME              200
#define TIMER_CONNECTION          300000      // Controllo connessione ogni 5 min
#define MQTT_CONNECTION           5000
//----------------------------------------------------------------------------
#define EEPROM_SIZE               256
//----------------------------------------------------------------------------
#define NUM_WIFI_SETTINGS         8
#define MAX_LENGTH_SETTING        16
//----------------------------------------------------------------------------
#define ON_PAYLOAD                "ON"
#define OFF_PAYLOAD               "OFF"
//----------------------------------------------------------------------------
#define JSON_MSG_LENGTH           32
//----------------------------------------------------------------------------
#define ProductKey                "fce33026-9a4d-47c1-a8fd-d6ff4d4cc135"
#define Version                   "31.1.0.0"
#define MakeFirmwareInfo(k, v)    "&_FirmwareInfo&k=" k "&v=" v "&FirmwareInfo_&"
//----------------------------------------------------------------------------
bool pushButton                   = false;
bool pushButtonPre                = false;
bool switchState                  = false;
bool switchStatePre               = false;
bool deviceConnected              = false;
//----------------------------------------------------------------------------
int pushButtonCount               = 0;
//----------------------------------------------------------------------------
unsigned long pushButtonTime      = 0;
unsigned long lastTimeCheckConn   = 0;
unsigned long lastMqttCheckConn   = 0;
//----------------------------------------------------------------------------
String DefaultApName              = "Sonoff_AP";
String mac                        = "";

struct WifiSetup{
  String Name;
  String Val;
};

WifiSetup Hostname                = {"Hostname", ""};
WifiSetup Ssid                    = {"Ssid", ""};
WifiSetup Password                = {"Password", ""};
WifiSetup MqttSubTopic            = {"MqttSubTopic", ""};
WifiSetup MqttPubTopic            = {"MqttPubTopic", ""};
WifiSetup MqttServer              = {"MqttServer", ""};
WifiSetup MqttUser                = {"MqttUser", ""};
WifiSetup MqttPassword            = {"MqttPassword", ""};

WifiSetup* WifiSettings[NUM_WIFI_SETTINGS] = {
  &Hostname, 
  &Ssid, 
  &Password, 
  &MqttSubTopic, 
  &MqttPubTopic,
  &MqttServer, 
  &MqttUser, 
  &MqttPassword};
