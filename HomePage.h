const char webPage[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html lang="en">
  
    <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=yes">
      <title>Home Page</title>
      <style>
        body {
          background-color: #b7b7ee;
        }
        div {
          padding: 10px;
        }
        input {
          border-radius: 5px;
          width: 100%;
          height: 40px;
          box-sizing: border-box;
          text-align: center;
        }
        select {
          border-radius: 5px;
          padding: 10px;
          width: 100%;
          text-align: center;
          text-align-last:center;
        }
        body {
          text-align: center;
          font-family: verdana;
        }
        button {
          border-radius: 5px;
          width: 100%;
          height: 40px;
          color:black;
        }
        .container {
          border-radius: 5px;
          border: 2px solid #000000;
          width: 90%;
          max-width: 450px;
          background-color: rgb(62, 131, 131);
          margin: auto;
        }
        .actions-container {
          width: 90%;
          max-width: 350px;
          margin: auto;
        }
        .settings-container {
          width: 90%;
          max-width: 350px;
          margin: auto;
        }
      </style>
    </head>
    <body>
      <div class="container">
        <div class="actions-container">
          <h1>Home Page</h1>
          <hr>
          <p>
            <a href="/on"><button style="background-color: rgb(14, 121, 14);">ON</button></a>
          </p>
          <p>
            <a href="/off"><button style="background-color: rgb(209, 16, 16);">OFF</button></a>
          </p>
          <hr>
        </div>
        <div class="settings-container">
          <h2>Settings</h2>
          <p>
            <label for="Hostname">Hostname</label>
          </p>
          <p>
            <input value="" id="Hostname" placeholder="DeviceName" maxlength="20">
          </p>
          <p>  
            <label for="Ssid">Ssid</label>
          </p>
          <p>
            <input value="" id="Ssid" placeholder="Wifi Net Name" maxlength="20">
          </p>
          <p>
            <label for="Password">Password</label>
          </p>
          <p>
            <input type="password" value="" id="Password" placeholder="Wifi Password" maxlength="20">
          </p>
          <p>  
            <label for="MQTT_Sub">MQTT Subscribe topic</label>
          </p>
          <p>
            <input value="" id="MQTT_Sub" placeholder="cmnd/MyDevice" maxlength="20">
          </p>
          <p>  
            <label for="MQTT_Pub">MQTT Publish topic</label>
          </p>
          <p>
            <input value="" id="MQTT_Pub" placeholder="stat/MyDevice" maxlength="20">
          </p>
          <p>  
            <label for="MQTT_Server">MQTT Server</label>
          </p>
          <p>
            <input value="" id="MQTT_Server" placeholder="192.168.1.100" maxlength="20">
          </p>
          <p>
            <label for="MQTT_User">MQTT User</label>
          </p>
          <p>
            <input value="" id="MQTT_User" placeholder="MQTT User" maxlength="20">
          </p>
          <p>  
            <label for="MQTT_Password">MQTT Password</label>
          </p>
          <p>
            <input type="password" value="" id="MQTT_Password" placeholder="MQTT Password" maxlength="20">
          </p>
          <p>  
            <label for="OTA_DRIVE_KEY">Ota Drive Product Key</label>
          </p>
          <p>
            <input value="" id="OTA_DRIVE_KEY" placeholder="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" maxlength="36">
          </p>
          <br>
          <p>
            <button type="button" id="savebtn" style="background-color: cornflowerblue;" onclick="save()">Save Settings</button>
          </p>
          <hr>
          <p>
            <a href="/restart"><button style="background-color: rgb(216, 196, 20);">Restart Device</button></a>
          </p>
          <hr>
          <p>
            <button style="background-color: rgb(189, 0, 0);" onclick="eepromConfirm()">Clean EEPROM</button>
          </p>
          <hr>
        </div>
      </div>
    </body>
  
    <script>      
      function save() {
        var Hostname = document.getElementById("Hostname").value;
        var Ssid = document.getElementById("Ssid").value;
        var Password = document.getElementById("Password").value;
        var MQTT_Sub = document.getElementById("MQTT_Sub").value;
        var MQTT_Pub = document.getElementById("MQTT_Pub").value;
        var MQTT_Server = document.getElementById("MQTT_Server").value;
        var MQTT_User = document.getElementById("MQTT_User").value;
        var MQTT_Password = document.getElementById("MQTT_Password").value;
        var OTA_DRIVE_KEY = document.getElementById("OTA_DRIVE_KEY").value;
  
        var data = {Hostname:Hostname,
                    Ssid:Ssid,
                    Password:Password,
                    MQTT_Sub:MQTT_Sub,
                    MQTT_Pub:MQTT_Pub,
                    MQTT_Server:MQTT_Server,
                    MQTT_User:MQTT_User,
                    MQTT_Password:MQTT_Password,
                    OTA_DRIVE_KEY:OTA_DRIVE_KEY};
  
        var xhr = new XMLHttpRequest();
        var url = "/settings";
    
        xhr.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
            if (xhr.responseText != null) {
              console.log(xhr.responseText);
            }
          }
        };
        xhr.open("POST", url, true);
        xhr.send(JSON.stringify(data));
      };

      function eepromConfirm() {
        if (confirm("Are you sure to clean all EEPROM?")) {
          window.location.href = '/clean';
        }
      }
    </script>
  
  </html>
)=====";
