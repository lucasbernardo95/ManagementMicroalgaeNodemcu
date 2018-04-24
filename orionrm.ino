#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
 
const char* ssid = "projectmiddleware";
const char* password = "middleware2018";

WiFiClient WiFiclient;           //instÂncia do Clientewifi
PubSubClient client(WiFiclient); //instância do cliente mqtt

void setup () {
 
    Serial.begin(115200);
    Serial.setTimeout(2000);

    // Aguarda o serial inicializar.
    while (!Serial) { }
  
    tryConnect(); //tenta conectar por 5s
    requestPOST(); //salva os dados dos sensores e do node
    //verifica se tem dado no tópico inscrito

    //hora do ronco, lek
    Serial.println("Going into deep sleep for 20 seconds");
    ESP.deepSleep(20e6); // 20e6 is 20 microseconds
}

/*fazz uma requisição via post ao serviço web e 
salva os dados dos sensores  e nível de energia atual*/
void requestPOST(){
    HTTPClient http;  //Declara um objeto da classe HTTPClient

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
    int httpCode = http.POST(sms); //Send the request e receive the http code

    //String payload = http.getString();   //Get the request response payload
    //Serial.println(payload);                     //Print the response payload
    //Serial.println(httpCode);  
 
    http.end();   //Close connection
    delay(10000); 
}

void tryConnect() {
    WiFi.begin("projectmiddleware", "middleware2018");//inicia o wifi
    // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.begin("projectmiddleware", "middleware2018");
    unsigned long wifiConnectStart = millis();
    while (WiFi.status() != WL_CONNECTED) {//while not conect
      // Check to see if
      if (WiFi.status() == WL_CONNECT_FAILED) {
          Serial.println("Failed to connect to WiFi. Please verify credentials: ");
          delay(10000);
      }
  
      delay(500);
      // Only try for 5 seconds.
      if (millis() - wifiConnectStart > 15000) {
          Serial.println("Failed to connect to WiFi");
          return;
      }
  }
}
 
void loop() {
}
