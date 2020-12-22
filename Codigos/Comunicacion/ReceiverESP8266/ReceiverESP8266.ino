#include<SoftwareSerial.h>
#include<ESP8266WiFi.h>
//Started SoftwareSerial at RX and TX pin of Esp8266
int ContSensores=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  boolean wh=true;
  while(Serial.available()and wh==true){
    float val=Serial.parseInt();
    if(Serial.read()=='\n'){
      if(val==0 && ContSensores==0){
        ContSensores=1;
      }
      else if(ContSensores==1){
        Serial.print("S. Pulso :");
        Serial.print(val);
        Serial.print("\n");
        ContSensores+=1;
      }
      else if(ContSensores==2){
        Serial.print("S. Parpadeo :");
        Serial.print(val);
        Serial.print("\n");
        ContSensores+=1;
      }
      else if(ContSensores==3){
        Serial.print("S. Velocidad :");
        Serial.print(val);
        Serial.print("\n");
        ContSensores+=1;
      }
      else if(ContSensores==4){
        Serial.print("S. Sonido :");
        Serial.print(val);
        Serial.print("\n");
        ContSensores=0;
        Serial.println("Terminar");
        wh=false;
      }
    }  
  }
  delay(500);

  Serial.println("Seguimos Con el codigo");



  

}
