//TOPIC is Topic
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define ENA D2
#define PWM_FREQUENCY 40000
int PWM = 180;  //0 - 255;
// Update these with values suitable for your network.

const char* ssid = "TP-JAVI";
const char* password = "xavier1234";
const char* mqtt_server = "192.168.0.250";
const char* TOPIC = "Tanque1/canal/airflow/motor1";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  analogWriteFreq(PWM_FREQUENCY);
  Serial.begin(115200);  //Start Serial Monitor to display current read value on Serial monitor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);

  analogWrite(ENA, PWM);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char number[length + 1];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] msg: ");
  for (int i = 0; i < length; i++) {
    number[i] = (char)payload[i];
    //Serial.print(number[i]);
  }
  number[length] = 0;
  Serial.print(" length: ");
  Serial.print(length);
  Serial.println();


  if (strcmp(topic, TOPIC) == 0) {
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
    String test = String(number);

    PWM = test.toInt();
    Serial.println(PWM);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC, "Connected");
      // ... and resubscribe
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}