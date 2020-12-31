#define SONOFF_BUTTON           0
#define SONOFF_RELAY            12
#define SONOFF_LED              13
#define SONOFF_SWITCH           14
//----------------------------------------------------------------------------
#define OFF_RELAY               LOW
#define OFF_LED                 HIGH
//----------------------------------------------------------------------------
#define BLINK_TIME              1000        // Delay cambio programma
#define TIME_FLASH_BLINK        100
#define PLACING_TIME            200
#define TIMER_CONNECTION        300000      // Controllo connessione ogni 5 min
//----------------------------------------------------------------------------
#define EEPROM_SIZE             256
//----------------------------------------------------------------------------
#define NUM_WIFI_SETTINGS       8
#define MAX_LENGTH_SETTING      16
//----------------------------------------------------------------------------
bool pushButton                 = false;
bool pushButtonPre              = false;
bool switchState                = false;
bool switchStatePre             = false;
bool deviceConnected            = false;
//----------------------------------------------------------------------------
int pushButtonCount             = 0;
//----------------------------------------------------------------------------
unsigned long pushButtonTime    = 0;
unsigned long lastTimeCheckConn = 0;
//----------------------------------------------------------------------------
String DefaultApName            = "Sonoff AP";

struct WifiSetup{
  String Name;
  String Val;
};

WifiSetup Hostname           = {"Hostname", ""};
WifiSetup Ssid               = {"Ssid", ""};
WifiSetup Password           = {"Password", ""};
WifiSetup Set_Light_Sub      = {"Set_Light_Sub", ""};
WifiSetup Set_Light_Pub      = {"Set_Light_Pub", ""};
WifiSetup MqttServer         = {"MqttServer", ""};
WifiSetup MqttUser           = {"MqttUser", ""};
WifiSetup MqttPassword       = {"MqttPassword", ""};

WifiSetup* WifiSettings[NUM_WIFI_SETTINGS] = {
  &Hostname, 
  &Ssid, 
  &Password, 
  &Set_Light_Sub, 
  &Set_Light_Pub, 
  &MqttServer, 
  &MqttUser, 
  &MqttPassword};
