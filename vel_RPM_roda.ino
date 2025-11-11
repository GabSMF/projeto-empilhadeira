#define DIAMETRO 50 /*em mm*/
#define ENCODER_PIN 3
#define DENTES 32 /*qnt*/
#define PULSOS_POR_ROT 34.0f
#define t_dl 200
#define PI 3.14159
#define PINO_IN1 5  // Pino responsável pelo controle no sentido horário
#define PINO_IN2 6  // Pino responsável pelo controle no sentido anti-horário
#define PINO_PWM 2

volatile int encoder = 0;
int ant_encoder = encoder;
float vel = 0;
float rot = 0;
float rpm = 0; // <-- 1. Adicione a variável RPM
unsigned long t = 0;

void incrementa_encoder()
{
  encoder++;
}

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), incrementa_encoder, RISING);
  t = millis();
  pinMode(PINO_IN1, OUTPUT);  // Define o pino IN1 como saída
  pinMode(PINO_IN2, OUTPUT);  // Define o pino IN2 como saída
  pinMode(PINO_PWM, OUTPUT); 
  digitalWrite(PINO_IN1, HIGH);
  digitalWrite(PINO_IN2, LOW);
}

void loop() {
  int valor_pwm = 100; 
  analogWrite(PINO_PWM, valor_pwm);
  if(millis() - t > t_dl)
  {
    rot = (encoder - ant_encoder) / PULSOS_POR_ROT; 
    ant_encoder = encoder;
    
    // vel = rot * PI * DIAMETRO / (t_dl / 1000.0); // Cálculo antigo (mm/s)
    
    // vvv 2. Este é o novo cálculo para RPM vvv
    rpm = rot * (60000.0 / t_dl); 
    
    t = millis();
    
    // vvv 3. Imprima o valor de RPM vvv
    Serial.println(rpm); 
  }
}
