char watt[5];
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID       "Device"
#define WLAN_PASS       "12345678"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME  "xxxxxxxxxxxxxxxxxxxxxxx"
#define AIO_KEY       "XXXXXXXXXXXXXXXXXXXXXXX"
float  bill_amount = 0;   
unsigned int energyTariff =200; 
WiFiClient client; 

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Power = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/power");
Adafruit_MQTT_Publish bill = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bill");
Adafruit_MQTT_Subscribe switchhall = Adafruit_MQTT_Subscribe(&mqtt,AIO_USERNAME "/feeds/switch");
void MQTT_connect();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
    mqtt.subscribe(&switchhall);
  Serial.println(F("Adafruit MQTT demo"));
pinMode(2, OUTPUT);
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
}

void loop() {
 int i=0;  MQTT_connect();
 float watt1;
 if(Serial.available() > 0 ){
   delay(100); //allows all serial sent to be received together
    while(Serial.available() && i<5) {
     watt[i++] = Serial.read();
     
    }
    watt[i++]='\0';
  }
     Adafruit_MQTT_Subscribe *subscription;
  subscription = mqtt.readSubscription(5000); 
    // Check if its the onoff button feed
    if (subscription == &switchhall) {
      Serial.print(F("On-Off button: "));
      Serial.println((char *)switchhall.lastread);
      
      if (strcmp((char *)switchhall.lastread, "ON") == 0) {
        digitalWrite(2, HIGH); 
      }
      if (strcmp((char *)switchhall.lastread, "OFF") == 0) {
        digitalWrite(2,LOW); 
      }
    }
  


   watt1 = atof(watt);
  Serial.println(  watt1);
    bill_amount = watt1 *(energyTariff/100);    
       if (! bill.publish(bill_amount)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! Power.publish(watt1)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
    
  }
  delay(2000);
  }
  void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
