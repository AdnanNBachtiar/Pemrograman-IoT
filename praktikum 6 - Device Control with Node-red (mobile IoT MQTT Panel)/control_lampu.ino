#include <WiFi.h>
#include <PubSubClient.h>

#define relay1 4
#define relay2 5
#define relay3 18


const char* ssid = "Deimon";
const char* password = "Pmi@1234";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

String publish = "iot/pertemuan6/msg";
String subscribe = "iot/pertemuan6/relays";

void setup() {
  Serial.begin(115200);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay1, 1);
  digitalWrite(relay2, 1);
  digitalWrite(relay3, 1);

  setup_wifi();
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);
}


