/* Home automation with ESP32
 * @author: Hasithakms@gmailcom
 * 
 * Currently light bulb control with relay module available
 * 
 * Resources
 * https://hackernoon.com/cloud-home-automation-series-part-4-connected-light-bulb-using-aws-esp32-and-arduino-94n377d
 * 
*/
#include "WiFi.h"
#include <PubSubClient.h>

int relay_pin = 13; // Relay Pin

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "SSID";
const char* password = "YOUR_WIFI_PASSWORD_HERE";
const char* mqtt_server = "MQTT_SERVER";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(String topic, byte * message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (topic == "room/lamp") {
    Serial.print("Changing Room lamp to ");
    if (messageTemp == "on") {
      digitalWrite(relay_pin, HIGH);
      Serial.print("On");
    }
    else if (messageTemp == "off") {
      digitalWrite(relay_pin, LOW);
      Serial.print("Off");
    }
  }
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe("room/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(relay_pin, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  if (!client.loop())
    client.connect("ESP8266Client");
}
