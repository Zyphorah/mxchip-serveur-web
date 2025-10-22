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

// Fonction pour déterminer le Content-Type selon l'extension
String getContentType(String path) {
  if (path.endsWith(".html")) return "text/html";
  if (path.endsWith(".htm")) return "text/html";
  if (path.endsWith(".css")) return "text/css";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png")) return "image/png";
  if (path.endsWith(".jpg")) return "image/jpeg";
  if (path.endsWith(".jpeg")) return "image/jpeg";
  if (path.endsWith(".gif")) return "image/gif";
  if (path.endsWith(".svg")) return "image/svg+xml";
  if (path.endsWith(".ico")) return "image/x-icon";
  if (path.endsWith(".woff")) return "font/woff";
  if (path.endsWith(".woff2")) return "font/woff2";
  if (path.endsWith(".ttf")) return "font/ttf";
  return "text/plain";
}

// Contenu de votre app (à remplacer par votre build/web/index.html)
const char APP[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Web App</title>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .container {
      background: white;
      border-radius: 20px;
      padding: 40px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      max-width: 600px;
      width: 90%;
      text-align: center;
    }
    h1 {
      color: #667eea;
      font-size: 2.5em;
      margin: 0 0 20px 0;
    }
    .status {
      background: #f0f4ff;
      border-left: 4px solid #667eea;
      padding: 20px;
      margin: 20px 0;
      border-radius: 5px;
      text-align: left;
    }
    .status strong {
      color: #667eea;
      display: block;
      margin-bottom: 10px;
    }
    .btn {
      display: inline-block;
      background: #667eea;
      color: white;
      padding: 15px 30px;
      text-decoration: none;
      border-radius: 50px;
      margin: 10px;
      transition: transform 0.3s, box-shadow 0.3s;
      font-weight: bold;
    }
    .btn:hover {
      transform: translateY(-3px);
      box-shadow: 0 10px 25px rgba(102, 126, 234, 0.4);
    }
    .api-demo {
      margin-top: 30px;
      padding-top: 30px;
      border-top: 2px solid #f0f0f0;
    }
    #apiResult {
      background: #f9f9f9;
      padding: 15px;
      border-radius: 10px;
      margin-top: 15px;
      font-family: monospace;
      font-size: 14px;
      text-align: left;
      display: none;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚀 Web sur MXChip</h1>
    <p style="color: #666; font-size: 1.1em;">
      Votre application web fonctionne sur un microcontrôleur STM32!
    </p>
    
    <div class="status">
      <strong>📡 Statut de connexion</strong>
      <p>✅ Connecté au WiFi<br>
      🌐 Serveur HTTP actif<br>
      ⚡ MXChip AZ3166 (STM32F412)</p>
    </div>

    <div>
      <a href="/info" class="btn">📊 Informations Système</a>
      <a href="/api/data" class="btn">🔌 API REST</a>
    </div>

    <div class="api-demo">
      <h3>Test API</h3>
      <button class="btn" onclick="testApi()">Tester l'API</button>
      <div id="apiResult"></div>
    </div>
  </div>

  <script>
    async function testApi() {
      const result = document.getElementById('apiResult');
      result.style.display = 'block';
      result.innerHTML = '⏳ Chargement...';
      
      try {
        const response = await fetch('/api/data');
        const data = await response.text();
        result.innerHTML = '<strong>Réponse de l\'API:</strong><br>' + data;
      } catch(error) {
        result.innerHTML = '❌ Erreur: ' + error.message;
      }
    }
  </script>
</body>
</html>
)rawliteral";

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    String line = "";
    String path = "";
    bool first = true;
    
    // Lire la requête HTTP
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
    
    // Normaliser le path
    if (path == "") path = "/";
    
    Serial.print("Requete: ");
    Serial.println(path);
    
    // Router les requêtes
    if (path == "/" || path == "/index.html") {
      // Page principale app
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=utf-8");
      client.println("Connection: close");
      client.println("Cache-Control: no-cache");
      client.println();
      client.print(APP);
      
    } else if (path == "/info") {
      // Page d'informations système
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=utf-8");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE html><html><head><title>Info</title>");
      client.println("<meta name='viewport' content='width=device-width'>");
      client.println("<style>body{font-family:Arial;margin:20px;background:#f5f5f5}");
      client.println(".card{background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);margin:10px 0}");
      client.println("h1{color:#667eea}</style></head><body>");
      client.println("<h1>Informations Système</h1>");
      client.println("<div class='card'><strong>Device:</strong> MXChip AZ3166</div>");
      client.println("<div class='card'><strong>CPU:</strong> STM32F412ZG @ 100MHz</div>");
      client.println("<div class='card'><strong>RAM:</strong> 256 KB</div>");
      client.println("<div class='card'><strong>IP:</strong> ");
      client.print(WiFi.localIP());
      client.println("</div>");
      client.println("<div class='card'><strong>SSID:</strong> ");
      client.print(WiFi.SSID());
      client.println("</div>");
      client.println("<div class='card'><strong>Uptime:</strong> ");
      client.print(millis() / 1000);
      client.println(" secondes</div>");
      client.println("<a href='/' style='display:inline-block;margin-top:20px;padding:10px 20px;background:#667eea;color:white;text-decoration:none;border-radius:5px'>Retour</a>");
      client.println("</body></html>");
      
    } else if (path.startsWith("/api/data")) {
      // API REST JSON
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Access-Control-Allow-Origin: *");
      client.println("Connection: close");
      client.println();
      client.print("{");
      client.print("\"status\":\"ok\",");
      client.print("\"device\":\"MXChip AZ3166\",");
      client.print("\"ip\":\"");
      client.print(WiFi.localIP());
      client.print("\",");
      client.print("\"uptime\":");
      client.print(millis() / 1000);
      client.print(",");
      client.print("\"rssi\":");
      client.print(WiFi.RSSI());
      client.println("}");
      
    } else {
      // 404 Not Found
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE html><html><head><title>404</title>");
      client.println("<style>body{font-family:Arial;text-align:center;padding:50px;background:#f5f5f5}");
      client.println("h1{color:#e74c3c}</style></head><body>");
      client.println("<h1>404 - Page non trouvee</h1>");
      client.println("<p>La ressource demandee n'existe pas.</p>");
      client.println("<a href='/'>Retour a l'accueil</a>");
      client.println("</body></html>");
    }
    
    client.stop();
  }
}
