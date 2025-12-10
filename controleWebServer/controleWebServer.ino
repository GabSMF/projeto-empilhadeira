#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "Projeto";
const char *password = "2022-11-07";

WebServer server(80);  // porta 80

void handlePostCommand() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.println("Recebido via POST:");
    Serial.println(body); // MARK: COMANDO!!!!

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "POST sem corpo");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.println(WiFi.localIP());

  // Rota POST
  server.on("/command", HTTP_POST, handlePostCommand);

  server.begin();
  Serial.println("HTTP server iniciado!");
}

void loop() {
  server.handleClient();
}
