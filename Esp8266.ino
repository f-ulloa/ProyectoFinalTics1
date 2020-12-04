#include<SoftwareSerial.h>
#include<ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>

//Pagina y web Socket Esp8266
void getData();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
Ticker timer;
int randomNum;
char * ssid = "PepiNemo";
char * password = "f6625438";
ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);


//-------------------------------------------Pagina Web----------------------------

char webpage[] PROGMEM = R"=====(
<html>
<!-------------- Adding a data chart using -->
<head>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js'></script>
</head>


<!---- Adding a slider for controlling date rate--------->
<hr/>
<div>
  <canvas id = "line-chart" width="800" height ="450"></canvas>
</div>

<body onload = "javascript:init()">
<!----------------------Adding a websocket to the client(webpage)------------------->
<script>
  
    var webSocket, dataPlot;
    var maxDataPoint = 20;
    function removeData(){
      dataPlot.data.labels.shift();
      dataPlot.data.datasets[0].data.shift();
    }
    function addData(label, data){
      if(dataPlot.data.labels.length> maxDataPoint)removeData(); 
      dataPlot.data.labels.push(label);
      dataPlot.data.datasets[0].data.push(data);
      dataPlot.update();
    }
    function init(){
      webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
      dataPlot=new Chart(document.getElementById("line-chart"),{
        type: 'line',
        data: {
          labels: [],
          datasets: [{
            data: [],
            label: "Ritmo Cardiaco",
            borderColor:"#3e95cd",
            fill: false
          }]
        }
      });
      webSocket.onmessage = function(event){
        var data = JSON.parse(event.data);
        var today=new Date();
        var t=today.getHours()+":"+today.getMinutes()+":"+today.getSeconds();
        addData(t, data.value);
        console.log(data);
    }
  }

</script>
</body>
</html>
)=====";

//--------------------------------------------------------------------------------

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if(type == WStype_TEXT){
    float dataRate = (float) atof((const char*) &payload[0]);
    timer.detach();
    timer.attach(dataRate,getData);
    }
}


void setup() {
  WiFi.begin(ssid, password);
  Serial.begin(9600);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address");
  Serial.print(WiFi.localIP());

  server.on("/",[](){
    server.send_P(200,"text/html",webpage);
  });

  server.begin();
  webSocket.begin();
  
  webSocket.onEvent(webSocketEvent);
  timer.attach(0.3,getData);
  
}

void loop() {
  webSocket.loop();
  server.handleClient();
  boolean wh=true;
 

  
}
void getData(){
  //Leer el dato del sensor
  if(Serial.available()){
    float val=Serial.parseInt();
    if(Serial.read()=='\n'){
      String json = "{\"value\":";
      String rn = (String) val;
      json +=rn;
      json += "}";
      webSocket.broadcastTXT(json.c_str(), json.length());  
    }
  }
  else{
    float val=300;
    String json = "{\"value\":";
    String rn = (String) val;
    json +=rn;
    json += "}";
    webSocket.broadcastTXT(json.c_str(), json.length());
  }
  //delay(300);
}

  
