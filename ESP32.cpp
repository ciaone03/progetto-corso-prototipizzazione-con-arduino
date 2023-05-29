#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
/*
char* ssid = "TIM-nuovarete";
char* password = "123456789abc";
*/
char* ssid = "Oppo";
char* password = "z7digay3";
char* botToken = "6080515741:AAG_E3pzqBO33FovHK05FklEqi7E9byVzWo";
/////////////////////////////////////////
#define apertura  33//output porta aperta
#define chiusura  27//output porta chiusa
#define Pindato1  15//input porta 1
#define Pindato2  12//input porta 2
int vet[10]={0,0,0,0,0,0,0,0,0,0};
int pinval1 = 0;
int pinval2 = 0;
int mem1=0;
int mem2=0;
String stringastorico = "";
///////////////////////////////////////////
void setup() {
  /////////////////////////////
  pinMode(apertura, OUTPUT);
  pinMode(chiusura, OUTPUT);
  pinMode(Pindato1, INPUT);
  pinMode(Pindato2, INPUT);
  /////////////////////////////
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Ottieni chat_id dell'utente che ha inviato l'ultimo messaggio al bot 
  String url = "https://api.telegram.org/bot" + String(botToken) + "/getUpdates"; 
  HTTPClient http; 
  http.begin(url); 
  int httpCode = http.GET(); 
  if (httpCode > 0) { 
    String payload = http.getString(); 
    Serial.println(payload); // stampa la risposta dell'API per il debug 
    // Analizza la risposta JSON dell'API e ottieni il chat_id 
    int chat_id = payload.substring(payload.indexOf("\"chat\":{\"id\":") + 13, payload.indexOf(",\"first_name\"")).toInt(); 
    Serial.println("chat_id: " + String(chat_id)); // stampa il chat_id per il debug 
  }
  
  delay(1000);
}

void loop() {
  //String x= recive();
///////////////cambia da vttore a stringa per storico
  pinval1=digitalRead(Pindato1);
  pinval2=digitalRead(Pindato2);
  if(pinval1!=mem1 || pinval2!=mem2)
  {
    mem1=pinval1;
    mem2=pinval2;
    for (int i=9;i>0;i--)
    {
      vet[i]=vet[i-1];
    }if (pinval1==HIGH && pinval2==LOW)
    {vet[0]=1;} 
    else if (pinval1==LOW && pinval2==HIGH)
    {vet[0]=2;}
    else if (pinval1==HIGH && pinval2==HIGH)
    {vet[0]=3;}
    else if(pinval1==LOW && pinval2==LOW)
    {vet[0]=4;}
  }
  stringastorico= "";
  for (int i = 0; i < 10; i++) {
    stringastorico += String(vet[i]) + " ";
  }
////////////////////
  // Controlla se ci sono nuovi messaggi 
  String msgLast = recive();
  Serial.println("Ultimo messaggio: " + msgLast);

  if(msgLast == "apri") {
    //TODO
    send("-1001934292459", "Porta Aperta");
    digitalWrite(apertura, HIGH); 
    digitalWrite(chiusura, LOW);
    delay(1000);
  }
  else if(msgLast == "chiudi") {
    //TODO
    send("-1001934292459", "Porta Chiusa");
    digitalWrite(chiusura, HIGH);
    digitalWrite(apertura, LOW); 
    delay(1000);
  }
  else if(msgLast == "statoporta") {
    send("-1001934292459", "Stato Porta:");
    //TODO
    if(digitalRead(Pindato1)==HIGH && digitalRead(Pindato2)==LOW)
    {
      send("-1001934292459", "Porta aperta");
    }else if (digitalRead(Pindato1)==LOW && digitalRead(Pindato2)==HIGH)
    {
      send("-1001934292459", "Porta chiusa");
    }else if (digitalRead(Pindato1)==HIGH && digitalRead(Pindato2)==HIGH)
    {
     send("-1001934292459", "allarme");
    }else
    {
      send("-1001934292459", "errore rivelamento");           
    }
    delay(1000);
  }
  else if(msgLast == "storico") {
    send("-1001934292459", "Storico: ");
    //TODO
    send("-1001934292459", " 0=vuoto, 1=aperta, 2=chiusa, 3=allarme, 4=errore rilevamento: ");
    send("-1001934292459", stringastorico);
  }else if(msgLast == "toktok"){
    send("-1001934292459", "FBI open UP");
  }
  digitalWrite(chiusura, LOW);
  digitalWrite(apertura, LOW);

}


// -----------------------------------------------------------------------------------------
String tempPayload="";
String recive()
{
  String url = "https://api.telegram.org/bot" + String(botToken) + "/getUpdates?offset=-1"; // offset=-1 restituisce solo l'ultimo messaggio 
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
  //  Serial.println(payload);
    http.end();
    delay(100);
    if(tempPayload == "")
    {
      tempPayload = payload;
    }
    if(tempPayload != payload)
    {
      tempPayload = payload;
      return search(payload);
    }
    return "";
  }
  
  else
    return "Error recive: " + String(httpCode);
}

  
void send(String chatID, String message)
{
  String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage?chat_id=" + chatID+ "&text=" + message;
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  
  if(httpCode > 0)
    Serial.println("CODE: " + httpCode);
  
  Serial.println("Message sent");
  http.end();
  delay(100);
}


String search(String text)
{
  String tmp = "";
  for(int i=0; text[i]!='\0'; i++)
  {
    if(text[i] == '/')
    {
      i++;
      for(int j=i; text[j]!='@' ;j++)
        tmp += text[j];
    }
  }
  return tmp;
}

// -----------------------------------------------------------------------------------------
