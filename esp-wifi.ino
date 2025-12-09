#include <WiFi.h>

WiFiServer server(80);

String header;
String stav = "zatvorena";

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);

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
  WiFiClient client = server.available();  // aktualizovanie prijatych dat

  if (client)  // ak sa pripojil klient...
  {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Novy klient");
    String currentLine = "";  // premenna pre prijate data

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available())  // test ci su prijate dake data
      {
        char c = client.read();  // nacitanie bajtu z buffera
        Serial.write(c);
        header += c;

        if (c == '\n')  // po ENTER je request
        {
          if (currentLine.length() == 0)  // ak sa nacital prazdny riadok, odosli potvrdenie o spojeni
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println("");



            if (header.indexOf("GET /otvoreny") >= 0) {
              Serial.println("STATUS: OTVORENY");
              stav = "otvorena";
            } else if (header.indexOf("GET /zatvoreny") >= 0) {
              Serial.println("STATUS: ZATVORENY");
              stav = "zatvorena";
            }

            client.println("<!DOCTYPE html><html><head>");
            client.println("<style>"); // integracia CSS 
            client.println(".button {");
            client.println("padding: 24px 48px;");
            client.println("font-size: 18px;");
            client.println("background-color: #007bff;");
            client.println("color: white;");
            client.println("border: none;");
            client.println("cursor: pointer;");
            client.println("}");
            client.println("</style>");
            client.println("</head><body>");
            client.println("<div align='center'>");

            client.println("<h1>Otvaranie brany</h1><br>");

            client.println("<p>STAV BRANY: <b>" + stav + "<b></p>");
            if (stav == "zatvorena")
              client.println("<p><a href=\"/otvoreny\"><button class=\"button\">OTVORIT</button></a></p>");
            else
              client.println("<p><a href=\"/zatvoreny\"><button class=\"button button2\">ZATVORIT</button></a></p>");

            client.println("</div>");
            client.println("</body></html>");
            client.println("");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;  // add it to the end of the currentLine
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Klient je odpojeny.");
    Serial.println("");
  }
}
