
//Include the SoftwareSerial library
#include "SoftwareSerial.h"

// Define os pinos de controle do motor ligados ao Arduino
#define PINO_IN1 5  // Pino responsável pelo controle no sentido horário
#define PINO_IN2 6  // Pino responsável pelo controle no sentido anti-horário
#define PINO_PWM 3


//Create a new software  serial
SoftwareSerial bluetooth(2, 3); //TX, RX (Bluetooth)

void setup(){ 
  //Initialize the hardware serial
  Serial.begin(9600);

  //Initialize the software serial
  bluetooth.begin(38400);
  
  // Configura os pinos de controle como saída
  pinMode(PINO_IN1, OUTPUT);  // Define o pino IN1 como saída
  pinMode(PINO_IN2, OUTPUT);  // Define o pino IN2 como saída
  pinMode(PINO_PWM, OUTPUT);  // Define o pino IN2 como saída
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
}

void loop() {   
  int valor_pwm = 0;  // Variável para armazenar o valor PWM (0-255)

  //Check received a byte from bluetooth by software serial
  if (bluetooth.available()) {
    // if vai pra frente
    char r = bluetooth.read(); //Read and save the byte
    if (r == 'a') {
      if (valor_pwm == 255) {
        bluetooth.print("Velocidade máxima atingida, não é possível aumentar mais");
      } else {
        valor_pwm += 15;
        bluetooth.print("Velocidade aumentada em 15, valor atual:");
        bluetooth.println(valor_pwm);

        delay(1000);
      }
    } else if (r == 'd') {
      if (valor_pwm == 0) {
        bluetooth.print("Velocidade zerada, não é possível diminuir mais...");
      } else {
        valor_pwm -= 15;
        bluetooth.print("Velocidade reduzida em 15, valor atual:");
        bluetooth.println(valor_pwm);
      }
    } else {
        bluetooth.print("Comando desconhecido, use 'a' para aumentar a velocidade e 'd' para diminuir...");
    }
  }
