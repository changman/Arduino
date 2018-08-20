/*
 WiFiEsp example: WebClient

 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "WiFiEsp.h"

#define WIFIRX 2
#define WIFITX 3

#define HAVE_DHT_22

#ifdef HAVE_DHT_22
#include "DHT.h"

#define DHTPIN 8     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#endif

#define HAVE_MQ135

#ifdef HAVE_MQ135
#define RLOAD 10.0

/// Calibration resistance at atmospheric CO2 level
#define RZERO 76.63

/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

#endif

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(WIFIRX, WIFITX); // RX, TX
#endif

const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

char ssid[] = "jungmin";            // your network SSID (name)
char pass[] = "akdntm78";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//char server[] = "192.168.0.17";
char server[] = "52.237.75.237";

unsigned long lastConnectionTime = 0; 
const unsigned long sensingInterval = 2000; 
static unsigned long _ETimer;


float g_hum=0.0f, g_temp=0.0,g_co2=0.0f;

// Initialize the Ethernet client object
WiFiEspClient client;

#ifdef HAVE_DHT_22
DHT dht(DHTPIN, DHTTYPE);
#endif

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

 #ifdef HAVE_DHT_22
  Serial.println("DHTxx test!");
  _ETimer = millis();
  
  dht.begin();
 #endif

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  printWifiStatus();
/*
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 9999)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /cityfarm/insert.php?temp=999.2&humi=888.1 HTTP/1.1");
    client.println("Host: arduino.cc");
    client.println("Connection: close");
    //client.println("{\"temp\":789.2,\"humi\":432.1}");
    client.println();
  }
   */
}

void loop()
{
 // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }


 if ( millis() - _ETimer >= (sensingInterval)) {
    _ETimer = millis() ;
    
#ifdef HAVE_DHT_22

  readFromDHT22();

#endif

#ifdef HAVE_MQ135
  readFromMQ135();
#endif
 
 }

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    lastConnectionTime = millis() ;
    Serial.println("Starting connection to server...");
    httpRequest(g_temp,g_hum,g_co2);
  }
}

void readFromMQ135()
{
   int val = analogRead(0);
   val = (1023./(float)val) * 5. - 1.* RLOAD;
   float Resistance;
   Resistance = val;
  
   float PPM;
   PPM = PARA * pow((Resistance/RZERO), -PARB);

  g_co2 = PPM;
}

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
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);


//    Serial.print("Humidity: ");
//    Serial.print(h);
//    Serial.print(" %\t");
//    Serial.print("Temperature: ");
//    Serial.print(t);
//    Serial.print(" *C ");
//    Serial.print(f);
//    Serial.print(" *F\t");
//    Serial.print("Heat index: ");
//    Serial.print(hic);
//    Serial.print(" *C ");
//    Serial.print(hif);
//    Serial.println(" *F"); 
}

// this method makes a HTTP connection to the server
void httpRequest(float temp, float humi,float co2)
{
   Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    char buf[256]={0};
  //  sprintf(buf,"GET /cityfarm/insert.php?temp=%0.1f&humi=%0.1f HTTP/1.1",4444.2,5555.1);
  //  Serial.println("GET /cityfarm/insert.php?);
    // send the HTTP PUT request
    client.print("GET /cityfarm/insert.php?temp=");
    client.print(temp); // 온도
    client.print("&humi=");
    client.print(humi); // 습도
    client.print("&co2=");
    client.print(co2); // 습도
    client.print("&id=");
    client.print("\"myhome\""); // ID
    client.println(" HTTP/1.1");
    client.println(F("Host: citifarm.cc"));
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
