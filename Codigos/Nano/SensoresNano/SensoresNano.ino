#include <LiquidCrystal_I2C.h>
#include<Wire.h>
//Parpadeo
#define SENSE A1

LiquidCrystal_I2C lcd(0x27,16,2); //0x20 o 0x27
//SensorUltraSonico
const int TriggerPin=4;
const int EchoPin=5;
long Duracion=0;

//Tacometro
int encoderR=2;
volatile unsigned UltimaInterrupcion=0;
volatile unsigned boleanoTacometro=0;

//Parpadeo
unsigned delyInicio=0;
  
int pulso=0;
void setup() {
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
  if(digitalRead(SENSE)){
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(8, LOW);
  }else {

    if(digitalRead(SENSE)){
      
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(8,LOW);
    }else{
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(8,HIGH);
    }
    delyInicio=0;
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
  

  
  pulso=analogRead(A0);
  if(pulso>=365){
    digitalWrite(12,HIGH);
    delay(30);
    digitalWrite(12,LOW);
  }
  delay(50);



  //LCD
  //Serial.println(pulso);
  lcd.setCursor(0,0);
  lcd.print("Pulso|Sonic|Ojo ");
  lcd.setCursor(0,1);
  lcd.print(pulso);
  lcd.setCursor(6,1);
  lcd.print(Distancia_cm);
  lcd.setCursor(4,1);
  if(boleanoTacometro==1){
    lcd.print("T");
    Serial.print(" T");
    
  }else{
    lcd.print("F");
    Serial.print(" F");
  }

}

void REncoder(){
  UltimaInterrupcion=millis();
  Serial.print(" Interrupcion");
  //lcd.clear();
}
