#include <Arduino_JSON.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> // Library untuk HTTPS

const char* ssid = "Deimon";
const char* password = "Pmi@1234";

String openWeatherMapApiKey = "b2a1181c192c038a96853517b9cb3394";

String city = "jakarta";
String countrycode = "ID";

unsigned long lastTime = 0;

unsigned long timerDelay = 10000;

String jsonBuffer;

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

  Serial.println("Timer set to 10 Seconds(timerDelay variable), it will take 10 seconds before publishing the first reading");
}

void loop() {
  if((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED) {
      String serverPath = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countrycode + "&appid=" + openWeatherMapApiKey;

      jsonBuffer = httpGETRequest(serverPath.c_str());

      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

      if(JSON.typeof(myObject) == "Undefined") {
        Serial.println("Parsing input failed");
        return;
      }

      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");

      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");

      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
    } else {
        Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  } 
}

String httpGETRequest(const char* serverName) {
  WiFiClientSecure client;
  client.setInsecure(); // Untuk testing, abaikan sertifikat SSL. Ini tidak aman untuk produksi

  HTTPClient http;

  http.begin(client, serverName);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if(httpResponseCode>0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}