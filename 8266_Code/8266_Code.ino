#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Ticker.h>  //Ticker Library

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleNotFound();

boolean bellrung = false;
float timer = 0;
float delayValue = 2; // 2 seconds to switch bellrung from true to false
char* wifiName = "WIFI NAME";
char* wifiPW = "WIFI PASS";


const int inputPin = 16; //D0

void setup(void){
  pinMode(inputPin, INPUT);
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP(wifiName, wifiPW);   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
  
  timer = 0;
  bellrung = false;
  delayValue *= 1000; // convert ms to sec
}

void loop(void){
  unsigned long currentMs = millis();
  
  server.handleClient();                    // Listen for HTTP requests from clients
  if (digitalRead(inputPin) == HIGH) {
    bellrung = true;
    timer = currentMs; // reset the timer
  }

  if (bellrung) {
    if (currentMs - timer >= delayValue) { // Check if the timer is our delayValue
      timer = currentMs; // reset timer
      bellrung = false;
    } // Else keep the condition to true
  }
  Serial.println(bellrung);
}

void handleRoot() {
  server.send(200, "text/plain", bellrung? "1":"0");   // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
