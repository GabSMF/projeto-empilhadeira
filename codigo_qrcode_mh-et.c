void setup() {
  Serial.begin(115200); 
  delay(1000);
  
  Serial1.begin(9600); // Conectado nos pinos 18 e 19
  delay(500);
  
  Serial.println("=== TESTE MH-ET LIVE ===");
  Serial.println("Enviando comandos de configuração...");
  
  // Comandos de configuração
  Serial1.println("M00910001."); // habilita comandos serial
  delay(100);
  Serial1.println("M00210001."); // escaneamento contínuo
  delay(100);
  Serial1.println("~M00B00014."); // atraso para ler mesmo produto
  delay(100);
  
  Serial.println("Configuração enviada. Aguardando leituras...");
  Serial.println("Aproxime um código de barras do leitor.");
}

void loop() {
  // Verifica dados do MH-ET
  if (Serial1.available() > 0) {
    Serial.println("Entrou - dados disponíveis");
    
    // MUDANÇA: Usa readString() em vez de readStringUntil()
    delay(50); // Pequena pausa para garantir que todos os dados chegaram
    String texto = Serial1.readString();
    texto.trim();
    
    Serial.println("Dados brutos recebidos: '" + texto + "'");
    Serial.println("Tamanho: " + String(texto.length()));
    
    if (texto.length() > 3) {
      Serial.println("====================");
      Serial.println("Código lido: " + texto);
      Serial.println("====================");
    } else {
      Serial.println("Dados muito curtos, ignorando.");
    }
  }
  
  // Permite enviar comandos manuais via Serial Monitor
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    Serial1.println(comando);
    Serial.println("Comando enviado: " + comando);
  }
}
