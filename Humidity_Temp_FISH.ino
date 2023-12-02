#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#include "DHT.h"
#define DHTPIN 2     ///D4 of the nodemcu 
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// Insert your network credentials
#define WIFI_SSID "power1"
#define WIFI_PASSWORD "INTENSITY"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAvuiuZRmjoyXgLwrMWyCS2l56H3W31WHg"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "fishtank-ef40d-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
uint32_t idleTimeForStream = 15000;
static int Dev_stat=0;

int count = 0;
void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  pinMode(D4, INPUT);
  dht.begin();
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
 
   /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
    

}

void loop()
{

if (Firebase.ready())
  {
     
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.print("humidity = ");
    Serial.println(h);
    Serial.print("temperature = ");
    Serial.println(t);
    Firebase.RTDB.setFloat(&fbdo, "Tank1/humidity", h);
    Firebase.RTDB.setFloat(&fbdo, "Tank1/temperature", t);
    Dev_stat = 0;
    float Sensor_value = analogRead(A0);
    Sensor_value = Sensor_value;
    Sensor_value = 1000 - Sensor_value;
    
    Sensor_value = abs (Sensor_value);
    Serial.println(Sensor_value);

    if(Sensor_value > 500)
    {
      Firebase.RTDB.setFloat(&fbdo, "Tank1/water", 2);
    }
    else
    {
      Firebase.RTDB.setFloat(&fbdo, "Tank1/water", 1);
    }
        
    delay(5000);

    
    if (fbdo.streamTimeout())
    {
      Serial.println("stream timed out, resuming...\n");

      if (!fbdo.httpConnected())
        Serial.printf("error code: %d, reason: %s\n\n", fbdo.httpCode(), fbdo.errorReason().c_str());
    }
        
  }
  
}


