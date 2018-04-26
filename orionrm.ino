/** passos para executar o código atual
1 - executar o middleware
2 - executar a aplicação publisher
3 - iniciar o MQTTRouter
4 - liberar o firewall 
5 - iniciar a rede wifi compartilhada do pc
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
 
const char* ssid = "projectmiddleware";
const char* password = "middleware2018";

//mensagem do broker mqtt
String message;
//tempo limite de espera para uma mensagem do tópico (energy)
unsigned long timeOut = 0;

WiFiClient WiFiclient;           //instÂncia do Clientewifi
PubSubClient client(WiFiclient); //instância do cliente mqtt

void tryConnect();   
void startMqtt();  
void reconnectaMQTT();
void requestPOST(); 

void setup () {
 
    Serial.begin(115200);
//    Serial.setTimeout(2000);
  
    tryConnect();     //inicia o wifi, tenta conectar por 5s
    startMqtt();      //inicia a conexão com o mqtt broker
    // Aguarda o serial inicializar.
    while (!Serial) { }
}

/*fazz uma requisição via post ao serviço web e 
salva os dados dos sensores  e nível de energia atual*/
void requestPOST(){
    HTTPClient http;  //Declara um objeto da classe HTTPClient

    Serial.println("Fazendo requisição em 10 segundos\n");   
    delay(10000); 
    http.begin("http://10.0.8.13:8085/orionrm/esp");  //Specify request destination
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");

    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& node = jsonBuffer.createObject();
    node["id"] = "ESP2";
    node["regiao"] = "R2";

    JsonObject& energia = jsonBuffer.createObject();
    energia["tipo"] = "energia";
    energia["dado"] = "48";

    JsonObject& agua = jsonBuffer.createObject();
    agua["tipo"] = "agua";
    agua["dado"] = "48";

    JsonObject& ph = jsonBuffer.createObject();
    ph["tipo"] = "PH";
    ph["dado"] = "98";

    JsonArray& sensores = node.createNestedArray("sensores");
    sensores.add(ph);
    sensores.add(agua);
    sensores.add(energia);
    
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
    Serial.println("try connect\n");
    WiFi.begin(ssid, password);//inicia o wifi
    // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    unsigned long limite = millis();
    while (WiFi.status() != WL_CONNECTED) {//while not conect
        // Check to see if
        if (WiFi.status() == WL_CONNECT_FAILED) {
            delay(1000);
        }
        //tenta aconectar por 3s
        if (millis() - limite > 3000){
          Serial.println("Failed to connect to WiFi.");
          return;
        }
    }
}

//Inicia a comunicação com o broker
void startMqtt() {
    Serial.println("startMqtt: ");
    const char* BROKER   = "10.0.8.13";      //endereço do broker 
    const int   PORT     = 1883;      //porta padrão usada pelo mqtt para o broker 1883 
    client.setServer(BROKER, PORT);//seta a porta e o endereço do broker
    client.setCallback(mqtt_callback);//faz uma chamada ao broker
}

//Função que recebe as mensagens publicadas
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        message += c;
    }
    Serial.println("\nTópico => " + String(topic) + " | Valor => " + String(message) + "\n");
    Serial.flush();
}

void reconnectaMQTT() {
    unsigned long limite = millis(); 
    Serial.println("reconnectaMQTT: ");
    while (!client.connected()) {
        if (client.connect("ESP8266-ESP12")) {
            client.subscribe("orion/energyPolicy");      
            Serial.println("inscrito\n");
        } else {
            Serial.println("Falha ao Reconectar\n");
            delay(1000);
        }
        //tenta aconectar por 3s
        if (millis() - limite > 3000){
          return;
        }
    }
}

void loop() {
    requestPOST();    //salva os dados dos sensores e do node
    if (!client.connected()) {
        reconnectaMQTT(); //verifica a conecxão com o mqtt e se inscrve em um tópico
    }

    timeOut = millis(); 
    while ( message.length() < 5) { //verifica se chegou alguma mensagem
        client.loop();
        delay(500); 
        //espere apenas por 3 segundos
        if (millis() - timeOut > 3000){//se o tempo esgotou e não chegou mensagem dorme por um itntervalo maior
              Serial.println("\ntempo esgotado!! partiu, ninar por 3s\n");
              ESP.deepSleep(3e6);
              return;
        }
    }
    if ( message.length() > 5) {//se tem mensagem
      Serial.println("opa fion!!! " + String(message));
      ESP.deepSleep(8e6);
              return;
    }
}
