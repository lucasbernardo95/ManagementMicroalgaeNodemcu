#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid =          "Conect wifi";
const char* password =      "assislucas2018";
const char* mqttServer =    "m14.cloudmqtt.com";
const int   mqttPort =      13367;
const char* mqttUser =      "esp1";
const char* mqttPassword =  "mpm2eeesp";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1500);
 
    }
  }
 
//  client.publish("esp/test", "Hello from ESP8266");
  client.subscribe("mpmee/energyPolicy");
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void loop() {
  if (!client.connected()) {
     while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
     
        if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
     
          Serial.println("connected");  
     
        } else {
     
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(1500);
     
      }
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }  
  }
  client.loop();
}
