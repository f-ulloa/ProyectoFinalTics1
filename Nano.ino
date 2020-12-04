#include <LiquidCrystal_I2C.h>
#include<SoftwareSerial.h>
#include<Wire.h>
#define SENSE A1

SoftwareSerial Esp(10,11); 
//LCD
boolean alertaLCD=false;
LiquidCrystal_I2C lcd(0x27,16,2); //0x20 o 0x27

//SensorUltraSonico
const int TriggerPin=4;
const int EchoPin=5;
long Duracion=0;

//Tacometro
int encoderR=13;
volatile unsigned UltimaInterrupcion=0;
volatile unsigned boleanoTacometro=0;

//Parpadeo
unsigned OjoTp=0;
int contAlertas=0;
boolean OjoCerrado=false;
boolean Sonico=false;
boolean Temporizador=false;

  
int pulso=0;
void setup() {
  //Comunicacion con ESP
  Esp.begin(9600);
  
  //LCD 
  lcd.init();
  lcd.backlight();          // Activar luz de fondo 
  lcd.clear();              // Borrar LCD

  //Tacometro
  attachInterrupt(digitalPinToInterrupt(encoderR),REncoder,CHANGE);
  //Sensor Ultrasonico
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  
  //Sensor Ritmo Cardiaco
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(12, OUTPUT);

  //Sensor Parpadeo
  pinMode(SENSE, INPUT);
  pinMode(8, OUTPUT);
 

}

void loop() {

  //Parpadeo
  if(!digitalRead(SENSE)){
    if(OjoCerrado==false && Temporizador==false){
      OjoCerrado=true;
      OjoTp=millis();
    }
    if(OjoTp!=0 &&(millis()-OjoTp)>3000 && OjoCerrado==true && Temporizador==false){
      Temporizador=true;
      Sonico=false;//Para que lo apague el Sensor Sonico
      digitalWrite(8, HIGH);
      digitalWrite(12, HIGH);
      contAlertas+=1;
      //Mostrar Alerta Simple en LCD
      alertaLCD=true;
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Alerta Simple");
      lcd.setCursor(1,1);
      lcd.print("Favor Despertar");
      
    }
  }
  else if(Sonico==true && Temporizador==true){
    OjoCerrado=false; //Ya habrio el Ojo
    digitalWrite(8, LOW);
    digitalWrite(12, LOW);
    Temporizador=false;
    //Quitamos el Mensaje De Alerta
    alertaLCD=false;
    lcd.clear();
  }
  
  //Tacometro
  if((millis()-UltimaInterrupcion)>5000 && UltimaInterrupcion!=0){
    boleanoTacometro=1;
  }else{
    boleanoTacometro=0;
  }


  
  //Sensor de Ultrasonico
  digitalWrite(TriggerPin,HIGH);
  delay(2);
  digitalWrite(TriggerPin,LOW);
  Duracion=pulseIn(EchoPin,HIGH);
  long Distancia_cm=Duracion / 58.2;
  if(Distancia_cm<10){
    Sonico=true;
  }else{
    Distancia_cm=0;
  }
  

  //Pulso
  pulso=analogRead(A0);
  
  //LCD
  if(alertaLCD==false){
    lcd.setCursor(0,0);
    lcd.print("Pulso|Sonic|Ale ");
    lcd.setCursor(0,1);
    lcd.print(pulso);
    lcd.setCursor(6,1);
    lcd.print(Distancia_cm);
    lcd.setCursor(4,1);
    if(boleanoTacometro==1){
      lcd.print("T");
    }else{
      lcd.print("F");
    }
    lcd.setCursor(12,1);
    lcd.print(contAlertas);
  }
  
  //Comunicacion Esp
  Esp.print(pulso);
  Esp.print("\n");
  delay(300);

}

void REncoder(){
  UltimaInterrupcion=millis();
  Serial.print(" Interrupcion");
  //lcd.clear();
}
