/*
GPIO03 and GPIO02 are pulled high momentarily after boot. 
Any connected device may "blink" when the Sonoff is powering up.

Sonoff r1
    GPIO00 - BUTTON
    GPIO12 - RELAY
    GPIO13 - LED1
    GPIO03 - RX PIN
    GPIO01 - TX PIN
    GPIO14

Sonoff r2 (ESP8285 SoC with 1MB flash)
    GPIO00 - BUTTON
    GPIO12 - RELAY
    GPIO13 - LED1
    GPIO03 - RX PIN
    GPIO01 - TX PIN
    GPIO02 no pullup, labeled as IO2

Sonoff r3 (ESP8285)
    GPIO00 - BUTTON
    GPIO12 - RELAY
    GPIO13 - LED1
    GPIO03 - RX PIN
    GPIO01 - TX PIN
    GPIO09
    GPIO10
    GPIO16  
*/

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
#define ON_PAYLOAD              "ON"
#define OFF_PAYLOAD             "OFF"
//----------------------------------------------------------------------------
#define ProductKey              "2aea9e14-07a7-4d73-8d83-1dc42cee5622"
#define Version                 "26.0.0.0"
#define MakeFirmwareInfo(k, v)  "&_FirmwareInfo&k=" k "&v=" v "&FirmwareInfo_&"
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
WifiSetup MqttSubTopic       = {"MqttSubTopic", ""};
WifiSetup MqttPubTopic       = {"MqttPubTopic", ""};
WifiSetup MqttServer         = {"MqttServer", ""};
WifiSetup MqttUser           = {"MqttUser", ""};
WifiSetup MqttPassword       = {"MqttPassword", ""};

WifiSetup* WifiSettings[NUM_WIFI_SETTINGS] = {
  &Hostname, 
  &Ssid, 
  &Password, 
  &MqttSubTopic, 
  &MqttPubTopic,
  &MqttServer, 
  &MqttUser, 
  &MqttPassword};
