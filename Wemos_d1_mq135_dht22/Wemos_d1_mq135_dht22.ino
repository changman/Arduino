/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define HAVE_DHT_22
#define HAVE_MQ135

#ifdef HAVE_DHT_22
#include "DHT.h"

#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#endif

#ifdef HAVE_MQ135
#define RLOAD 10.0

/// Calibration resistance at atmospheric CO2 level
#define RZERO 76.63

/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

#endif


#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

#ifdef HAVE_DHT_22
DHT dht(DHTPIN, DHTTYPE);
#endif

unsigned long g_lastConnectionTime = 0; 
const unsigned long g_sensingInterval = 10000; 
static unsigned long g_etimer;

float g_hum=0.0f, g_temp=0.0,g_co2=0.0f;

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("jungmin", "akdntm78");

    g_etimer = millis();

#ifdef HAVE_DHT_22
  USE_SERIAL.println("DHTxx test!");
  dht.begin();
 #endif

}

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {
      
      if ( millis() - g_etimer >= (g_sensingInterval)) {
          USE_SERIAL.println("Try Request Http!");
          g_etimer = millis() ;
      
#ifdef HAVE_DHT_22
  readFromDHT22();
#endif

#ifdef HAVE_MQ135
  readFromMQ135();
#endif  

      httpRequest(g_temp, g_hum,g_co2);
      }
    }

    delay(1000);
}

#ifdef HAVE_MQ135
void readFromMQ135()
{
   int val = analogRead(0);
   val = (1023./(float)val) * 5. - 1.* RLOAD;
   float Resistance;
   Resistance = val;
  
   float PPM;
   PPM = PARA * pow((Resistance/RZERO), -PARB);
//   USE_SERIAL.println("");
//   USE_SERIAL.print("A0: ");
//   USE_SERIAL.print(val);
//   USE_SERIAL.print("\t");
 //  USE_SERIAL.print("PPM: ");
 //  USE_SERIAL.println(PPM);

  g_co2 = PPM;
}
#endif

#ifdef HAVE_DHT_22
void readFromDHT22()
{
     float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    g_temp = t;
    g_hum = h;

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      USE_SERIAL.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);


 //   USE_SERIAL.print("Humidity: ");
 //   USE_SERIAL.print(h);
 //   USE_SERIAL.print(" %\t");
 //   USE_SERIAL.print("Temperature: ");
 //   USE_SERIAL.print(t);
 //   USE_SERIAL.print(" *C ");
 //   USE_SERIAL.print(f);
 //   USE_SERIAL.print(" *F\t");
 //   USE_SERIAL.print("Heat index: ");
 //   USE_SERIAL.print(hic);
 //   USE_SERIAL.print(" *C ");
 //   USE_SERIAL.print(hif);
 //   USE_SERIAL.println(" *F"); 
}
#endif

void httpRequest(float temp, float humi,float co2)
{
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
 //       http.begin("http://uangeltest.cafe24.com:5910/monitor/memdb/GAME_SCORE_STAT"); //HTTP
        String url( "http://52.237.75.237/cityfarm/insert.php?");
        url+="temp=";
        url+=temp;
        url+="&";
        url+="humi=";
        url+=humi;
        url+="&";
        url+="co2=";
        url+=co2;
        url+="&";
        url+="id=\"myhome\"";
           
        http.begin(url);
         
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
  
}

