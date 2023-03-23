#include <WiFi.h>
#include <Adafruit_BMP085.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "SAMAR"
#define WLAN_PASS       "menal@123#"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883                   // use 8883 for SSL
#define AIO_USERNAME  "menalkour2803"
#define AIO_KEY       "aio_mGuq655LrPhIC0lxaFCwF5v0a3nA"




WiFiClient client;



Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'temperature' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>

Adafruit_MQTT_Publish Weather = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/Feeds/Weather");

Adafruit_MQTT_Publish Pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/Feeds/Pressure");

// Setup a feed called 'sw1' for subscribing to changes.
Adafruit_MQTT_Subscribe AC = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/Feeds/Ac");

Adafruit_MQTT_Subscribe lamp = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/Feeds/lamp");

/*************************** Sketch Code ************************************/



void MQTT_connect();



const int led1 =2;
const int led2 = 4;

float p;
float q;

String stringOne, stringTwo;

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for sw1 feed.
  mqtt.subscribe(&AC);

  // Setup MQTT subscription for sw2 feed.
  mqtt.subscribe(&lamp);

  if (!bmp.begin()) {
    Serial.println("BMP180 Sensor not found ! ! !");
    while (1) {}
  }
}

uint32_t x = 0;

void loop() {


  p = bmp.readPressure();
  q = bmp.readTemperature();
  Serial.println(p);
  Serial.println(q);
  delay(100);

  MQTT_connect();



  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &AC) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)sw1.lastread);
      //digitalWrite(led1, HIGH);

      stringOne = (char *)AC.lastread;
      Serial.print(F("stringOne: "));
      Serial.println(stringOne);

      if (stringOne == "ON") {
        digitalWrite(led1, HIGH);
      }
      if (stringOne == "OFF") {
        digitalWrite(led1, LOW);
      }
    }

    if (subscription == &lamp) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)sw2.lastread);
      //digitalWrite(led2, HIGH);

      stringTwo = (char *)lamp.lastread;
      Serial.print(F("stringTwo: "));
      Serial.println(stringTwo);

      if (stringTwo == "ON") {
        digitalWrite(led2, HIGH);
      }
      if (stringTwo == "OFF") {
        digitalWrite(led2, LOW);
      }

    }
  }


  if (! Weather.publish(q)) {
    //Serial.println(F("Temp Failed"));
  } else {
    //Serial.println(F("Temp OK!"));
  }

  if (! Pressure.publish(p)) {
    //Serial.println(F("pressure Level Failed"));
  } else {
    //Serial.println(F("pressure Level OK!"));
  }


}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  //Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    //Serial.println(mqtt.connectErrorString(ret));
    //Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }

}