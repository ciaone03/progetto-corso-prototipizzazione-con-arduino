#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include <RF24_config.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#define	trigger		3 
#define	echo		2
#define m1p			6	//avanti	
#define m1n			5	//indietro
 
//stato della porta (input a esp32)
#define ledblu1 4//ledVERDE
#define ledblu2 9//ledROSSO  
//input da esp 32
//aprire porta
#define ledverde A2 
//chiudere porta
#define ledrosso A1

#define buzzer A3

const int soglia=512;               

RF24 radio(7,8);
const byte address[6] = "00001";

int statoPorta=0;//default porta aperta
int messaggio=3;
int i=20;//i = potenza motore

void setup()
{
  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(m1p,OUTPUT);
  pinMode(m1n,OUTPUT);
  pinMode(buzzer,OUTPUT);
  //
  pinMode(ledblu1,OUTPUT);
  pinMode(ledblu2,OUTPUT);
  pinMode(ledrosso,INPUT);
  pinMode(ledverde,INPUT);
  //
  lcd.begin(16,2); //Init with pin default ESP8266 or ARDUINO
  lcd.init();
  lcd.backlight(); //accende la retroilluminazione
  Serial.begin(9600);
  delay(2000);
  
  //inizio parte wirless slave
  // Inizializza il modulo radio
  radio.begin();

  // Imposta il livello di potenza di trasmissione
  radio.setPALevel(RF24_PA_MIN);

  // Imposta la velocità di trasmissione dati
  radio.setDataRate(RF24_250KBPS);

  // Imposta il canale radio
  radio.setChannel(108);

  // Imposta l'indirizzo del destinatario
  radio.openReadingPipe(0, address);

  // Abilita la ricezione dei messaggi
  radio.startListening();
}
//funzione per sensore distanza
int getdist(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);  
  delayMicroseconds(10);
  digitalWrite(trigger,LOW);
  long durata = pulseIn(echo,HIGH);
  int distanza = durata*0.034/2;
  return distanza;
}

//funzione apertura porta
void apri()
{
  lcd.clear();
  digitalWrite(m1p, i);    	//i= potenza motore(velocità)
  digitalWrite(m1n, LOW);
  lcd.print("apertura porta");
  delay(500);//da testare, tempo per girare chiave
  statoPorta=1;
  digitalWrite(ledblu1,HIGH);
  digitalWrite(ledblu2,LOW);
}

//funzione apertura porta
void chiudi()
{
  lcd.clear();
  digitalWrite(m1p, LOW);
  digitalWrite(m1n, i);		//i= potenza motore(velocità)
  lcd.print("chiusura porta");
  delay(500);//da testare, tempo per girare chiave
  statoPorta=0;
  digitalWrite(ledblu1,LOW);
  digitalWrite(ledblu2,HIGH);
}

//funzione che fa scattare l'allarme
void errore()
{
  lcd.clear();  
  digitalWrite(m1p, LOW);
  digitalWrite(m1n, LOW);
  lcd.print("tentato ingr");
  while(messaggio==0)
  {
    radio.read(&messaggio, sizeof(messaggio));
  	for(int j=0;j<3;j++){
  	digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW); 
  	delay(500);
  	}
  }
  digitalWrite(ledblu1,HIGH);
  digitalWrite(ledblu2,HIGH);
}

//stato in cui sta se non avvengono azioni
void fermo()
{
  lcd.clear();  
  digitalWrite(m1p, LOW);
  digitalWrite(m1n, LOW);
  
  lcd.print(".");
  delay(500);
  lcd.clear();
  lcd.print("..");
  delay(500);
  lcd.clear();
  lcd.print("...");
  delay(200);
}

void loop()
{
  if(radio.available())
  {
  	radio.read(&messaggio, sizeof(messaggio));
    int ostacolo = getdist();//calcolo_distanza(); 
   	int apertura=analogRead(ledverde);
    int chiusura=analogRead(ledrosso);
    if (ostacolo<5 && statoPorta==0)
    {
    	apri();
      Serial.println("apri ostacolo");
      delay(1000);
    }
    else if (ostacolo<5 && statoPorta==1)
    {
      chiudi();
      Serial.println("chiudi ostacolo");
      delay(1000);
    }
    else if((messaggio==1) && statoPorta==0)
    {
    	apri();
      Serial.println("apri password");
    }else if((apertura>soglia)&&(chiusura<soglia)&&statoPorta==0)
    {
      apri();
      Serial.println("apri bot"); 
    }
  	else if((messaggio==2) && statoPorta==1)
    {
    	chiudi();
      Serial.println("chiudi password");
    }
    else if((apertura<soglia)&&(chiusura>soglia)&&statoPorta==1)
    {
      chiudi();
      Serial.println("chiudi bot");
    }
  	else if(messaggio==0)
    {
    	errore();
      Serial.println("errore");
    }
  	else
    {
    	fermo();
    }
  } 
}
