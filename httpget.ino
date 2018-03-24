#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

/*Seta o nome da rede e senha*/ 
const char* ssid = "Conect wifi";
const char* password = "assislucas2018";
 
void setup () {
  //Define a velocidade da comunicação serial
  Serial.begin(115200);
  //inicia a conexão wifi passando o nome da rede e senha
  WiFi.begin(ssid, password);
  //tenta conectar a rede wifi
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..\n");
 
  }
 
}
 
void loop() {

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) { 
    
    //Declare an object of class HTTPClient
    HTTPClient http;  
    //Specify request destination
    http.begin("http://jsonplaceholder.typicode.com/users/1");  //link test

    //Send the request and store the response code
    int httpCode = http.GET();                                                                 
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);             //Print the response payload
 
    }
 
    http.end();   //Close connection
 
  }
 
  delay(10000);    //Send a request every 10 seconds
 
}
