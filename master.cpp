//librerie per i moduli NRF24L01
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
//altre librerie
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include<SPI.h>

RF24 radio (7, 8);
const byte address[6]= "00001";

//configurazione tipo lcd
LiquidCrystal_I2C lcd(0x27,16,2);

//per tastierino 
const byte ROWS = 4;
const byte COLS = 4; 


const byte PASSWORDLENGTH = 5;
char password[PASSWORDLENGTH] = "12CD";
char chiudi[PASSWORDLENGTH] = "####";
char data[PASSWORDLENGTH];

int counter = 0;
int messaggio = 0;
int contaErrori = 0;
int i = 0;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pinROWS[ROWS] = {A0, A1, A2, 6}; 
byte pinCOLS[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), pinROWS, pinCOLS, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  pinMode(pinLED,OUTPUT);
  lcd.begin(16,2); //Init with pin default ESP8266 or ARDUINO
  lcd.init();
  lcd.backlight(); //accende la retroilluminazione
  
  // Inizializza il modulo radio
  radio.begin();
  // Imposta il livello di potenza di trasmissione
  radio.setPALevel(RF24_PA_MIN);
  // Imposta la velocità di trasmissione dati
  radio.setDataRate(RF24_250KBPS);
  // Imposta il canale radio
  radio.setChannel(108);
  // Imposta l'indirizzo del mittente
  radio.openWritingPipe(address);
}
  
void loop()
{ 
  lcd.setCursor(0,0);
  char customKey = customKeypad.getKey();
  if (customKey)
  {
  	data[counter] = customKey;
    lcd.setCursor(0,0); 
    lcd.print(data);
    delay(100);
    counter++;
    
    //controllo password inserita    
    if(counter==PASSWORDLENGTH-1)
    {
    	lcd.clear();
      //guardo se la password messa è quella per aprire la porta  
      if(!strcmp(data,password))
    	{
   			Serial.println("Password Corretta");
          	lcd.print("Password giusta");
          	messaggio=1;
          	digitalWrite(pinLED,HIGH);
          	delay(1000);
       	 	digitalWrite(pinLED,LOW);
          	contaErrori=0;
    	}
      //in caso non lo sia vedo se è quella per chiuderla
      else if(!strcmp(data,chiudi))
      {
        	Serial.println("Porta Chiusa");
        	lcd.print("Porta Chiusa");
        	messaggio=2;
        	digitalWrite(pinLED,HIGH);
        	delay(1000);
     	 	digitalWrite(pinLED,LOW);
      }
      //se non è nanche quella allora..
    	else
      {	
        Serial.println("Password Errata");
       	//se hai gia sbagliato fa partire l'allarme
       	if(contaErrori>1)
        {
          Serial.println("allarme");
         	lcd.print("Password Errata");        
				  lcd.setCursor(0, 1);       
				  lcd.print("blocco 10 secondi");
          messaggio=0;
          for(i=0;i<5;i++)
        	{
        		digitalWrite(pinLED,HIGH);
       			delay(1000);
   	   			digitalWrite(pinLED,LOW);
          	delay(1000);
      		}
        	contaErrori=0;
        }
        //altrimenti aumenta il contatore errori e puoi reinserirla
        else
        {
          Serial.println("Psbaglio Errata"); 
          lcd.print("Password Errata");
         	delay(2000);
         	contaErrori++;
        }
      }
    	counter = 0;
    	lcd.clear();
    	for(i=0;i<4;i++)
      {
      	data[i]=0;
      }
    }
  }
  //invia il messaggio
  Serial.print("messaggio: ");
  Serial.print(messaggio);  
  Serial.println();        
  radio.write(&messaggio,sizeof(messaggio));
  //pone il messaggio a 3 (dice allo slave di stare in posizione "fermo")  
  messaggio=3;
}
