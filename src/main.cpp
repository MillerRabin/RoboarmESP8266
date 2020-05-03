#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "Servo.h"

const char* ssid = "DragonSlayer";
const char* password = "ifyouwanttohave";

ESP8266WebServer server(80);

const int led = 13;

#define RotatePin 5
#define upPin 4
#define holdPin 12
#define fwdPin 14
Servo rotateServo;
Servo upServo;
Servo holdServo;
Servo fwdServo;

void setEngine(String name, unsigned int value) {
  if (name == "rotate") {
    rotateServo.write(value);
    return;
  }
  if (name == "up") {
    upServo.write(value);
    return;
  }
  if (name == "fwd") {
    fwdServo.write(value);
    return;
  }
  if (name == "hold") {
    holdServo.write(value);
    return;
  }
}

void handleRoot() {
  digitalWrite(led, 1);
  for (uint8_t i = 0; i < server.args(); i++) {
    unsigned int val = server.arg(i).toInt();
    setEngine(server.argName(i), val);    
  }
  server.send(200, "text/plain", "success");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setupEngines() {
  rotateServo.attach(RotatePin);
  holdServo.attach(holdPin);
  upServo.attach(upPin);
  fwdServo.attach(fwdPin);
}

void setup(void){  
  setupEngines();
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  MDNS.addService("http", "tcp", 80);
  if (!MDNS.begin("roboarm")) {
    Serial.println("MDNS responder not started");
  } else {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
