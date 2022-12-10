// --- LIBRARIES INCLUDE ---
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// --- DEFAULT SETTINGS ---
#define ESP_ID          "1"

#define WIFI_SSID       "SSID"
#define WIFI_PASSWORD   "PASS"

#define MQTT_SERVER     "192.168.10.1"

#define LOOP_TIME       3000 //3 SECS LOOP TIME

WiFiClient wifi;
PubSubClient client(wifi);

//Valves

char *valves[] = {
  "1-1",
  "1-2",
  "1-3",
  "1-4",
  "1-5",
  "1-6",
  "1-7",
  "1-8",
};

unsigned int valvesCount = 8;

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi:");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected to");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a client ID
    String clientId = "ESP-WROOM-02-ID-";
    clientId += ESP_ID;
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("sensor/1-1/temperature");
    } else {
      Serial.print("Error! Message: ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //Publish of each heating valve status
  for (int i = 0; i < valvesCount; i++) {
    Serial.print("Publishing valve ID: ");
    Serial.println(valves[i]);
    String valveName = "valve/";
    valveName += valves[i];
    client.publish(String(valveName).c_str(), String(random(0, 100)).c_str(), true);
  }

  delay(LOOP_TIME);
}
