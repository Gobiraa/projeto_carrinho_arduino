#include <HX711.h>
#include <AFMotor.h>
#include <LiquidCrystal.h>

#define pinDT 6 // pinos para a balanca
#define pinSCK 7 // pinos para a balanca
#define botao 9 // pino do botão
#define fdccima 8 // fim de curso que identifica a janela fechada.
#define fdcbaixo 10 // fim de curso que identifica a janela aberta.
#define fdcfrente 13 // fim de curso que fecha a janela.
#define fdcatras A1 // fim de curso que abre a janela.

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Inicializa LCD com pinos padrões
HX711 scale;
AF_DCMotor motor(1); // Define o motor no conector M1

enum State {
  AGUARDAR_BOTAO,
  FAZER_PESAGEM,
  ABRIR_PORTA,
  EMPURRAR_PRODUTO,
  FECHAR_PORTA
};

State currentState = AGUARDAR_BOTAO;
float medida = 0;

void inicializarBalanca() {
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-186567);
  delay(2000);
  scale.tare();
  Serial.println("Balanca Zerada");
}

void Pesagem() {
  medida = scale.get_units(5);
  Serial.print("Peso lido: "); // Adiciona uma impressão no monitor serial para depuração
  Serial.println(medida, 3);

  // Atualiza o LCD com a leitura
  lcd.setCursor(0, 0);
  lcd.print("Peso / Kg:");  // Texto para mostrar o peso
  lcd.setCursor(0, 1);
  lcd.print(medida, 3); // Exibe o peso com 3 casas decimais

  if (medida <= 0) { // Se o peso for inválido, cancela operação
    lcd.setCursor(0, 0);
    lcd.print("Erro: Peso Inv.");
    Serial.println("Peso inválido, aguardando novo produto.");
    delay(2000);
    lcd.clear();
    currentState = AGUARDAR_BOTAO;
    return;
  }

  scale.power_down();
  delay(1000);
  scale.power_up();

  currentState = ABRIR_PORTA;
}

void motorAbrePorta() { // Acionar motor para gaveta abrir
  Serial.println("Abrindo a porta...");
  motor.setSpeed(150);
  motor.run(BACKWARD);
  while (digitalRead(fdcbaixo) == LOW && digitalRead(fdcatras) == LOW) {
    // Aguarda até que ambos os fins de curso estejam HIGH
  }
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Porta aberta.");
}

void abrirPorta() { // Abrindo porta para o produto ser empurrado
  motorAbrePorta();
  delay(1000);
  currentState = EMPURRAR_PRODUTO;
}

void empurrarProduto() { // Gaveta empurra produto
  Serial.println("Empurrando o produto...");
  motor.setSpeed(150);
  motor.run(FORWARD);
  delay(2000);
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Produto empurrado.");

  currentState = FECHAR_PORTA;
}

void motorFechaPorta() {
  Serial.println("Fechando a porta...");
  motor.setSpeed(150);
  motor.run(FORWARD);
  while (digitalRead(fdccima) == LOW && digitalRead(fdcfrente) == LOW) {
    // Aguarda até que ambos os fins de curso estejam HIGH
  }
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Porta fechada.");
}

void fecharPorta() {
  motorFechaPorta();
  currentState = AGUARDAR_BOTAO;
}

void setup() {
  Serial.begin(57600);

  // Inicializando LCD com a lógica padrão de pinos
  lcd.begin(16, 2);
  pinMode(6, OUTPUT);
  analogWrite(6, 100); // Aumenta o contraste para melhor visibilidade

  // Mostra "Inicializando..." antes de mostrar "Balanca Pronta!"
  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");
  lcd.setCursor(0, 1);
  lcd.print("Aguarde...");
  delay(2000);

  pinMode(fdccima, INPUT);
  pinMode(fdcfrente, INPUT);
  pinMode(fdcbaixo, INPUT);
  pinMode(fdcatras, INPUT);
  pinMode(botao, INPUT_PULLUP);

  inicializarBalanca();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Balanca Pronta!");
  delay(1000);
  lcd.clear();

  currentState = AGUARDAR_BOTAO;
}

void loop() {
  switch (currentState) {
    case AGUARDAR_BOTAO:
      if (digitalRead(botao) == LOW) {
        delay(50);
        if (digitalRead(botao) == LOW) {
          currentState = FAZER_PESAGEM;
        }
      }
      break;
    case FAZER_PESAGEM:
      Pesagem();
      break;
    case ABRIR_PORTA:
      abrirPorta();
      break;
    case EMPURRAR_PRODUTO:
      empurrarProduto();
      break;
    case FECHAR_PORTA:
      fecharPorta();
      break;
  }
}
