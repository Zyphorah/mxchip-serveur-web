#include <Arduino.h>
#include <AZ3166WiFi.h>

char ssid[] = "TELUS8223";
char password[] = "V956XHvC9bTh";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(2000);  // Attendre que le Serial soit prêt
  
  Serial.println("\n\n========================================");
  Serial.println("  Serveur HTTP MXChip AZ3166");
  Serial.println("========================================");
  
  // Initialiser le WiFi
  Serial.print("Connexion WiFi a: ");
  Serial.println(ssid);
  
  // Se connecter au WiFi
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println("");
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("*** CONNECTE AU WIFI! ***");
    Serial.print("*** IP: ");
    Serial.print(WiFi.localIP());
    Serial.println(" ***");
    Serial.println("");
    
    // Démarrer le serveur HTTP
    server.begin();
    Serial.println("Serveur HTTP demarre sur port 80");
    Serial.print("Accedez a: http://");
    Serial.println(WiFi.localIP());
    Serial.println("========================================");
    Serial.println("");
  } else {
    Serial.println("*** ERREUR: Impossible de connecter au WiFi ***");
    Serial.println("Verifiez:");
    Serial.print("  - SSID: ");
    Serial.println(ssid);
    Serial.println("  - Mot de passe");
    Serial.println("  - Signal WiFi");
    Serial.println("========================================");
  }
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    String line = "";
    String path = "";
    bool first = true;
    
    while (client.connected() && client.available()) {
      char c = client.read();
      if (c == '\n') {
        if (line.length() == 0) break;
        if (first) {
          int s1 = line.indexOf(' ');
          int s2 = line.indexOf(' ', s1+1);
          if (s1>0 && s2>s1) path = line.substring(s1+1, s2);
          first = false;
        }
        line = "";
      } else if (c != '\r') {
        line += c;
      }
    }
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    if (path == "/" || path == "") {
      client.println("<html><head><title>MXChip</title></head><body>");
      client.println("<h1>Serveur MXChip AZ3166</h1>");
      client.println("<p><a href='/hello'>Hello</a> | <a href='/info'>Info</a></p>");
      client.println("</body></html>");
    } else if (path == "/hello") {
      client.println("Hello from MXChip!");
    } else if (path == "/info") {
      client.println("<html><body><h1>Info</h1><p>IP: ");
      client.print(WiFi.localIP());
      client.println("</p></body></html>");
    } else {
      client.println("404");
    }
    
    client.stop();
  }
}
