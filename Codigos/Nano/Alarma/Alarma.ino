#include <LiquidCrystal_I2C.h>
#include<Wire.h>
#define SENSE A1

unsigned OjoTp=0;
int contAlertas=0;

LiquidCrystal_I2C lcd(0x27,16,2); //0x20 o 0x27
//SensorUltraSonico
const int TriggerPin=4;
const int EchoPin=5;
long Duracion=0;

//Parpadeo
unsigned delyInicio=0;
boolean OjoCerrado=false;
boolean Sonico=false;
boolean Temporizador=false;

void setup() {
  //LCD 
  lcd.init();
  lcd.backlight();          // Activar luz de fondo 
  lcd.clear();  
  Serial.begin(9600);
  
  //Sensor Parpadeo
  pinMode(SENSE, INPUT);
  pinMode(8, OUTPUT);
  // put your setup code here, to run once:
  
  //Sensor Ultrasonico
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}



void loop() {

    //Sensor de Ultrasonico
  digitalWrite(TriggerPin,HIGH);
  delay(2);
  digitalWrite(TriggerPin,LOW);
  Duracion=pulseIn(EchoPin,HIGH);
  long Distancia_cm=Duracion / 58.2;
  Serial.println(Distancia_cm);
  if(Distancia_cm<10){
    Sonico=true;
  }
  
  //Parpadeo
  if(!digitalRead(SENSE)){
    if(OjoCerrado==false){
      OjoCerrado=true;
      OjoTp=millis();
    }
    if((millis()-OjoTp)>3000 && OjoCerrado==true && Temporizador==false){
      Temporizador=true;
      Sonico=false;//Para que lo apague el Sensor Sonico
      digitalWrite(8, HIGH);
      contAlertas+=1;
    }
  }
  else if(Sonico==true){
    OjoCerrado=false; //Ya habrio el Ojo
    Temporizador=false;
    digitalWrite(8, LOW);
  }
  
 

  //LCD
  //Serial.println(pulso);
  lcd.setCursor(0,0);
  lcd.print("Pulso|Sonic|Ale ");
  lcd.setCursor(0,1);
  lcd.print(0);
  lcd.setCursor(6,1);
  lcd.print(Distancia_cm);
  lcd.setCursor(4,1);
  lcd.print("F");
  lcd.setCursor(12,1);
  lcd.print(contAlertas);
}
