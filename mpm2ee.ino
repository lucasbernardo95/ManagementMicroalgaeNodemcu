/**
 *                    MPM2EE         
 * Middleware for Policy Management and Energy Economics 
 * Middleware para Gerenciamento de Políticas de Economia de Energia
 * 
 * Rede: notebook
 * desa.. fw p/ rece pública 
 * 
 * broker local "mosquito"
 */
#include <ESP8266WiFi.h>            //uso do módulo wifi
#include <ESP8266HTTPClient.h>      //uso da lib http
#include <ArduinoJson.h>            //manipulação de arquivos json
#include <PubSubClient.h>           //funcionalidades do protocolo pub/sub
#include "user_interface.h"         //interface para habilitar o LIGHT SLEEP wifi_set_sleep_type
#include <EEPROM.h>                 //permite escrever e ler dados na EEPROM

WiFiClient WiFiclient;              //instÂncia do Clientewifi
PubSubClient client(WiFiclient);    //instância do cliente mqtt

long previousMillis = 0;            //Variável de controle do tempo
long timeOutColeta = 60000;         //Tempo em ms do intervalo a ser executado a função salveSensorDate

/*===========================Declaração de protótipos de funções============================*/
void startWifi();                    //Inicia o wifi e tenta contectar
void startMqtt();                   //inicia o Mqtt
void tryReconectMQTT();              //tenta conectar ao broker
void tryReconectWifi();              //Tenta reconectar à rede wifi
void sendSensorDate();              //Chama o método cadastrar do middleware para salvar os dados dos sensores
void callBackMqtt(char* topic, byte* payload, unsigned int length); //Função chamada ao detectar a publicação de um dado no tópico inscrito e aplicar uma política
int getTime();                       //Retorna o tempo de sleep setado ao aplicar uma política
void setTime(uint8_t timeSleep);     //Função para setar um tempo de sleep
int getModeSleep();                  //retorna um inteiro indicando qual o modo de suspenção aplicado
void setModeSleep(uint8_t modesleep);//Função para setar um novo modo de suspenção
void switchEEPROM(bool flag);        //Inicia ou encerra a gravação na EEPROM
/*==========================================================================================*/

/*Seta a configurações necessárias para conectar o wifi e tenta contectar a uma rede*/
void startWifi() {
    Serial.println("========== Start Wifi ==========\n");
    WiFi.persistent(true);
    WiFi.mode(WIFI_OFF); //  Force the ESP into client-only mode (padrão)
    WiFi.mode(WIFI_STA);

    int cont = 0;
    WiFi.begin("meu wifi", "naotemsenha");
    
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("tentando reconectar");
        delay(1500);
        cont ++;
        if (cont == 8){
            return;
        }
    }
    Serial.println("========== End Wifi ==========\n");
}

/*Seta os parâmetros para conexão com o servidor broker*/
void startMqtt() {
    Serial.println("========== start Mqtt ==========\n");
    client.setServer("10.0.8.11", 1883);
    client.setCallback(callBackMqtt);

    int cont = 0;
    while (!client.connected()) {    
          if (client.connect("ESP1")) {
              Serial.println("Conectado");
              client.subscribe("mpm2ee/energyPolicy");
          } else {
              Serial.print("Falha: ");      
              Serial.println(client.state());         
              delay(2000);
              cont ++;
              if (cont == 7)
                  break;
          }
     }Serial.println("========== End startMqtt ==========\n");
}

//tenta reconectar aoa broker
void tryReconectMQTT() {
    if (client.connected()){
        return;
    }
    int cont = 0;
    while (!client.connected()) {    
          //desconecta e reseta os parâmetros de autênticação
          client.disconnect();
          delay(2000);
          if (client.connect("ESP8266Client", "esp8266", "esp8266")) {
              Serial.println("Conectado");
              client.subscribe("mpm2ee/energyPolicy");
          } else {
              Serial.print("Falha: ");      
              Serial.println(client.state());         
              cont++;
              if(cont == 7)
                  break;
          }
     }
}

//tenta reconectar à rede wifi
void tryReconectWifi() {
    
    if (WiFi.status() == WL_CONNECTED){
        return;
    }
    
    Serial.println("======Try Reconect Wifi=========\n");
    int cont = 0;
    if (WiFi.status() != WL_CONNECTED) {                   //Se não estiver conectado ao wifi
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("tentando reconectar");
            delay(1500);
            cont++;
            if (cont == 7)
                break;
            }
    }
}

//faz uma requisição http para o middleware e envia os dados dos sensores via post
void sendSensorDate() {
    Serial.println("==================== start send data ====================\n");
    HTTPClient http;                                    //Declara um objeto da classe HTTPClient
    http.begin("http://10.0.8.11:8085/mpm2ee/esp");     //Especifica o destino da solicitação (endereço do serviço)
    http.addHeader("Content-Type", "application/json"); //Informa os detalhes da requisição (json)
    http.addHeader("Accept", "application/json");       
    http.POST(getSensores ());                          //Envia os dados dos sensores
    http.end();                                         //Encerra a conexão http
    Serial.println("==================== End send data ====================\n");
}

//captura os dados dos sensores e monta um json
String getSensores () {
    Serial.println("criando objetos");
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& node = jsonBuffer.createObject();
    node["id"] = "ESP1";
    node["regiao"] = "Interna";
    node["data"] = "-";
    node["hora"] = "-";
    //verifica qual a política está aplicada, se houver
    int valor = getModeSleep();//seta o modo de sono
    node["duracao"]  = 0;  
    if (valor == 0)
      node["politica"] = "modemsleep";
    else if (valor == 1)
      node["politica"] = "lightsleep";
    else if (valor == 2){
      node["politica"] = "deepsleep";
      node["duracao"]  = valor;
    }else
      node["politica"] = "void";
    
    //Informa cada um dos sensores conectados ao node
    JsonObject& energia = jsonBuffer.createObject();
    energia["tipo"] = "energia";
    energia["dado"] = "48";

    JsonObject& agua = jsonBuffer.createObject();
    agua["tipo"] = "agua";
    agua["dado"] = "48";

    JsonObject& ph = jsonBuffer.createObject();
    ph["tipo"] = "PH";
    ph["dado"] = "98";

    //Insere os dados dos sensores ao node
    JsonArray& sensores = node.createNestedArray("sensores");
    sensores.add(ph);
    sensores.add(agua);
    sensores.add(energia);
    
    String sms;
    node.printTo(sms);             //Printa os dados dos sensores
    Serial.println(sms);
    return sms;
}

/* *
 * Função chamada sempre que alguma mensagem é publicada no tópico em que o node foi inscrito.
 * Recebe a política que foi publicada e em seguida aplica, modificando o estado de sono e trabalho
 * do node.
 */
void callBackMqtt(char* topic, byte* payload, unsigned int length) {
    String mensagem;//mensagem do broker mqtt
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        mensagem += c;
    }Serial.println("Mensagem: " + mensagem);//String(topic)
    Serial.flush(); //Aguarda pela transmissão de dados seriais de saída para concluir.

    if (mensagem.equals("modemsleep")) {
        Serial.println("ModemSleep");
        //altera o valor da eeprom, para evitar entrar em deepsleep caso tenha entrado alguma vez
        uint8_t timesleep = 0;  //valor zero para o tempo e sono quer dizer que não é deepsleep, por tanto altera os valores salvos para não entrar em deepsleep por engano 
        setModeSleep(timesleep);//seta o modo de sono
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
    } else if (mensagem.equals("lightsleep")) {
        Serial.println("LightSleep");
        //altera o valor da eeprom, para evitar entrar em deepsleep caso tenha entrado alguma vez
        uint8_t modesleep = 1;         
        setModeSleep(modesleep);//seta o modo de sono
        wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); //https://github.com/esp8266/Arduino/issues/1381
    } else {
          //pega o valor STRING_C da string recebida e converte para uint8_t
          //o valor recebido deverá ser apenas um inteiro representando quantos
          //minutos o nó deve permanecer em deepsleep
          uint8_t timesleep = (uint8_t) atoi(mensagem.c_str()); 
          uint8_t modesleep = 2;        //2 = deepsleep
          setTime(timesleep);           //seta o tempo de sono
          setModeSleep(modesleep);      //seta o modo de sono
          Serial.println("entrando em deepSleep");
          
          /*Onde o “time_in_us” é o tempo em microssegundos em que o ESP estará em modo 
          deep sleep. O valor máximo é de 4,294,967,295 microssegundos (2^32), o que 
          corresponde a aproximadamente 70 minutos.*/
          ESP.deepSleep(timesleep * 60000000); // irá dormir por "timesleep" minutos
    }
}

void setup() {
    Serial.begin(115200);         //velocidade da comunicação serial
    Serial.setTimeout(2000);      //deley de 2s
    while(!Serial) { }            //Wait for serial to initialize.
    startWifi();                  //Inicia o wifi
    startMqtt();                  //Seta oos parâmetros para conexão com o broker e tenta contectar
}

//a cada 1 minuto, definido em timeOutColeta, o nó envia os dados dos sensores ao middleware e, 
//se o deepsleep tiver aplicado em algum momento o nó irá dormir por um intervalo definido na política
void loop() {
    unsigned long currentMillis = millis();                //Tempo atual em ms => millis() seu valor é zerado em 49 À 50 dias //millis() o tempo que o arduino está ligado

    tryReconectWifi();
    tryReconectMQTT();
    
    client.loop();                                         //Chama o loop do client mqtt para ouvir a rede
    
    if (currentMillis - previousMillis >= timeOutColeta) { //Verifica se o tempo desgotou
        previousMillis = currentMillis;                    // Salva o tempo atual
        sendSensorDate();            //envia os dados dos sensores
        if (getModeSleep() == 2) {                         //se o modo for 2, quer dizer que deve entrar em deepsleep
            int tempo = getTime();                         //recebe o tempo que deverá dormir
            if (tempo <= 0)
                tempo = 1;
            Serial.print("entrando em deepsleep por ");
            Serial.print(tempo * 60000000);
            Serial.println(" segundos");
            //Sleep for x seconds, then wakeup and send data again
            ESP.deepSleep(tempo * 60000000, WAKE_RF_DEFAULT);                       //entra em deepsleep 60000000 = 60 segundos = 1 minuto
        }
    }
}

//Retorna o tempo de sono salvo na EEPROM
int getTime() {
    switchEEPROM(true);           //inicia a eeprom
    return EEPROM.read(1); //Lê o valor salvo na posição 1 da memória
    switchEEPROM(false);           //encerra a eeprom
}

void setTime(uint8_t timeSleep) {
    switchEEPROM(true);           
    EEPROM.write(1, timeSleep); //Escreve o tempo que o nó deverá permanecer dormindo
    switchEEPROM(false);          
}

//Retorna o modo de sono que foi salvo na EEPROM
int getModeSleep() {
    switchEEPROM(true);           
    return EEPROM.read(0); //Lê o valor salvo na posição 0 da memória
    switchEEPROM(false);          
}

void setModeSleep(uint8_t modesleep) {//indica o modo de sono mdeepsleep
    switchEEPROM(true);        
    EEPROM.write(0, modesleep);   //seta o modo de sono 
    switchEEPROM(false);          
}

//inicia ou encerra a EEPROM com base no valor passado. true inicia e false encerra
void switchEEPROM(bool flag){
    if (flag)
        EEPROM.begin(8); //inicia a EEPROM com 8bits para armazenamento
    else 
        EEPROM.end();    //encerra a EEPROM
}
