/*
  Código para testar motor DC com tc1508A e perceber melhor aceleração/desaceleração
  Pino 5 -> IN1 | Pino 6 -> IN2
*/

const int motorPin1 = 5;  // IN1 do driver tc1508A
const int motorPin2 = 6;  // IN2 do driver tc1508A

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
}

void loop() {
  // Movimento sentido anti-horário
  for (int velocidade = 0; velocidade <= 255; velocidade += 2) { 
    analogWrite(motorPin1, velocidade);
    analogWrite(motorPin2, 0);
    delay(100); 
  }
  
  for (int velocidade = 255; velocidade >= 0; velocidade -= 2) {
    analogWrite(motorPin1, velocidade);
    analogWrite(motorPin2, 0);
    delay(100);
  }
  
  // Pausa antes de inverter a direção
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 0);
  delay(1500);
  
  // Movimento no sentido horario
  for (int velocidade = 0; velocidade <= 255; velocidade += 2) {
    analogWrite(motorPin2, velocidade);
    analogWrite(motorPin1, 0);
    delay(100);
  }
  
  for (int velocidade = 255; velocidade >= 0; velocidade -= 2) {
    analogWrite(motorPin2, velocidade);
    analogWrite(motorPin1, 0);
    delay(100);
  }
  
  // Pausa antes de reiniciar o ciclo
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 0);
  delay(1500);
}
