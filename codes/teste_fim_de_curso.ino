const int endStopPin = 7; // Pino onde o fim de curso está conectado
const int ledPin = 12; // Pino onde o LED está conectado
int fdcEstado = 0; // Variável para armazenar o estado do fim de curso

void setup() {
  pinMode(endStopPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  fdcEstado = digitalRead(endStopPin);
  if (fdcEstado == LOW) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}