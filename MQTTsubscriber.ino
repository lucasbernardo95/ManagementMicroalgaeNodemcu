#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//const char* SSID     = "LucasBernardo"; //Nome da rede wifi
//const char* PASSWD   = "123456789";     //senha do wifi
//const char* BROKER   = "void";      //endereço do broker //porta 1883 padrão 

#define WIFI D0 //indica que o wifi está conectado
#define DATA D1 //indica que recebeu algum dado via mqtt

WiFiClient WiFiclient; //instÂncia do Clientewifi
PubSubClient client(WiFiclient);    //instância do cliente mqtt

//protótipos de funções declaradas no código
void startPins(); //inicializa os pinos dos sensores a serem usados
void startWifi(); //inicializa a comunicação voa wofo
void startMqtt(); //inicializa a comunicação com o broker
void callback();  //função callback, chamada sempre que ouver notivifação de uma nova mensagem recebida

void setup() {
    startPins();
    startWifi();
    startMqtt();
}

void loop() {
    if (!client.connected()) {
        reconnectaMQTT();
    }
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(WIFI, LOW);
      delay(100);
    }digitalWrite(WIFI, HIGH);
    client.loop();
}

void startPins() {
    Serial.begin(115200);//velocidade da comunicação serial
    pinMode(WIFI, OUTPUT);
    pinMode(DATA, OUTPUT);
    digitalWrite(WIFI, LOW);
    digitalWrite(DATA, LOW);
}

void startWifi() {
    const char* SSID     = "LucasBernardo"; //Nome da rede wifi
    const char* PASSWD   = "123456789"; 
    WiFi.begin(SSID, PASSWD);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(WIFI, HIGH);
      delay(100);
      digitalWrite(WIFI, LOW);
      delay(100);
    }
    digitalWrite(WIFI, HIGH);
}

void startMqtt() {
    const char* BROKER   = "xx.x.x.x";      //endereço do broker 
    const int   PORT     = 1883;      //porta padrão usada pelo mqtt para o broker 1883 
    client.setServer(BROKER, PORT);//seta a porta e o endereço do broker
    client.setCallback(mqtt_callback);//faz uma chamada ao broker
}

//Função que recebe as mensagens publicadas
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        message += c;
    }
    Serial.println("Tópico => " + String(topic) + " | Valor => " + String(message));
    if (length > 1) {
        digitalWrite(DATA, HIGH);
    } else {
        digitalWrite(DATA, LOW);
    }
    Serial.flush();
}


void reconnectaMQTT() {
    
    while (!client.connected()) {
        if (client.connect("ESP8266-ESP12")) {
            client.subscribe("orion/energyPolicy");
        } else {
            Serial.println("Falha ao Reconectar");
            delay(2000);
        }
    }
}
