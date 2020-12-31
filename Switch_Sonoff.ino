#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ReadInput.h>
#include <DigiOut.h>
#include <StoreStrings.h>
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
  Serial.println("Application running!");
  Connection_Manager();
}

void loop() {
  if (deviceConnected) {
    client.loop();
  }
  server.handleClient();

  // Controllo connessione
  if (millis() - lastTimeCheckConn > TIMER_CONNECTION) {
    // Blink di check connessione
    Serial.print("Connection check... ");
    Led.Off();
    delay(PLACING_TIME);
    lastTimeCheckConn = millis();
    if ((WiFi.status() != WL_CONNECTED) || !client.connected()) {
      Connection_Manager();
    } else {
      Serial.println("OK");
      // Accendo led di conferma connessione alla rete
      delay(PLACING_TIME);
      Led.On();
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
      Restart();
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
      for(int i=0; i<EEPROM_SIZE; i++){
        mem.write(i, String('\0'));
      }
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
    client.publish(Set_Light_Pub.Val.c_str(), "ON");
  }
  Serial.println("Switched ON");
}

void Switch_Off() {
  Rele.Off();
  if (deviceConnected) {
    client.publish(Set_Light_Pub.Val.c_str(), "OFF");
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
  String setPower;
  char message_buff[10];
  int i = 0;
  
  if (String(topic) == Set_Light_Sub.Val) {
    for (i = 0; i < length; i++) {
      message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    setPower = String(message_buff);
    Serial.println("Mqtt command: setPower = " + String(setPower));
    if (setPower == "OFF") {
      Switch_Off();
    }
    if (setPower == "ON") {
      Switch_On();
    }
  }
}

void ShowIpAddr(){
  if (deviceConnected){
    String ip = WiFi.localIP().toString();
    byte numIterations = 0;
    String ipNumStr = "0";
    byte ipNum = 0;
    
    Serial.println(ip);
    for (int i=0; i<ip.length(); i++){
      if (ip.charAt(i) == '.'){
        numIterations++;
      }
      if (numIterations == 3){
        ipNumStr = ip.substring(i+1, ip.length());
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

void Connection_Manager() {
  Serial.println("Connection process started");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(PLACING_TIME);
  LoadSettingsFromEeprom();
  if (Hostname.Val == "" || Hostname.Val.startsWith(" ") || Hostname.Val.length() >= MAX_LENGTH_SETTING){
    Hostname.Val = DefaultApName;
  }
  Serial.println("AP Name: " + Hostname.Val);
  // Lampeggio inizio connessione
  Led.Blink(PLACING_TIME, 3, PLACING_TIME);
  WiFi.softAP(Hostname.Val.c_str());
  WiFi.mode(WIFI_AP_STA);
  mdns.begin(Hostname.Val, WiFi.localIP());
  server.on("/", []() {
    server.send(200, "text/html", webPage);
  });
  server.on("/on", []() {
    server.send(200, "text/html", webPage);
    Switch_On();
  });
  server.on("/off", []() {
    server.send(200, "text/html", webPage);
    Switch_Off();
  });
  server.on("/restart", []() {
    server.send(200, "text/html", webPage);
    Restart();
  });
  server.on("/settings", HTTP_POST, changeSettings);
  server.begin();
  
  if (Ssid.Val != "" && !Ssid.Val.startsWith(" ")){
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
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    // Lampeggio connessione riuscita
    Led.Blink(PLACING_TIME, 3, PLACING_TIME);
    client.setServer(MqttServer.Val.c_str(), 1883);
    client.setCallback(Callback);
    client.connect(Hostname.Val.c_str(), MqttUser.Val.c_str(), MqttPassword.Val.c_str());
    client.subscribe(Set_Light_Sub.Val.c_str());
    deviceConnected = true;

    // Accendo led per indicare l'avvenuta connessione
    Led.On();
    
  } else {
    Serial.println("Not connected!");
    // Lampeggio connessione fallita
    Led.Blink(PLACING_TIME, 10, TIME_FLASH_BLINK);
    deviceConnected = false;
    // Lascio spento il led per indicare l'assenza di connessione
    Led.Off();
  }
  // Azzero ultimo tempo di connessione
  lastTimeCheckConn = millis();
}

void changeSettings() {
  int numIteration = 0;
  int startPoint = -1;
  int parNum = 0;
  String valRecived;
  String data = server.arg("plain");

  Serial.println("Parameters recived:");
  Led.Blink(PLACING_TIME, 3, PLACING_TIME);
  for (int i=0; i<data.length(); i++){
    if (data.charAt(i) == '"'){
      if(startPoint == -1){
        startPoint = i;
      } else {
        valRecived = data.substring(startPoint + 1, i);
        startPoint = -1;
        numIteration++;
        if (numIteration % 2 == 0){
          Serial.println(valRecived);
          if (valRecived != "" && !valRecived.startsWith(" ")){
            WifiSettings[parNum]->Val = valRecived;
            parNum++;
          }
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
  for(byte i=0; i<NUM_WIFI_SETTINGS; i++){
    mem.write(mem.getLastWrittenByte(), WifiSettings[i]->Val);
  }
  Serial.println("Settings saved!");
}

void LoadSettingsFromEeprom() {
  Serial.println("Loading settings from EEPROM");
  mem.resetReadCounter();
  for(byte i=0; i<NUM_WIFI_SETTINGS; i++){
    WifiSettings[i]->Val = mem.read(mem.getLastReadedByte(), MAX_LENGTH_SETTING);
    Serial.println( WifiSettings[i]->Name + ": " +  WifiSettings[i]->Val);
  }
  Serial.println("Settings loaded!");
}
