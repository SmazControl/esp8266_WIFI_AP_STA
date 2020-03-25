#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// Set WiFi credentials
#define WIFI_SSID "phloenlom_2.4GHz"
#define WIFI_PASS "248248248"
 
// Set AP credentials
#define AP_SSID "ESP8266"
#define AP_PASS "magicword"
#define AP_CHANNEL 1
#define AP_HIDDEN false
#define AP_MAX_CON 8

// Set IP addresses
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Create a new web server
ESP8266WebServer webserver(80);

String web_buff;
// Handle Root
void rootPage() { 
  webserver.send(200, "text/html", web_buff); 
}
 
// Handle 404
void notfoundPage(){ 
  webserver.send(404, "text/plain", "404: Not found"); 
}

void setup()
{
  // Setup serial port
  Serial.begin(115200);
  Serial.println();
 
  // Begin Access Point
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL, AP_HIDDEN, AP_MAX_CON);
 
  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
 
  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
 
  // Connected to WiFi
  Serial.println();
  Serial.println("Connected!");
  Serial.print("IP address for network ");
  Serial.print(WIFI_SSID);
  Serial.print(" : ");
  Serial.println(WiFi.localIP());
  Serial.print("IP address for network ");
  Serial.print(AP_SSID);
  Serial.print(" : ");
  Serial.print(WiFi.softAPIP());
// Start Web Server
  webserver.on("/", rootPage);
  webserver.onNotFound(notfoundPage);
  webserver.begin();

    WiFiClient client;
    HTTPClient http; //must be declared after WiFiClient for correct destruction order, because used by http.begin(client,...)

    Serial.print("[HTTP] begin...\n");

    // configure server and url
    http.begin(client, "http://jigsaw.w3.org/HTTP/connection.html");
    //http.begin(client, "jigsaw.w3.org", 80, "/HTTP/connection.html");

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // create buffer for read
        uint8_t buff[128] = { 0 };

        web_buff = http.getString();
        // with API
        Serial.println(web_buff);
        // or "by hand"

        // get tcp stream
        WiFiClient * stream = &client;

        // read all data from server
        while (http.connected() && (len > 0 || len == -1)) {
          // read up to 128 byte
          int c = stream->readBytes(buff, std::min((size_t)len, sizeof(buff)));
          Serial.printf("readBytes: %d\n", c);
          if (!c) {
            Serial.println("read timeout");
          }

          // write it to Serial
          Serial.write(buff, c);

          if (len > 0) {
            len -= c;
          }
        }

        Serial.println();
        Serial.print("[HTTP] connection closed or file end.\n");
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    Serial.println("");
    Serial.println("Web Server begin");  
}
 
void loop() {
  // put your main code here, to run repeatedly:
  webserver.handleClient(); 
}
