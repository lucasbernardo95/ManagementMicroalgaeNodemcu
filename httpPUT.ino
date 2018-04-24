#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
 
const char* ssid = "projectmiddleware";
const char* password = "middleware2018";
 
void setup () {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..");
 
  }
 
}
 
void loop() {
 
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    Serial.println("Conectado\n");   
    HTTPClient http;  //Declare an object of class HTTPClient

    Serial.println("Fazendo requisição em 10 segundos\n");   
    delay(10000); 
    http.begin("http://10.0.8.9:8085/orionrm/esp");  //Specify request destination
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& node = jsonBuffer.createObject();
    node["id"] = "ESP3";
    node["regiao"] = "R3";
    node["energia"] = "87.25";

    JsonObject& agua = jsonBuffer.createObject();
    agua["tipo"] = "agua";
    agua["dado"] = "48";

    JsonObject& ph = jsonBuffer.createObject();
    ph["tipo"] = "PH";
    ph["dado"] = "98";

    JsonArray& sensores = node.createNestedArray("sensores");
    sensores.add(ph);
    sensores.add(agua);
    
    String sms;
    node.printTo(sms);
    //int httpCode = http.PUT(sms);
    int httpCode = http.POST(sms);                                                                  //Send the request

    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);                     //Print the response payload
    Serial.println(httpCode);  
 
    http.end();   //Close connection
 
  }
  Serial.println("delay de 10 segundos\n");   
  delay(10000);    //Send a request every 30 seconds
 
}
