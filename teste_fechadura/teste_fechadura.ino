const int buttonPin = 8; // Pino onde o botão está conectado
const int relayPin = 9; // Pino onde o relé está conectado
int buttonState = 0; // Variável para armazenar o estado do botão

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(relayPin, OUTPUT); 
  digitalWrite(relayPin, LOW);
}

void loop() {
  buttonState = digitalRead(buttonPin); 
  if (buttonState == LOW) { 
    digitalWrite(relayPin, HIGH);
    delay(1000);
    digitalWrite(relayPin, LOW);
  }
}