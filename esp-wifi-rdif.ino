#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 4  

WiFiServer server(80);
MFRC522 mfrc522(SS_PIN, RST_PIN);

String header;

String stav = "off";
String farba = "ff0000";

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);

  SPI.begin(18, 19, 23, SS_PIN);
  mfrc522.PCD_Init();  

  WiFi.begin("soc2019", "socka2019");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi pripojena!");
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // Handle client connections
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();

      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send HTTP response headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println("");

            if (header.indexOf("GET /on") >= 0) {
              Serial.println("STATUS: on");
              stav = "on";
            } else if (header.indexOf("GET /off") >= 0) {
              Serial.println("STATUS: off");
              stav = "off";
            }

            // Send HTML content
            client.println("<!DOCTYPE html><html><head>");
            client.println("<style>");
            client.println(".button {");
            client.println("padding: 24px 48px;");
            client.println("font-size: 18px;");
            String idk = "background-color: #" + farba + ";";
            client.println(idk);
            client.println("color: white;");
            client.println("border: none;");
            client.println("cursor: pointer;");
            client.println("}");
            client.println("</style>");
            client.println("</head><body>");
            client.println("<div align='center'>");

            client.println("<h1>LED status:</h1><br>");
            client.println("<p>STAV: <b>" + stav + "<b></p>");
            if (stav == "off") {
              client.println("<p><a href=\"/on\"><button class=\"button\">ZASVIETIT</button></a></p>");
              digitalWrite(2, LOW);
            } else {
              client.println("<p><a href=\"/off\"><button class=\"button button2\">ZHASNUT</button></a></p>");
              digitalWrite(2, HIGH);
            }

            client.println("</div>");
            client.println("</body></html>");
            client.println("");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }

  // Handle RFID logic separately
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      String uidString = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
        uidString += String(mfrc522.uid.uidByte[i], HEX);
      }
      uidString.toLowerCase();

      if (uidString == "23b7fca5") {
        farba = "00ff00";
      } else if (uidString == "63930b10") {
        farba = "ff0000";
      } else {
        farba = "ff0000";
      }
      Serial.println(uidString);
    }
  }
}
