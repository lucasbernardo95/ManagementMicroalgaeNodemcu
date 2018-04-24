#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
 
const char* ssid = "projectmiddleware";
const char* password = "middleware2018";
 
void setup () {
 Serial.begin(115200);
  Serial.println();  //Clear some garbage that may be printed to the serial console
}
 
void loop() {
 
StaticJsonBuffer<200> jsonBuffer;
JsonObject& node = jsonBuffer.createObject();
node["id"] = "ESP3";
node["regiao"] = "R3";
node["energia"] = "87.25";

Serial.println("\n");
node.printTo(Serial);
Serial.println("\nNode criado\n");

JsonObject& agua = jsonBuffer.createObject();
agua["tipo"] = "agua";
agua["dado"] = "48";

Serial.println("\n");
agua.printTo(Serial);
Serial.println("\nSensor agua criado\n");

JsonObject& ph = jsonBuffer.createObject();
ph["tipo"] = "PH";
ph["dado"] = "98";

Serial.println("\n");
ph.printTo(Serial);
Serial.println("\nSensor PH criado\n");

JsonArray& sensores = node.createNestedArray("sensores");
sensores.add(ph);
sensores.add(agua);


Serial.println("\n");
sensores.prettyPrintTo(Serial);
Serial.println("\nObjeto sensores criado\n");


Serial.println("\n\n\n");
//node.printTo(Serial);//printa o json todo em uma linha]
node.prettyPrintTo(Serial);//printa o json em linhas quebradas de forma mais legível
Serial.println("\nObjeto node final\n");

/** exemplo do node final
{
  "id": "ESP3",
  "regiao": "R3",
  "energia": "87.25",
  "sensores": [
    {
      "tipo": "PH",
      "dado": "98"
    },
    {
      "tipo": "agua",
      "dado": "48"
    }
  ]
}
*/
 delay(50000);
}
