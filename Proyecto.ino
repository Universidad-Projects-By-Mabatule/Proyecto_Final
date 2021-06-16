  #include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "FS.h"
#include "SPIFFS.h"
#include "DHT.h"
#include <ArduinoJson.h>
#include <Time.h>
#include <TimeLib.h>
#define DHTTYPE DHT11
#define PIN_HECHO 23
#define PIN_TRIG 22
#define DHTPIN 26
DHT dht(DHTPIN, DHTTYPE);
const float SONIDO = 34300.0; // Velocidad del sonido en cm/s
int distancia;
int led_green =    16;
int led_yellow =  0;
int led_blue =      4;  
int buzzer=13;
int fotores=35;
int id=0;
bool resp1;
bool resp2;
bool resp3;
const char* AWS_ENDPOINT = "a2ozm6491st8c4-ats.iot.us-east-2.amazonaws.com";
const int AWS_ENDPOINT_PORT = 8883;

const char* MQTT_CLIENT_ID = "sis234_mcu";
const char* SUBCRIBE_TOPIC = "ucb/esp_in"; // subscribe
const char* PUBLISH_TOPIC = "ucb/esp_out"; // publish

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUcfdqoxPRWcNrwUenthDZeO+Lv38wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDUyMDAzNTEw
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKGz2hhgdvrsJqzLS9nB
u4TGgkDls975PbFyhRtVd7tuZQLXfDEroPqH8rMUP+JWTjFhuJJX+HobvZ+yi8g3
FsjxCPn++L2FBQ5EpEPRBk5ciB1Le18o6ft1YzoSnLrR17n/lvm/SAcGo7+/55mx
G/HYYbh+Kg24CCkI0dKUM0qSf1EpH8OD6XxYZWhR/HJS6/fLfY+h3bXXPxhJpInY
/EFJzek2ObT4B9WSIuySzebrNJzD/QzsfzI3JcwP19bnGOKoqCL1+TugphsEC1yV
i1uPZhsQEZICbOQASuRAUdg5rhDIC/B73bXn5c+lsPdO/hDhDX4B3V5zpFJFmfE2
sC8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUGgksBxLL2ofNGisHn4601NnRwWQwHQYD
VR0OBBYEFMXmOS18FM7ml8wlEekbhprNrKvtMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBaOVMG2hGvXQ+EQREf8HNRhL23
Ytuun2ehaZg4nt9QY+xpbAjT65w8Kmad6vt3fDgkL9uSWnL8KAJmdZzFsusKDj/5
j1F/0sqISK/zNXtTlzxbRWP/M6FIwLHIw5yf8ARkXgPz8nuiiKjlMyb8FX0jzlor
zttFh+QnAJIFBVAMeR2jSlB1w2F5J1yA0WpVTpps3tsNMbzix+7yJqXemhU6KQac
5lu89ZN5ZXneZNpMWWLFwA4ZOYiaarWh6w9jTdze7rR86lBw08jqBTD1KMhZsFrf
uBEEnguvPfdX87JM46ph6oPqLl/mG8SFX9Q2QcNlfogEqBDmGqThfWGvac0T
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAobPaGGB2+uwmrMtL2cG7hMaCQOWz3vk9sXKFG1V3u25lAtd8
MSug+ofysxQ/4lZOMWG4klf4ehu9n7KLyDcWyPEI+f74vYUFDkSkQ9EGTlyIHUt7
Xyjp+3VjOhKcutHXuf+W+b9IBwajv7/nmbEb8dhhuH4qDbgIKQjR0pQzSpJ/USkf
w4PpfFhlaFH8clLr98t9j6Hdtdc/GEmkidj8QUnN6TY5tPgH1ZIi7JLN5us0nMP9
DOx/MjclzA/X1ucY4qioIvX5O6CmGwQLXJWLW49mGxARkgJs5ABK5EBR2DmuEMgL
8Hvdteflz6Ww907+EOENfgHdXnOkUkWZ8TawLwIDAQABAoIBAA61NiwrcgcGvcnP
v/lxhqXgDG7stbydPQBL41GGJ5rTwrpMcU5jooPHI8soF1t18CzLr6cX+1DX/xps
egxe/mKOkftzNpq+Y6RVl/9XHtS30+WePP190jJ+DeV1Ve4kMhQH7GqQ/HMvJmvF
f8JZ+qm3vcOzLg4LZCqfU75snnG01nbp+qJyr/qg73XpqGdShHhKLu9YLIr78aod
8KrMU8Qj5oYKO1iJ9cKYHWNhyJ+W9Cln+fkrhxJ7PluA2Vq4kmz6GstOkqcKK/ZD
vF0UFZuWM84MX/5/pEwGq1qG/uKulHn/MsJsiLCTjDjrvjMVXuB3xlLc6zznH8js
B/dzoQkCgYEA088LVvwMtuFjmglcOHZmIWkbuE/D+qF2gXx4F7ydLax/DVAavyEh
KnAIAtMho8QTImDy3K/A/AiDDWvzWTVS/yfuLgk+JEe24hsI2oktT8IZ8rz2UpGo
MDePVfl407Uutw4Ek8B+OebNAu1TJKOiY0Qz6N5+c7O4Kq7b+HCwtmUCgYEAw3CT
Ta2ZMUKpzqQu/637UNtyFzXbDrSseB6mAyDvbNEEL3GbkdIu+g+GQtI7AnsopfOp
eJx+WCx7PevDIWoNgISjCdKKhpfjR94nI+g3dbghWI8CFMUEWqn+qV3x58wkub7L
Vsq2QlE4fA06EFmIiCsAFJOxXEaGMVvJcqCfCQMCgYBuiYzndl7ESlgKmXvxU4j1
Qr9ervIeKImKBgU+jgaRQYaE0htDR+zhEtp7bugNaVR7CpF0LpzOazrJlF7JXZTU
6GfayqQNlweQdH9oXVKyXx/fmFIMwUs+7p/1RF6UTef1THGDEmBNo2+2aViMtIEY
wQ0+KG9hvpcMX3kY7OasKQKBgAqNwraYIESjt+27ufn+mCqmnwOX2etyYDngVdCn
b6Ztpw800FTQh87jFbn4Bk28/tXvUYB82unCBSmz2IauZmpBP1ihcO+RtilEWtK6
kHwf5FUW6+LZPMH0mhblRtHgmT8aiXp3Wd486pLwV+lYHGJWCBwUWaiE+mwh67/v
4oanAoGBAJm2konx9KFSua7bV4Oxqidgoq3soS4gGccPyr1eUSc+QVXl+8nKVryS
T5bFC/wBDjR6xL8P4XFKNOf3WW7sVhZ4uuXVg/EzCHT4AeeVHoSYDlGnSYGZ30DJ
YC72mwSGT+G3ltnc+A3upLCXOgGM9TaXbxG+114r9fHbtoIco/xE
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);


StaticJsonDocument<JSON_OBJECT_SIZE(1)> inputDoc;

// PubSubClient callback function
void callback(const char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += String((char) payload[i]);
  }
  if (String(topic) == SUBCRIBE_TOPIC) {
    Serial.println("Message from topic " + String(SUBCRIBE_TOPIC) + ":" + message);

    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      String action = String(inputDoc["action"].as<char*>());
    
      if (action == "led_green_on")digitalWrite(led_green,HIGH);
      else if (action == "led_green_off")digitalWrite(led_green,LOW);

      if (action == "led_yellow_on")digitalWrite(led_yellow,HIGH);
      else if (action == "led_yellow_off")digitalWrite(led_yellow,LOW);

      if (action == "led_blue_on")digitalWrite(led_blue,HIGH);
      else if (action == "led_blue_off")digitalWrite(led_blue,LOW);

      if (action == "all_leds_on")on_all_leds();
      else if (action == "all_leds_off")off_all_leds();
      
      if (action == "Alarm_on" || action == "Alert" )  {sonido(); on_all_leds();}
      else if (action == "Alarm_off") {noTone();off_all_leds();}
      
      //if (action == "Alert" ) {sonido(); on_all_leds();}
      //else if (action == "Alert_off" || "Alarm_off") {noTone();off_all_leds();}
      
    }
  }
}

boolean mqttClientConnect() {
  Serial.println("Connecting to AWS " + String(AWS_ENDPOINT));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("Connected to " + String(AWS_ENDPOINT));

    mqttClient.subscribe(SUBCRIBE_TOPIC);

    Serial.println("Subscribed to " + String(SUBCRIBE_TOPIC));
  } else {
    Serial.println("Can't connect to " + String(AWS_ENDPOINT));
  }
  return mqttClient.connected();
}
char* FiletoString(File file){
   String str = "";
    while (file.available())
    {
      str += (char)file.read();
    }
    // Serial.println(str);
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    
    
    return cstr;
}
void setup() {
  pinMode(fotores, INPUT);
  pinMode(PIN_HECHO, INPUT);     
  pinMode(PIN_TRIG , OUTPUT);
  pinMode(led_blue, OUTPUT);            
  pinMode(led_yellow, OUTPUT);
  pinMode(led_green, OUTPUT);
  off_all_leds();


  Serial.begin(115200); 

  if (!SPIFFS.begin()) { Serial.println("Failed to mount file system"); return;}
  
  File id_file = SPIFFS.open("/id.txt", "r");
  File password_file = SPIFFS.open("/password.txt", "r");
  
  if (!id_file) { Serial.println("Failed to Wifi address file");}
 
  if (!password_file) { Serial.println("Failed to Password Wifi address file");}
  

   dht.begin();
   const char* ssid=FiletoString(id_file);
   const char* pass=FiletoString(password_file);
   
  Serial.println("Connecting to " + String(ssid));
  WiFi.begin(ssid, pass);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Can't connect to " + String(ssid));
    while (1) delay (200);
  }
  Serial.println("Connected to " + String(ssid));

  wifiClient.setCACert(AMAZON_ROOT_CA1);
  wifiClient.setCertificate(CERTIFICATE);
  wifiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(AWS_ENDPOINT, AWS_ENDPOINT_PORT);
  mqttClient.setCallback(callback);
}

unsigned long previousConnectMillis = 0;
unsigned long previousPublishMillis = 0;



StaticJsonDocument<JSON_OBJECT_SIZE(6)> outputDoc;
char outputBuffer[128];

void publishMessage(unsigned int id,unsigned int distancia,unsigned int sensorLuz,unsigned int temperatura,unsigned int humedad) {
  outputDoc["id"] = id;
  outputDoc["distancia"] = distancia;
  outputDoc["luminosidad"] = sensorLuz;
  outputDoc["temperatura"] = temperatura;
  outputDoc["humedad"] = humedad;
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(PUBLISH_TOPIC, outputBuffer);
}


void loop() {


  int sensordeLuz=analogRead(fotores);
  int humedad = dht.readHumidity();
  int temperatura = dht.readTemperature();
 
  iniciarTrigger();
  distancia=calcularDistancia();

  unsigned long now = millis();
  if (!mqttClient.connected()) {
    // Connect to MQTT broker
    if (now - previousConnectMillis >= 4000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) {
        previousConnectMillis = 0;
      } else delay(1000);
    }
  } else {
    // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    mqttClient.loop();
    delay(20);

    if (now - previousPublishMillis >= 8000) {
      previousPublishMillis = now;
      // Publish message
      publishMessage(id,distancia,sensordeLuz,temperatura,humedad);
      id++;
    }
  }
}
void off_all_leds()
{
  digitalWrite(led_blue, LOW);
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_green, LOW);
}
void on_all_leds()
{
  digitalWrite(led_blue, HIGH);
  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_green, HIGH);
}
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(PIN_TRIG, HIGH);   
  delayMicroseconds(10);
  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(PIN_TRIG, LOW);
}
float calcularDistancia()
{
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  long tiempo = pulseIn(PIN_HECHO, HIGH);
  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  float distancia = tiempo * 0.000001 * SONIDO / 2.0;
  delay(500);
  return distancia;
}
void tone(byte pin, int freq) {
  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(pin, 0); // attach beeper
  ledcWriteTone(0, freq); // play tone
}
void noTone() {
  tone(buzzer, 0);
}
void sonido(){
   int toneVal;
   float sinVal;
   for(int x=0; x<180; x++){
            // convertimos grados en radianes para luego obtener el valor.
             sinVal = (sin(x*(3.1412/180)));
            // generar una frecuencia a partir del valor sin
            toneVal = 2000+(int(sinVal*1000));
            tone(buzzer, toneVal);
            delay(2); 
     }  
}
