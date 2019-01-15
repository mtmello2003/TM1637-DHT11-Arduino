//MTM JAN 2019

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

const float offset = -3.5;
const float h_offset = 5;
const byte ledPin = 13;

TM1637 tm1637(CLK, DIO);
TM1637 tm1637b(CLK2, DIO2);
DHT dht(DHTPIN, DHT11);

int temp = 0;
int humidity = 0;
float oldtemp = 0;
float lasttemp = 0;
int oldtemp2  = 0;
int oldhumidity = 0;
int olddigito = 0;
int estabiliza = 0;
float temperature;

void setup() {
  dht.begin();
  
  Serial.begin(9600);
  tm1637.init();
  tm1637.set(7);
  tm1637b.init();
  tm1637b.set(7);
  tm1637.point(POINT_OFF);

  tm1637.display(0, 0x7f);
  tm1637.display(1, 0x7f);
  tm1637.display(2, 0x7f);
  tm1637.display(3, 0x7f);
  tm1637b.display(0, 0x7f);
  tm1637b.display(1, 0x7f);
  tm1637b.display(2, 0x7f);
  tm1637b.display(3, 0x7f);
  delay(100);
}


//-----------
void loop() {
  
  //pisca o led do arduino
  digitalWrite(ledPin, !digitalRead(ledPin)); 

  // atualiza display
  AtlzTMC();
  AtlzTMC(); 

} //end void loop
  

//-------------
void AtlzTMC(){
 
  dht.read();  
  Serial.begin(9600);
  delay(1000);
  float temperature = dht.getTemperatureC();
  temperature  = temperature + offset;
  int   temp = temperature;
  Serial.println(temperature);
 
  int  humidity = dht.getHumidity();
  humidity = humidity + h_offset;
  int   digito = (temperature - temp) * 10;
  Serial.println("----------");

  if (oldtemp==0 & olddigito==0) {
    oldtemp = temp;
    olddigito = digito;
  }


  //só mostra a nova temperatura se ela realmente estabilizar
  //necessario este tratamento pq o DHT varia até 2 graus constantemente
  if ((temperature != oldtemp)&(temperature == lasttemp)) {
    estabiliza++;
    if (estabiliza > 1) {
      estabiliza  = 0 ; 
      oldtemp = temperature ;
      olddigito = digito;
      lasttemp  = temperature;
    }
  }
  else
    estabiliza = 0; 
    lasttemp = temperature;

   //converte de float para int
   oldtemp2 = oldtemp; 
   
   int digitoneT = oldtemp2 / 10;
   int digittwoT = oldtemp2 % 10;
   int digitoneH = humidity / 10;
   int digittwoH = humidity % 10;
 
    //display temperature
    tm1637.point(POINT_OFF);
    tm1637.display(0, digitoneT );
    tm1637.display(1, digittwoT );
    tm1637.display(2, 16 );
    tm1637.display(3, olddigito );
    //tm1637.display(3, 12 );
    //display humidity    
    tm1637b.point(POINT_OFF);
    tm1637b.display(0, digitoneH);
    tm1637b.display(1, digittwoH);
    tm1637b.display(2, 19); 
    tm1637b.display(3, 17);
}
