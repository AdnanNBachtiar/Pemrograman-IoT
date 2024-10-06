#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Deimon";
const char* password = "Pmi@1234";

// replace with thingspeak.com apikey

String serverName = "http://api.thingspeak.com/update?api_key=LGU61DTEKX4I9RBK";

// default timer is set to 10 second fot testing

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  Serial.println("");

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 1- seconds(timerDelay variable), it will take 10 seconds before publishing the first reading.");

  randomSeed(analogRead(33));
}

void loop() {
  // send an http get request

  if((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "&field1=" + String(random(40));
      

      http.begin(client, serverPath.c_str());

      int httpResponseCode = http.GET();

      if(httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}