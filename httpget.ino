#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
void setup () {
  //Define a velocidade da comunicação serial
  Serial.begin(115200);
  //inicia a conexão wifi passando o nome da rede e senha
  WiFi.begin("Conect wifi", "assislucas2018");
  //tenta conectar a rede wifi
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(500);
    Serial.print("trying to connect..\n");
 
  }
 
}
 
void loop() {

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) { 
    
    //Declare an object of class HTTPClient
    HTTPClient http;  
    //Specify request destination
    //http.begin("http://jsonplaceholder.typicode.com/users");  //link test
    http.begin("http://10.0.8.9:8080/orionrm/esp/list");
    //Specify content
    http.addHeader("Content-Type", "text/plain");
    
    //Send the request and store the response code
    int httpCode = http.POST("");                                                                 
    String payload = http.getString();   //Get the request response payload
     
    Serial.println(payload);             //Print the response payload
    Serial.println(httpCode);             //Print the HTTP response code
 
    http.end();   //Close connection
 
  } else{
 
    Serial.println("Error in WiFi connection");   
 
  }
 
  delay(10000);    //Send a request every 10 seconds
 
}
