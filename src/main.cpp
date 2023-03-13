#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <IOT.h>

byte ledStatus;
String path;

FirebaseData fbdos;
FirebaseAuth auth;
FirebaseConfig config;

void initWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
void initgpio()
{
  pinMode(led, OUTPUT);
}
void initFirebase()
{
  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  if (!Firebase.RTDB.beginStream(&fbdos, "/Output/"))
  {
    Serial.println("Stream begin failed. ");
    Serial.println(fbdos.errorReason().c_str());
  }
  else
  {
    Serial.println("Stream begin Successfull.");
  }
}
void readData()
{
  if (Firebase.ready())
  {
    if (!Firebase.RTDB.readStream(&fbdos))
    {
      Serial.println(fbdos.errorReason().c_str());
    }
    if (fbdos.streamAvailable())
    {
      if (fbdos.dataType() == "int")
      {
        path = fbdos.dataPath();
        if (path == "/output0")
        {
          ledStatus = fbdos.intData();
          digitalWrite(led, ledStatus);
        }
        if (path == "/output1")
        {
          ledStatus = fbdos.intData();
          digitalWrite(led, !ledStatus);
        }
      }
    }
  }
}
void setup()
{
  Serial.begin(115200);
  initWiFi();
  initFirebase();
  initgpio();
}

void loop()
{
  readData();
}
