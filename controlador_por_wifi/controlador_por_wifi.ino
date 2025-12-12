#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

/*
Codigo para que o servidor web receba os comandos via POST e os envie para o robo.
Comandos como 'F200', 'S', 'U255' podem ser enviados pelo client do computador, qque serve como
controle remoto.


*/

// --- CONFIGURAÇÕES WIFI ---
const char *ssid = "Projeto";
const char *password = "2022-11-07";

WebServer server(80);

// --- PINOS MOTORES DE TRAÇÃO ---
// Motor 1 (Esquerdo)
#define PINO_IN1 22
#define PINO_IN2 23
#define PINO_PWM 18

// Motor 2 (Direito)
#define PINO_IN3 19
#define PINO_IN4 21
#define PINO_PWM2 17

// --- PINOS MOTOR LIFT (EMPILHADEIRA) ---
#define PINO_IN5 33     // Controle sentido A (Lift)
#define PINO_IN6 32     // Controle sentido B (Lift)
#define PINO_PWM_LIFT 4 // Velocidade (Lift)

// Variáveis globais
char comandoAtual = 'P'; // Começa Parado
int velocidadeAtual = 0;

// Forward declarations of helper functions
void moverFrente(int pwm);
void moverTras(int pwm);
void virarEsquerda(int pwm);
void virarDireita(int pwm);
void pararRodas();
void moverLiftCima(int pwm);
void moverLiftBaixo(int pwm);
void pararLift();
void pararTudo();
void executarComando();

// --- HANDLER DO SERVIDOR WEB ---
void handlePostCommand() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    body.trim(); // Remove whitespace

    Serial.println("Recebido via POST: " + body);

    if (body.length() > 0) {
      char cmd = body.charAt(0);

      // Tenta ler a velocidade se houver
      int novaVelocidade = 0;
      if (body.length() > 1) {
        novaVelocidade = body.substring(1).toInt();
      }

      comandoAtual = cmd;

      // Atualiza velocidade se foi enviado um valor maior que 0
      if (novaVelocidade > 0) {
        velocidadeAtual = novaVelocidade;
      }

      // Trava de segurança PWM
      if (velocidadeAtual > 255)
        velocidadeAtual = 255;
      if (velocidadeAtual < 0)
        velocidadeAtual = 0;

      executarComando();

      server.send(200, "text/plain",
                  "Comando Executado: " + String(comandoAtual) +
                      " Vel: " + String(velocidadeAtual));
    } else {
      server.send(400, "text/plain", "Comando vazio");
    }
  } else {
    server.send(400, "text/plain", "POST sem corpo");
  }
}

void setup() {
  Serial.begin(115200);

  // Configura Tração
  pinMode(PINO_IN1, OUTPUT);
  pinMode(PINO_IN2, OUTPUT);
  pinMode(PINO_PWM, OUTPUT);

  pinMode(PINO_IN3, OUTPUT);
  pinMode(PINO_IN4, OUTPUT);
  pinMode(PINO_PWM2, OUTPUT);

  // Configura Lift
  pinMode(PINO_IN5, OUTPUT);
  pinMode(PINO_IN6, OUTPUT);
  pinMode(PINO_PWM_LIFT, OUTPUT);

  pararTudo();

  // Conexão WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Rota POST
  server.on("/command", HTTP_POST, handlePostCommand);

  server.begin();
  Serial.println("HTTP server iniciado!");
  Serial.println("Envie POST para /command com corpo ex: 'F200', 'S', 'U255'");
}

void loop() { server.handleClient(); }

void executarComando() {
  switch (comandoAtual) {
  // --- COMANDOS DE TRAÇÃO ---
  case 'F':      // FRENTE
    pararLift(); // Garante que o lift pare
    moverFrente(velocidadeAtual);
    Serial.println("RODAS: Frente");
    break;

  case 'B': // RÉ
    pararLift();
    moverTras(velocidadeAtual);
    Serial.println("RODAS: Tras");
    break;

  case 'L': // ESQUERDA
    pararLift();
    virarEsquerda(velocidadeAtual);
    Serial.println("RODAS: Esq");
    break;

  case 'R': // DIREITA
    pararLift();
    virarDireita(velocidadeAtual);
    Serial.println("RODAS: Dir");
    break;

  // --- COMANDOS DO LIFT ---
  case 'U':       // UP (SUBIR)
    pararRodas(); // Garante que o carro pare
    moverLiftCima(velocidadeAtual);
    Serial.println("LIFT: Subindo");
    break;

  case 'D': // DOWN (DESCER)
    pararRodas();
    moverLiftBaixo(velocidadeAtual);
    Serial.println("LIFT: Descendo");
    break;

  // --- COMANDO DE PARADA ---
  case 'S':
  case 'P':
    Serial.println("PARADA TOTAL");
    pararTudo();
    velocidadeAtual = 0;
    break;

  default:
    // Comando desconhecido, ignora.
    Serial.println("Comando desconhecido: " + String(comandoAtual));
    break;
  }
}

// ==========================================
// --- FUNÇÕES AUXILIARES ---
// ==========================================

// --- Funções das Rodas ---
void moverFrente(int pwm) {
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
  analogWrite(PINO_PWM, pwm);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, HIGH);
  analogWrite(PINO_PWM2, pwm);
}

void moverTras(int pwm) {
  digitalWrite(PINO_IN1, LOW);
  digitalWrite(PINO_IN2, HIGH);
  analogWrite(PINO_PWM, pwm);
  digitalWrite(PINO_IN3, HIGH);
  digitalWrite(PINO_IN4, LOW);
  analogWrite(PINO_PWM2, pwm);
}

void virarEsquerda(int pwm) {
  digitalWrite(PINO_IN1, LOW);
  digitalWrite(PINO_IN2, HIGH);
  analogWrite(PINO_PWM, pwm);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, HIGH);
  analogWrite(PINO_PWM2, pwm);
}

void virarDireita(int pwm) {
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
  analogWrite(PINO_PWM, pwm);
  digitalWrite(PINO_IN3, HIGH);
  digitalWrite(PINO_IN4, LOW);
  analogWrite(PINO_PWM2, pwm);
}

void pararRodas() {
  digitalWrite(PINO_IN1, LOW);
  digitalWrite(PINO_IN2, LOW);
  analogWrite(PINO_PWM, 0);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, LOW);
  analogWrite(PINO_PWM2, 0);
}

// --- Funções do Lift ---
void moverLiftCima(int pwm) {
  digitalWrite(PINO_IN5, HIGH);
  digitalWrite(PINO_IN6, LOW);
  analogWrite(PINO_PWM_LIFT, pwm);
}

void moverLiftBaixo(int pwm) {
  digitalWrite(PINO_IN5, LOW);
  digitalWrite(PINO_IN6, HIGH);
  analogWrite(PINO_PWM_LIFT, pwm);
}

void pararLift() {
  digitalWrite(PINO_IN5, LOW);
  digitalWrite(PINO_IN6, LOW);
  analogWrite(PINO_PWM_LIFT, 0);
}

// --- Parada Geral ---
void pararTudo() {
  pararRodas();
  pararLift();
}
