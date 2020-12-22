#include <LiquidCrystal_I2C.h>
#include<SoftwareSerial.h>
#include<Wire.h>
#define SENSE A1


//-------------------Inicializamos las variables para el funcionamiento de los sensores ----------------------------------


//Tacometro
int encoderR=2;
volatile unsigned UltimaInterrupcion=0;
volatile unsigned boleanoTacometro=0;

//SensorUltraSonico
const int TriggerPin=4;
const int EchoPin=5;
long Duracion=0;


//Parpadeo y Alerta leve y Grave
float OjoTp=0;
int contAlertas=0;
boolean OjoCerrado=false;
boolean Sonico=false;
boolean Temporizador=false;


//Pulso 
int pulso=0;
//Ritmo Cardiaco
boolean ritmo=false;
int PPM=0;
float factor = 0.75;    // coeficiente para filtro pasa bajos
float maximo = 0.0;   // para almacenar valor maximo 
int minimoEntreLatidos = 300; // 300 mseg. de tiempo minimo entre latidos
float valorAnterior = 500;  // para almacenar valor previo
int latidos = 0;    // contador de cantidad de latidos

//LCD
boolean alertaLCD=false;
LiquidCrystal_I2C lcd(0x27,16,2); //0x20 o 0x27

//Comunicacion con el ESP8266
SoftwareSerial Esp(10,11);



void setup() {

  //Tacometro
  attachInterrupt(digitalPinToInterrupt(encoderR),REncoder,CHANGE);

  //Sensor Ultrasonico
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  //Sensor Parpadeo
  pinMode(SENSE, INPUT);
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);

  //Sensor Ritmo Cardiaco
  pinMode(A0,INPUT);
  
  //LCD 
  lcd.init();
  lcd.backlight();          // Activar luz de fondo 
  lcd.clear();              // Borrar LCD

  //Comunicacion con ESP
  Esp.begin(9600);
  //Serial Propio del Nano
  Serial.begin(9600);

}

void loop() {

  //Parpadeo  y alerta leve y grave
  if(!digitalRead(SENSE) ){ //!digitalRead(SENSE) el conductor cerro el ojo
    if(OjoCerrado==false && alertaLCD==false){
      //Cuando cerro el ojo, marcamos que lo hizo con el booleano y 
      //el momento que lo hizo lo guardamos para contar cuanto lleva cerrado con OjoTp
      OjoCerrado=true;
      OjoTp=millis();
      //Serial.println("Cerro El ojo");
    }
    if(OjoTp!=0 &&(millis()-OjoTp)>4000 && OjoCerrado==true && alertaLCD==false){
      //Cuando ya pasaron los 3 segundos desde que cerro el ojo y no habia una alarma ya cursando
      //Se activa una alarma leve o grave dependiendo del contador de Alertas
      //Serial.println("Paso el Tiempo, inicia una alerta");
      //Iniciamos una alerta
      alertaLCD=true;
      digitalWrite(8, HIGH);//Comienza a sonar la alarma 
      digitalWrite(12, HIGH);// Se prende el led
      contAlertas+=1;
      
      if(contAlertas <= 3){// Alerta Leve
        Sonico=false;//Para que lo apague el Sensor Sonico
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Alerta Simple");
        lcd.setCursor(1,1);
        lcd.print("Favor Despertar");
        
      }else{
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("Alerta Grave");
        lcd.setCursor(1,1);
        lcd.print("Detener Vehiculo");
      }
      
      
    }
  }
  else if(OjoCerrado==true && alertaLCD==false){
    //Si el conductor abrio el ojo antes de ativar la alarma, reiniciamos el temporizador
    OjoCerrado=false;
    OjoTp=0;
    //Serial.println("abrio el ojo sin alerta y se reinicia el OjoTP");

  }
  else if(Sonico==true && alertaLCD==true && contAlertas<=3){
    //Apagar la alarma leve con el sensor ultrasonico
    OjoCerrado=false; //Ya habrio el Ojo
    OjoTp=0;
    //Serial.println("Se apago alarma leve, se reinicia TP");
    digitalWrite(8, LOW);
    digitalWrite(12, LOW);
    //Quitamos el Mensaje De Alerta
    alertaLCD=false;
    lcd.clear();
  }
  else if(contAlertas>3 &&alertaLCD==true && boleanoTacometro==1  ){
    // Si ya han pasado 3 alertas leves y esta ocurriendo el una alerta grave alertaLCD==true, se debe detener el vehiculo boleanoTacometro==1
    //Apagamos la alarma
    digitalWrite(8, LOW);
    digitalWrite(12, LOW);
    

    //Mostramos que se notificara a las autoridades,
    //Creando este if para que solo ocurra una vez, para poder limpiar la pantalla
    if(contAlertas==4){
      //Serial.println("Se apago alarma grave, y notifica");
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Notificando");
      lcd.setCursor(2,1);
      lcd.print("autoridades");
    }
    contAlertas=5;
    
  }
  


  //Tacometro
  if((millis()-UltimaInterrupcion)>5000 && UltimaInterrupcion!=0){
    boleanoTacometro=1;//igual T, Detenido
  }else{
    //Al hacer UltimaInterrupcion!=0 hacemos que el vehiculo parta en movimiento, para la demostracion de la alerta grave
    boleanoTacometro=0;//Muesta F, no detenido
  }


  
  //Sensor de Ultrasonico
  digitalWrite(TriggerPin,HIGH);
  digitalWrite(TriggerPin,LOW);
  Duracion=pulseIn(EchoPin,HIGH);
  long Distancia_cm=Duracion / 58.2;
  if(Distancia_cm<9){
    Sonico=true;
  }else{
    Distancia_cm=9;
  }
  

  //Pulso
  pulso=analogRead(A0);
  //Ritmo Cardiaco
  static unsigned long tiempoLPM = millis();  // tiempo Latidos Por Minuto con valor actual devuelto por millis()
  static unsigned long entreLatidos = millis(); // tiempo entre Latidos con valor actual devuelto por millis()
  float valorFiltrado = factor * valorAnterior + (1 - factor) * pulso; // filtro pasa bajos
  float cambio = valorFiltrado - valorAnterior;   // diferencia entre valor filtrado y valor anterior
  valorAnterior = valorFiltrado;    // actualiza valor anterior con valor filtrado

  if ((cambio >= maximo) && (millis() > entreLatidos + minimoEntreLatidos)) { 
    // si cambio es es mayor o igual a maximo y pasaron al menos 300 mseg.
    maximo = cambio;      // actualiza maximo con valor de cambo
    entreLatidos = millis();    // actualiza variable entreLatidos con millis()
    latidos++;        // incrementa latidos en uno
  }
  
  maximo = maximo * 0.97;   // carga maximo como el 97 por ciento de su propio valor para dejar decaer y no perder pulsos

  if (millis() >= tiempoLPM + 15000) {    // si transcurrieron al menos 15 segundos
    Serial.print("Latidos por minuto: "); // muestra texto
    PPM=latidos * 5;
    Serial.println(PPM);    // muestra variable latidos multiplicado por 4
    latidos = 0;        // coloca contador de latidos en cero
    tiempoLPM = millis();     // actualiza variable con valor de millis()
    ritmo=true;
  }
  
  //LCD
  if(alertaLCD==false){
    if(ritmo==true){
      lcd.clear();
      ritmo=false;
    }
    lcd.setCursor(0,0);
    lcd.print("PPM  |Sonic|Ale ");
    lcd.setCursor(0,1);
    lcd.print(PPM);
    lcd.setCursor(6,1);
    lcd.print(Distancia_cm);
    lcd.setCursor(4,1);
    if(boleanoTacometro==1){
      lcd.print("T");//T, detenido
    }else{
      lcd.print("F");//F, movimiento
    }
    lcd.setCursor(12,1);
    lcd.print(contAlertas);
  }
  
  //Comunicacion Esp
  Esp.print(pulso);
  Esp.print("\n");
  delay(35);

}

void REncoder(){
  //Ocurre cuando el tacometro siente una interrupcion
  UltimaInterrupcion=millis();
  Serial.print(" Interrupcion");
}
