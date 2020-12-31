const char webPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>Home Page</title>
    <style>
      div {
        padding: 10px;
      }
  
      input {
        padding: 10px;
        width: 270px;
      }
  
      body {
        text-align: center;
        font-family: verdana;
      }
  
      button {
        padding: 5px;
        width: 300px;
      }

      .container {
        width: 350px;
        background-color: rgb(62, 131, 131);
        margin: auto;
      }
  
    </style>
  </head>

  <body>
		<div class="container">
      <h1>Home Page</h1>
      <hr>
      <p>
        <a href="/on"><button style="color:black; background-color: rgb(14, 121, 14);">ON</button></a>
      </p>
      <p>
        <a href="/off"><button style="color:black; background-color: rgb(209, 16, 16);">OFF</button></a>
      </p>
      <p>
        <a href="/restart"><button style="color:black; background-color: rgb(170, 20, 216);">Restart</button></a>
      </p>
      <hr>
      <h2> Settings</h2>
      <p>
        <label for="Hostname">Hostname</label>
      </p>
      <p>
        <input value="" id="Hostname" placeholder="Hostname">
      </p>
      <p>  
        <label for="Ssid">Ssid</label>
      </p>
      <p>
        <input value="" id="Ssid" placeholder="Ssid">
      </p>
      <p>
        <label for="Password">Password</label>
      </p>
      <p>
        <input type="password" value="" id="Password" placeholder="Password">
      </p>
      <p>  
        <label for="MQTT_Sub">MQTT Sub</label>
      </p>
      <p>
        <input value="" id="MQTT_Sub" placeholder="MQTT Sub">
      </p>
      <p>
        <label for="MQTT_Pub">MQTT Pub</label>
      </p>
      <p>
        <input value="" id="MQTT_Pub" placeholder="MQTT Pub">
      </p>
      <p>  
        <label for="MQTT_Server">MQTT Server</label>
      </p>
      <p>
        <input value="" id="MQTT_Server" placeholder="MQTT Server">
      </p>
      <p>
        <label for="MQTT_User">MQTT User</label>
      </p>
      <p>
        <input value="" id="MQTT_User" placeholder="MQTT User">
      </p>
      <p>  
        <label for="MQTT_Password">MQTT Password</label>
      </p>
      <p>
        <input type="password" value="" id="MQTT_Password" placeholder="MQTT Password">
      </p>
      <br>
      <p>
        <button type="button" id="savebtn" style="color:black; background-color: cornflowerblue;" onclick="save()">Save</button>
      </p>
      <hr>
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

      var data = {Hostname:Hostname, Ssid:Ssid, Password:Password, MQTT_Sub:MQTT_Sub, MQTT_Pub:MQTT_Pub, MQTT_Server:MQTT_Server, MQTT_User:MQTT_User, MQTT_Password:MQTT_Password};
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
  </script>

</html>
)=====";
