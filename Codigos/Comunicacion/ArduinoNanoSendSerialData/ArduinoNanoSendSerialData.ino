#include<SoftwareSerial.h>
//Started SoftwareSerial at RX and TX pin of Arduino Nano
SoftwareSerial Esp(10,11); 

int ContSensores=0;
void setup() {
  Serial.begin(9600);
  Esp.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //ContSensores==0 ->Sincronizar ,1-> Pulso,2->Parpadeo, 3->Velocidad
  //4->Sonido
  
  if(ContSensores==0){
    Esp.print("0");
    Esp.print("\n");
    ContSensores+=1;
  }
  if(ContSensores==1){
    Esp.print("1");
    Esp.print("\n");
    ContSensores+=1;
  }
  if(ContSensores==2){
    Esp.print("2");
    Esp.print("\n");
    ContSensores+=1;
  }
  
  if(ContSensores==3){
    Esp.print("3");
    Esp.print("\n");
    ContSensores+=1;
  }
  if(ContSensores==4){
    Esp.print("4");
    Esp.print("\n");
    ContSensores=0;
  }
  delay(500);
  


}
