//Update 12/12/17

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>


#define DHTPIN D3     // what digital pin we're connected to
#define DHTTYPE DHT11





DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
Adafruit_BMP085 bmp;
ADC_MODE(ADC_VCC);
const int FW_VERSION = 0002;
const char* fwUrlBase = "http://192.168.1.68/fota/";
#define ssid "PLUSNET-7P2C"
#define password "P050778J310382H"
const char* server = "api.thingspeak.com";
#define sleepTimeS 10
String api = "Q52D9IJR6I7OC8NF";
void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
    dht.begin();
    WiFi.begin(ssid, password);
  }
}

//check for updates



  
void loop() {
  
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String fwURL = String( fwUrlBase );
  fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );

  Serial.println( "Checking for firmware updates." );
  Serial.print( "MAC address: " );
  Serial.println( mac );
  Serial.print( "Firmware version URL: " );
  Serial.println( fwVersionURL );

  HTTPClient httpClient;
  httpClient.begin(fwVersionURL);
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version: " );
    Serial.println( FW_VERSION );
    Serial.print( "Available firmware version: " );
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      Serial.println( "Preparing to update" );

      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    }
    else {
      Serial.println( "Already on latest version" );
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  httpClient.end();
  
  
  
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  float y = bmp.readTemperature();
  float j = bmp.readPressure()/100;
  float v = ESP.getVcc()/1000.0;

    
  if (isnan(h) || isnan(t) ) {
    //Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hic = dht.computeHeatIndex(t, h, false);
  Serial.println(WiFi.localIP());
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Temperature = ");
  Serial.print(y);
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(j);
  Serial.println(" mb");
  Serial.print(v); 
  Serial.print(" v");
  

  if (client.connect(server, 80)){
    String postStr = api;
      postStr += "&field1=";
      postStr += String(t);
      postStr += "&field2=";
      postStr += String(h);
      postStr += "&field3=";
      postStr += String(y);
      postStr += "&field4=";
      postStr += String(j);
      postStr += "&field5=";  
      postStr += String(v);
      client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+api+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     client.stop();
     delay(1000);
     
    
 // check for new firmware

     
     
  //const int FW_VERSION = 0001;
  //const char* fwUrlBase = "http://192.168.1.68/fota/";
  }

 // go to deepsleep for 15 minutes
  

  Serial.println();
  //ESP.deepSleep(sleepTimeS * 60 * 1000000,WAKE_RF_DEFAULT);
 // system_deep_sleep_set_option(0);
 // system_deep_sleep(15 * 60 * 1000000);
  //delay(120000);
}
