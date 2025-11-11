// Define os pinos de controle do motor ligados ao Arduino
#define PINO_IN1 5  // Pino responsável pelo controle no sentido horário
#define PINO_IN2 6  // Pino responsável pelo controle no sentido anti-horário
#define PINO_PWM 2
#define PINO_IN3 7 // Pino responsável pelo controle no sentido horário
#define PINO_IN4 8  // Pino responsável pelo controle no sentido anti-horário
#define PINO_PWM2 9


void setup(){ 
  // Configura os pinos de controle como saída
  pinMode(PINO_IN1, OUTPUT);  // Define o pino IN1 como saída
  pinMode(PINO_IN2, OUTPUT);  // Define o pino IN2 como saída
  pinMode(PINO_PWM, OUTPUT);  // Define o pino IN2 como saída
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
  pinMode(PINO_IN3, OUTPUT);  // Define o pino IN1 como saída
  pinMode(PINO_IN4, OUTPUT);  // Define o pino IN2 como saída
  pinMode(PINO_PWM2, OUTPUT);  // Define o pino IN2 como saída
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, HIGH);
}
  
void loop() {   
  int valor_pwm = 100;  // Variável para armazenar o valor PWM (0-255)
 
  analogWrite(PINO_PWM, valor_pwm);  // Aplica o valor PWM no pino IN1
  analogWrite(PINO_PWM2, valor_pwm);  // Aplica o valor PWM no pino IN1
  
 
}
