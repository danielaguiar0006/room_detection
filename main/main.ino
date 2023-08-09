#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SENSOR_PIN D1 // Change this to the pin you connected the sensor's OUT pin

const char* ssid = "SpectrumSetup-C0";
const char* password =  "loudtree621";

const char* serverName = "http://192.168.1.110:8080/motion_detected";

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  pinMode(SENSOR_PIN, INPUT);   // Initialize the SENSOR_PIN as an input
  connectToWiFi(ssid, password);
}

void loop() {
  int sensorValue = digitalRead(SENSOR_PIN);
  
  if(sensorValue == HIGH) {     // Check if the sensor is detecting a motion
    Serial.println("Motion detected!");
    sendHTTPRequest();
    delay(1000);                // Delay to avoid multiple notifications for the same motion event
  }
}


void connectToWiFi(const char* ssid, const char* password) {
  int ledStatus = 0;

  WiFi.begin(ssid, password); // begin WiFi connection
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // Print the IP address
}

void sendHTTPRequest() {
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    HTTPClient http;    //Declare object of class HTTPClient
    WiFiClient wifiClient;

    http.begin(wifiClient, serverName);      //Specify request destination
    int httpCode = http.POST("");  //Send the request

    //Check the returning code
    if (httpCode > 0) { 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);    //Print the response payload
    } else {
      Serial.println("HTTP request failed");
    }
    http.end();   //Close connection
  } else {
    Serial.println("WiFi not connected");
  }
}
