#include <tm1637.h>
#include <TroykaDHT.h>
#include <EEPROM.h>

//{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
//0~9,A,b,C,d,E,F,o,h,-,r

#define DHTPIN 4 // Pin sensor is connected to
#define CLK 7
#define DIO 8

#define CLK2 3
#define DIO2 2

#define CLOCK_DELAY 1
const byte ledPin = 13;

const int buttonPin = A5;
int buttonState = 0; 

const int buttonPin2 = A4;
int buttonState2 = 0; 

const int luz_max = 7;
const int buttonPin3 = A3;
int buttonState3 = 0;

TM1637 tm1637(CLK, DIO);
TM1637 tm1637b(CLK2, DIO2);
DHT dht(DHTPIN, DHT11);

int S = 0;
int H =  0;
int M = 0;
int luz = 7;
int cont = 0;
int cont2 = 0;
float cont3 = 0;
float UtlTime = 0;
int temp = 0;
int humidity = 0;
int oldtemp = 0;
int estabiliza = 0;

boolean hflag = false;
boolean mflag = false;
boolean bflag = false;
boolean double01bt = false;
boolean display1 = true; //mostra horas no primeiro display

void setup() {

  // inicializa o pino do botao como entrada:
  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(buttonPin2, INPUT_PULLUP);  
  pinMode(buttonPin3, INPUT_PULLUP);  
  
  dht.begin();
  
  //Serial.begin(9600);
  tm1637.init();
  tm1637.set(luz);
  tm1637b.init();
  tm1637b.set(luz);
  //tm1637b.point(POINT_ON);
  tm1637.point(POINT_OFF);

  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.display(0, 0x7f);
  tm1637.display(1, 0x7f);
  tm1637.display(2, 0x7f);
  tm1637.display(3, 0x7f);
  tm1637b.display(0, 0x7f);
  tm1637b.display(1, 0x7f);
  tm1637b.display(2, 0x7f);
  tm1637b.display(3, 0x7f);
  //delay(100);

  //Recupera ultima hora gravada.
  H = EEPROM.read(0);
  M = EEPROM.read(1);
  S = EEPROM.read(2);
  //display1 = EEPROM.read(3);
  
  // FIXA SOMENTE TEMPERATURA
  // ELIMINA A OPÇÃO DE HORAS
  //
  display1 = false; 
  //
  //
  
}


//-----------
void loop() {
//Só entra se passou 1 segundo
if (millis() - UtlTime >= 1000) { 
  
  //pisca o led do arduino
  digitalWrite(ledPin, !digitalRead(ledPin)); 
  
  UtlTime=millis();
  //Update time
  S++;
  if (S>59) { S = 0;  M++; UtlTime = millis() ;  }
  if (M>59) {	M = 0; 	H++;   }
  if (H>23) H = 0;

  //Armazena a hora na eeprom
  EEPROM.write(0, H);
  EEPROM.write(1, M);
  EEPROM.write(2, S);
  
  //modifica o primeiro mostrador: tira a hora e mostra a humidade, caso aperte botão 1 + botao 2
  if (double01bt) { display1 = !display1; EEPROM.write(3,display1); double01bt  = false;  }
  
  // atualiza display
  AtlzTMC();
  }

  cont3++;
  if (cont3=100000) {
    LeBotoes();
    if (mflag || hflag || bflag ) {
      AtlzVar();
      AtlzTMC(); 
    }
    cont3=0;
  }
} //end void loop
  

//--------------
void LeBotoes(){
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  if (buttonState == LOW )   hflag = true;
  if (buttonState2 == LOW )  mflag = true;
  if (buttonState3 == LOW )  bflag = true;
  if (buttonState == LOW && buttonState2 == LOW ) { hflag = false; mflag = false; double01bt = true; delay(400); }  
}


//-------------
void AtlzVar(){
    // verifica se o botao H esta pressionado.
  if (hflag)  { H++; if (H > 23) H = 0; delay(100); hflag=false; }
  
  // verifica se o botao H esta pressionado.
  if (mflag)  {  M++; S = 0; if (M > 59) M = 0 ; UtlTime = millis() ;  delay(100); mflag=false;  }
  
  // reduz luminosidade dos displays
  if (bflag) {
    luz = luz - 3;
    tm1637b.point(POINT_OFF);
    if (luz < 0) luz = luz_max;
    tm1637.set(luz);
    tm1637b.set(luz);
    tm1637b.display(0, 0x7f);
    tm1637b.display(1, 0x7f);
    tm1637b.display(2, 0x7f);
    tm1637b.display(3, luz);
    tm1637.display(0, 0x7f);
    tm1637.display(1, 0x7f);
    tm1637.display(2, 0x7f);
    tm1637.display(3, luz);
    delay(200);
    tm1637.init();
    tm1637b.init();
    bflag=false;
  }
}


//-------------
void AtlzTMC(){
  int digitHoraH = H / 10;
  int digitHoraL = H % 10;
  int digitMinH = M / 10;
  int digitMinL = M % 10;
  int digitSecH = S / 10;
  int digitSecL = S % 10;

  dht.read();  
  int temp = dht.getTemperatureC();
  int humidity = dht.getHumidity();

  //só mostra a nova temperatura se ela realmente estabilizar
  //necessario este tratamento pq o DHT varia até 2 graus constantemente
  if ( temp != oldtemp) {
    estabiliza++;
    if (estabiliza > 7) {
      estabiliza  = 0 ; 
      oldtemp = temp ;  
    }
  }
  else
    estabiliza = 0; 
  
  int digitoneT = oldtemp / 10;
  int digittwoT = oldtemp % 10;
  int digitoneH = humidity / 10;
  int digittwoH = humidity % 10;


  cont++;
  
if (display1) {
  // pisca os 2 pontos da hora
  if (cont>1) 
    tm1637b.point(POINT_OFF);
  else
    tm1637b.point(POINT_ON);
  
  if (cont > 1) cont = 0;

    //display time
    tm1637b.display(0, digitHoraH);
    tm1637b.display(1, digitHoraL);
    tm1637b.display(2, digitMinH);
    tm1637b.display(3, digitMinL);

  if (S%3 == 0) {
    cont2++;
    if (cont2>10) {
      //display humidity
      tm1637.display(0, digitoneH);
      tm1637.display(1, digittwoH);
      tm1637.display(2, 19);
      tm1637.display(3, 17);
      if (cont2>12) cont2 = 0;
    } 
    else {
      //display temperature
      tm1637.display(0, digitoneT);
      tm1637.display(1, digittwoT);
      tm1637.display(2, 16); 
      tm1637.display(3, 12);
      }
  }
}
else {
    //display temperature
    tm1637.point(POINT_OFF);
    tm1637.display(0, digitoneT );
    tm1637.display(1, digittwoT );
    tm1637.display(2, 16 );
    tm1637.display(3, 12 );
    //display humidity    
    tm1637b.point(POINT_OFF);
    tm1637b.display(0, digitoneH);
    tm1637b.display(1, digittwoH);
    tm1637b.display(2, 19); 
    tm1637b.display(3, 17);
} 
}

