#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* SSID = "LucasBernardo"; // rede wifi
const char* PASSWORD = "mede5reaisquetedigo"; // senha da rede wifi

const char* BROKER = "test.mosquitto.org";
int BROKER_PORT = 1883; 

#define LED D0
#define WAIT D1

// prototypes
void initPins();
void initSerial();
void initWiFi();
void initMQTT();

WiFiClient espClient;
PubSubClient MQTT(espClient); // instancia o mqtt

void setup() {
  initPins();
  initSerial();
  initWiFi();
  initMQTT();
}

void loop() {
  digitalWrite(LED, HIGH);
  if (!MQTT.connected()) {
    reconnectMQTT();//tenta conectar ao servidor broker
  }
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
  Serial.println("MQTT loop...");
  //recconectWiFi();//conecta a rede wifi
  MQTT.loop();
}

// implementacao dos prototypes

void initPins() {
  pinMode(LED, OUTPUT);
  pinMode(WAIT, OUTPUT);
  digitalWrite(LED, 0);
  digitalWrite(WAIT, 0);
}

void initSerial() {
  Serial.begin(115200);
}

void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));

  
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WAIT, HIGH);
    delay(100);
    digitalWrite(WAIT, LOW);
    delay(100);
  }
  Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
}

// Funcão para se conectar ao Broker MQTT
void initMQTT() {
  Serial.println("initMQTT begin!!!");
  MQTT.setServer(BROKER, BROKER_PORT);//seta a porta e o endereço do broker
  MQTT.setCallback(mqtt_callback);//faz uma chamada ao broker
}

//Função que recebe as mensagens publicadas
void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  String message;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    message += c;
  }
  Serial.println("Tópico => " + String(topic) + " | Valor => " + String(message));
  if (message == "1") {
    digitalWrite(LED, 1);
  } else {
    digitalWrite(LED, 0);
  }
  Serial.flush();
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Tentando se conectar ao Broker MQTT: " + String(BROKER));
    if (MQTT.connect("ESP8266-ESP12")) {
      Serial.println("Conectado");
      MQTT.subscribe("temp/random");
    } else {
      Serial.println("Falha ao Reconectar");
      Serial.println("Tentando se reconectar em 2 segundos");
      delay(2000);
    }
  }
}

void recconectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
}
