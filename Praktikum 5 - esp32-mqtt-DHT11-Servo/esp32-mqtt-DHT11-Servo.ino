#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP32Servo.h>

#define pinDHT 5
#define pinServo 26
#define DHTType DHT11

DHT dht(pinDHT, DHTType);
Servo servo;

const char* ssid = "Deimon";
const char* password = "Pmi@1234";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long delayTime = 10000;  // delay publish

void setup() {
  Serial.begin(115200);
  dht.begin();
  servo.attach(pinServo);
  servo.write(0);
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
  Serial.println("WiFi Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(200);
}

void loop() {
  if (!client.connected()) {
    Reconnect();
  }
  client.loop();

  currentTime = millis();
  if (currentTime - previousTime >= delayTime) {
    previousTime = currentTime;

    // Membaca suhu dan kelembapan
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Validasi apakah pembacaan sensor valid
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Gagal membaca sensor DHT");
      return;  // Jangan kirim data jika sensor gagal dibaca
    }

    // Buat format JSON
    String jsonData = "{\"temperature\":" + String(temperature, 2) + ",\"humidity\":" + String(humidity, 2) + "}";

    // Konversi String ke array char
    int jsonDataLen = jsonData.length() + 1;
    char jsonDataChar[jsonDataLen];
    jsonData.toCharArray(jsonDataChar, jsonDataLen);

    // Tampilkan di serial monitor dan kirim via MQTT
    Serial.print("{0}");
    Serial.println(jsonData);
    client.publish("IoT_Sensor", jsonDataChar);
  }
}


void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "IoT_Command") {
    Serial.print("Pesan:");
    Serial.println(messageTemp);

    // Pastikan format JSON
    if (messageTemp.startsWith("{") && messageTemp.endsWith("}")) {
      // Ambil nilai posisi servo dari JSON
      int servoPos = parseServoPosition(messageTemp);
      if (servoPos >= 0 && servoPos <= 180) {
        servo.write(servoPos);
      } else {
        Serial.println("Nilai servo tidak valid. Harus antara 0 dan 180.");
      }
    } else {
      Serial.println("Pesan tidak dalam format JSON yang valid.");
    }
  }
}

int parseServoPosition(String jsonMessage) {
  jsonMessage.trim();
  int servoPos = -1;  // Nilai default untuk posisi servo tidak valid
  int start = jsonMessage.indexOf(":") + 1;
  int end = jsonMessage.indexOf("}");
  if (start > 0 && end > start) {
    String posString = jsonMessage.substring(start, end);
    servoPos = posString.toInt();  // Konversi dari string ke integer
  }
  return servoPos;
}

void Reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT Connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    Serial.print("Client ID:" + clientId + "|Status:");

    if (client.connect(clientId.c_str())) {
      Serial.print("Connected");
      client.subscribe("IoT_Command");
    } else {
      Serial.print("Failed,rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
