#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ReadInput.h>
#include <DigiOut.h>
#include <StoreStrings.h>
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>
#include "GlobalVar.h"
#include "HomePage.h"

WiFiClient espClient;
PubSubClient client (espClient);
ESP8266WebServer server(80);
MDNSResponder mdns;
ReadInput Button(SONOFF_BUTTON);
ReadInput Switch(SONOFF_SWITCH);
DigiOut Led(SONOFF_LED, OFF_LED);
DigiOut Rele(SONOFF_RELAY, OFF_RELAY);
StoreStrings mem(EEPROM_SIZE);
DynamicJsonDocument message(JSON_MSG_LENGTH);

void setup() {
  Serial.begin(115200);
  delay(500);

  Rele.Begin();
  Led.Begin();

  switchState = Switch.State();
  switchStatePre = switchState;
  pushButton = !Button.State();
  pushButtonPre = pushButton;

  Serial.println("\n----------------------");
  Serial.println("Application ver. " + String(Version) + " running!");
  Connection_Manager();
}

void loop() {
  if (deviceConnected) {
    if (client.connected()) {
      client.loop();
    } else {
      if (millis() - lastMqttCheckConn > MQTT_CONNECTION) {
        Serial.println("Mqtt client not connected");
        lastMqttCheckConn = millis();
        // Attempt to reconnect
        if (client.connect(Hostname.Val.c_str(), MqttUser.Val.c_str(), MqttPassword.Val.c_str())) {
          lastMqttCheckConn = 0;
          client.subscribe(MqttSubTopic.Val.c_str());
          Serial.println("Mqtt client reconnected");
        }
      }
    }
  }
  server.handleClient();

  // Controllo connessione ed aggiornamenti ota
  if (millis() - lastTimeCheckConn > TIMER_CONNECTION) {
    // Blink di check connessione
    Serial.print("Connection check... ");
    Led.Off();
    delay(PLACING_TIME);
    lastTimeCheckConn = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Connection_Manager();
    } else {
      Serial.println("OK");
      OtaUpdate();
      // Accendo led di conferma connessione alla rete
      Led.On();
    }
  }

  // Controllo presione pulsante multifunzione
  pushButton = !Button.State();

  if (pushButton && !pushButtonPre) {
    Led.Off();
    delay(PLACING_TIME);
    pushButtonTime = millis();
  }
  if (pushButtonPre && !pushButton) {
    PushButtonFunction(pushButtonCount);
    pushButtonCount = 0;
    if (WiFi.status() == WL_CONNECTED) {
      Led.On();
    }
  }
  pushButtonPre = pushButton;

  if (pushButton) {
    if (millis() - pushButtonTime > (BLINK_TIME - TIME_FLASH_BLINK)) {
      pushButtonTime = millis();
      Led.Blink(PLACING_TIME, 1, TIME_FLASH_BLINK);
      pushButtonCount ++;
    }
  }

  // Controllo stato switch
  switchState = Switch.State();

  if (switchStatePre == false && switchState == true) {
    Switch_Toggle();
  }
  if (switchStatePre == true && switchState == false) {
    Switch_Toggle();
  }
  switchStatePre = switchState;
}

void PushButtonFunction(int func) {
  Serial.print("Function: " + String(func) + " -> ");
  switch (func) {
    case 1:
      Serial.println("Toggle");
      delay(500);
      Switch_Toggle();
      break;
    case 2:
      Serial.println("Connection check");
      delay(500);
      Connection_Manager();
      break;
    case 3:
      Serial.println("Check FW update");
      OtaUpdate();
      break;
    case 4:
      Serial.println("Show ip address");
      ShowIpAddr();
      break;
    case 5:
      Serial.println("Load settings");
      LoadSettingsFromEeprom();
      break;
    case 6:
      Serial.println("Save settings");
      SaveSettingsInEeprom();
      break;
    case 7:
      Serial.println("EEPROM clean");
      for (int i = 0; i < EEPROM_SIZE; i++) {
        mem.write(i, String('\0'));
      }
      Restart();
      break;
    case 8:
      Serial.println("Wifi signal power");
      getWifiPower(Ssid.Val);
      break;
    case 9:
      Serial.println("Sonoff restart");
      Restart();
      break;
    default:
      Serial.println("No function!");
      Led.Blink(PLACING_TIME, 5, TIME_FLASH_BLINK);
      break;
  }
}

void Restart() {
  Serial.println("Restart");
  delay(500);
  Led.Blink(PLACING_TIME, 5, TIME_FLASH_BLINK);
  ESP.restart();
}

void Switch_On() {
  Rele.On();

  if (deviceConnected) {
    JsonObject msg = message.to<JsonObject>();    
    char msg_out[JSON_MSG_LENGTH];
    msg["state"] = ON_PAYLOAD;
    serializeJson(msg, msg_out);
    client.publish(MqttPubTopic.Val.c_str(), msg_out);
    Serial.print("Message published: ");
    Serial.println(msg_out);
  }
  Serial.println("Switched ON");
}

void Switch_Off() {
  Rele.Off();

  if (deviceConnected) {JsonObject msg = message.to<JsonObject>();    
    char msg_out[JSON_MSG_LENGTH];
    msg["state"] = OFF_PAYLOAD;
    serializeJson(msg, msg_out);
    client.publish(MqttPubTopic.Val.c_str(), msg_out);
    Serial.print("Message published: ");
    Serial.println(msg_out);
  }
  Serial.println("Switched OFF");
}

void Switch_Toggle() {
  if (Rele.State() != OFF_RELAY) {
    Switch_Off();
  } else {
    Switch_On();
  }
}

void Callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Payload recived: ");
  for (byte i = 0; i < length; i++) {
    Serial.print(char(payload[i]));
  }
  Serial.println("");

  if ((String(topic) == MqttSubTopic.Val) && (length > 5)) {
    const char* stat;
    String state;
    Serial.println("Command/s on payload: ");
    deserializeJson(message, payload);
    JsonObject root = message.as<JsonObject>();

    for (JsonPair kv : root) {
      String key = kv.key().c_str();

      if (key.equals("state")) {
        stat = message["state"];
        state = String(stat);
        Serial.println("  - state = " + state);

      } else {
        Serial.println("  - Key unknown recived = " + key + kv.value().as<char*>());
      }
    }

    if (state.equals(OFF_PAYLOAD)) {
      Switch_Off();
    }
    if (state.equals(ON_PAYLOAD)) {
      Switch_On();
    }
  }
}

void ShowIpAddr() {
  if (deviceConnected) {
    String ip = WiFi.localIP().toString();
    byte numIterations = 0;
    String ipNumStr = "0";
    byte ipNum = 0;

    Serial.println(ip);
    for (int i = 0; i < ip.length(); i++) {
      if (ip.charAt(i) == '.') {
        numIterations++;
      }
      if (numIterations == 3) {
        ipNumStr = ip.substring(i + 1, ip.length());
        ipNum = ipNumStr.toInt();
        break;
      }
    }

    Led.Blink(PLACING_TIME, ipNum, PLACING_TIME);

  } else {

    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(PLACING_TIME, 10, TIME_FLASH_BLINK);
  }
}

bool getWifiPower(String netName) {
  int netsNumber = WiFi.scanNetworks();
  int minRawLevel = -75;
  int maxRawLevel = -45;
  int minLevel = 0;
  int maxLevel = 5;
  bool result = false;

  if (netsNumber > 0) {
    for (int i = 0; i < netsNumber; ++i) {
      if (String(WiFi.SSID(i)) == netName) {
        result = true;
        int rawPower = WiFi.RSSI(i);
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(rawPower);
        Serial.print(")");
        if (minRawLevel <= rawPower && rawPower <= maxRawLevel) {
          int power = map(rawPower, minRawLevel, maxRawLevel, minLevel, maxLevel);
          switch (power) {
            case 0:
              Serial.println(" Unreacheable");
              break;
            case 1:
              Serial.println(" Very Low");
              break;
            case 2:
              Serial.println(" Low");
              break;
            case 3:
              Serial.println(" Good");
              break;
            case 4:
              Serial.println(" High");
              break;
            case 5:
              Serial.println(" Very High!");
              break;
          }
          Led.Blink(PLACING_TIME, power, PLACING_TIME);
        } else {
          Serial.println(" Signal level out of bounds");
        }
      }
    }
  }
  return result;
}

void Connection_Manager() {
  Serial.println("Connection process started");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(PLACING_TIME);
  LoadSettingsFromEeprom();

  WiFi.mode(WIFI_AP_STA);

  if (Ssid.Val != "" && !Ssid.Val.startsWith(" ")) {
    if (getWifiPower(Ssid.Val)) {
      WiFi.begin(Ssid.Val, Password.Val);
      Serial.println("Connecting to: " + String(Ssid.Val));
      byte numCehck = 0;
      while ((WiFi.status() != WL_CONNECTED)) {
        if (numCehck >= 150) {
          lastTimeCheckConn = millis();
          break;
        }
        numCehck++;
        delay(200);
      }
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    if (mac.equals("")) {
      String macTemp = String(WiFi.macAddress());
      for (byte i=0; i<macTemp.length(); i++){
        if (macTemp.charAt(i) != ':'){
          mac += macTemp.charAt(i);
        }
      }
    }
    Serial.println("IP: " + WiFi.localIP().toString() + " Mac: " + String(WiFi.macAddress()));
    client.setServer(MqttServer.Val.c_str(), 1883);
    client.setCallback(Callback);
    client.connect(Hostname.Val.c_str(), MqttUser.Val.c_str(), MqttPassword.Val.c_str());
    client.subscribe(MqttSubTopic.Val.c_str());

    // Accendo led per indicare l'avvenuta connessione
    Led.On();
    deviceConnected = true;

  } else {
    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(PLACING_TIME, 10, TIME_FLASH_BLINK);
    // Lascio spento il led per indicare l'assenza di connessione
    Led.Off();
    deviceConnected = false;
  }

  // Configurazione AP e webServer
  if (Hostname.Val == "" || Hostname.Val.startsWith(" ") || Hostname.Val.length() >= MAX_LENGTH_SETTING) {
    Hostname.Val = DefaultApName;
  }
  Serial.println("AP Name: " + Hostname.Val);

  WiFi.softAP(Hostname.Val.c_str());
  mdns.begin(Hostname.Val, WiFi.localIP());
  server.on("/", []() {
    server.send(200, "text/html", webPage);
  });
  server.on("/on", []() {
    server.sendHeader("Location", "/");
    server.send(303);
    Switch_On();
  });
  server.on("/off", []() {
    server.sendHeader("Location", "/");
    server.send(303);
    Switch_Off();
  });
  server.on("/restart", []() {
    server.sendHeader("Location", "/");
    server.send(303);
    Restart();
  });
  server.on("/settings", HTTP_POST, ChangeSettings);
  server.onNotFound(handleNotFound);
  server.begin();

  // Azzero ultimo tempo di connessione
  lastTimeCheckConn = millis();
}

void handleNotFound() {
  String message = "Err 404\nPage Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void ChangeSettings() {
  int numIteration = 0;
  int startPoint = -1;
  int parNum = 0;
  String valRecived;
  String data = server.arg("plain");

  Serial.println("Parameters recived:");
  Serial.println(data);
  Led.Blink(PLACING_TIME, 3, PLACING_TIME);
  for (int i = 0; i < data.length(); i++) {
    if (data.charAt(i) == '"') {
      if (startPoint == -1) {
        startPoint = i;
      } else {
        valRecived = data.substring(startPoint + 1, i);
        startPoint = -1;
        numIteration++;
        if (numIteration % 2 == 0) {
          Serial.println(valRecived);
          if (valRecived != " " && valRecived != "" && !valRecived.startsWith(" ")) {
            WifiSettings[parNum]->Val = valRecived;
          }
          parNum++;
        } else {
          Serial.print(valRecived + ": ");
        }
      }
    }
  }
  SaveSettingsInEeprom();
  Connection_Manager();
}

void SaveSettingsInEeprom() {
  Serial.println("Saving settings in EEPROM");
  mem.resetWriteCounter();
  for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
    mem.write(mem.getLastWrittenByte(), WifiSettings[i]->Val);
  }
  Serial.println("Settings saved!");
}

void LoadSettingsFromEeprom() {
  Serial.println("Loading settings from EEPROM");
  mem.resetReadCounter();
  for (byte i = 0; i < NUM_WIFI_SETTINGS; i++) {
    WifiSettings[i]->Val = mem.read(mem.getLastReadedByte(), MAX_LENGTH_SETTING);
    Serial.println( WifiSettings[i]->Name + ": " +  WifiSettings[i]->Val);
  }
  Serial.println("Settings loaded!");
}

void OtaUpdate() {
  String url = "http://otadrive.com/DeviceApi/GetEsp8266Update?";
  url += "&s=" + mac;
  url += MakeFirmwareInfo(ProductKey, Version);

  Serial.println("Get firmware from url:");
  Serial.println(url);

  t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, url, Version);
  switch (ret)
  {
  case HTTP_UPDATE_FAILED:
    Serial.printf("Update Faild, Error: (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    break;
  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("No new update available");
    break;
  case HTTP_UPDATE_OK:
    Serial.println("Update OK");
    break;
  default:
    Serial.println(ret);
    break;
  }
}
