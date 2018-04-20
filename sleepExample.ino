/**
 * An example showing how to put ESP8266 into Deep-sleep mode
 */
#define LED D1
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(500);

  digitalWrite(LED, HIGH);
  // Wait for serial to initialize.
  while(!Serial) { }
  
  Serial.println("I'm awake.");

  Serial.println("Going into deep sleep for 5 seconds");
  ESP.deepSleep(5e6); // 5e6 is 5 microseconds
  digitalWrite(LED, LOW);
  delay(1000);
}

void loop() {
}
