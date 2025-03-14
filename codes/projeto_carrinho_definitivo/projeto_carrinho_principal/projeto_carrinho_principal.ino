#include <HX711.h>
#include <AFMotor.h>
#include <LiquidCrystal.h>

#define pinDT 8        // Pino DT da balança
#define pinSCK 7       // Pino SCK da balança
#define botao 9        // Botão de início
#define fdccima A2     // Fim de curso superior (porta fechada)
#define fdcbaixo 10    // Fim de curso inferior (porta aberta)
#define fdcfrente 13   // Fim de curso frente (gaveta fechada)
#define fdcatras A1    // Fim de curso traseiro (gaveta aberta)

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Inicializa LCD
HX711 scale;                           
AF_DCMotor motor(1);                   // Motor DC conectado ao M1

enum State {
  AGUARDAR_BOTAO,
  FAZER_PESAGEM,
  VERIFICAR_PESO,
  ABERTURA_MANUAL,
  AGUARDANDO_PRODUTO,
  ABRIR_PORTA_INTERNA,
  EMPURRAR_PRODUTO,
  RECUAR_GAVETA,
  FECHAR_PORTA_INTERNA,
  ABERTURA_MANUAL_ENTRADA
};

State currentState = AGUARDAR_BOTAO;
float medida = 0;

void inicializarBalanca() {
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-186567); // Calibração da balança
  delay(2000);
  scale.tare();
  Serial.println("Balanca Zerada");
}


bool pesoValido(float peso) {
  return peso > 0 && peso < 10; // Exemplo: peso deve estar entre 0 e 10 kg
}

bool portaEntradaAbertaManualmente() {
  return digitalRead(fdcbaixo) == HIGH; // Simula abertura manual
}

bool produtoColocado() {
  return true; // detecção de produto colocado
}

void motorAbrePortaInterna() {
  Serial.println("Abrindo a porta interna...");
  motor.setSpeed(150);
  motor.run(BACKWARD);
  while (digitalRead(fdcatras) == LOW) {
    // Aguarda até que o fim de curso traseiro seja ativado
  }
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Porta interna aberta.");
}

void motorFechaPortaInterna() {
  Serial.println("Fechando a porta interna...");
  motor.setSpeed(150);
  motor.run(FORWARD);
  while (digitalRead(fdcfrente) == LOW) {
    // Aguarda até que o fim de curso frente seja ativado
  }
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Porta interna fechada.");
}

void motorEmpurraProduto() {
  Serial.println("Empurrando o produto...");
  motor.setSpeed(150);
  motor.run(FORWARD);
  delay(2000);
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Produto empurrado.");
}

void motorRecuaGaveta() {
  Serial.println("Recuando a gaveta...");
  motor.setSpeed(150);
  motor.run(BACKWARD);
  delay(1000);
  motor.setSpeed(0);
  motor.run(RELEASE);
  Serial.println("Gaveta recuada.");
}

void setup() {
  Serial.begin(57600);

  // Configura os pinos
  pinMode(botao, INPUT_PULLUP);
  pinMode(fdccima, INPUT);
  pinMode(fdcfrente, INPUT);
  pinMode(fdcbaixo, INPUT);
  pinMode(fdcatras, INPUT);

  // Inicializa o LCD
  lcd.begin(16, 2);
  pinMode(6, OUTPUT);
  analogWrite(6, 100); // Ajuste de contraste do LCD

  // Mensagem inicial
  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");
  lcd.setCursor(0, 1);
  lcd.print("Aguarde...");
  delay(2000);

  // Inicializa a balança
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
      if (digitalRead(botao) == LOW) { // Botão pressionado
        delay(50); // Debounce
        if (digitalRead(botao) == LOW) {
          currentState = FAZER_PESAGEM;
        }
      }
      break;

    case FAZER_PESAGEM:
      medida = scale.get_units(5);
      Serial.print("Peso lido: ");
      Serial.println(medida, 3);

      lcd.setCursor(0, 0);
      lcd.print("Peso / Kg:  ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(medida, 3);

      if (medida <= 0) { // Peso inválido
        lcd.setCursor(0, 0);
        lcd.print("Erro: Peso Inv.");
        Serial.println("Peso inválido, aguardando novo produto.");
        delay(2000);
        lcd.clear();
        currentState = AGUARDAR_BOTAO;
        return;
      }

      currentState = VERIFICAR_PESO;
      break;

    case VERIFICAR_PESO:
      if (pesoValido(medida)) {
        currentState = ABERTURA_MANUAL;
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Peso Incorreto");
        delay(2000);
        lcd.clear();
        currentState = AGUARDAR_BOTAO;
      }
      break;

    case ABERTURA_MANUAL:
      if (portaEntradaAbertaManualmente()) {
        currentState = AGUARDANDO_PRODUTO;
      }
      break;

    case AGUARDANDO_PRODUTO:
      if (produtoColocado()) {
        currentState = ABRIR_PORTA_INTERNA;
      }
      break;

    case ABRIR_PORTA_INTERNA:
      motorAbrePortaInterna();
      delay(1000);
      currentState = EMPURRAR_PRODUTO;
      break;

    case EMPURRAR_PRODUTO:
      motorEmpurraProduto();
      currentState = RECUAR_GAVETA;
      break;

    case RECUAR_GAVETA:
      motorRecuaGaveta();
      currentState = FECHAR_PORTA_INTERNA;
      break;

    case FECHAR_PORTA_INTERNA:
      motorFechaPortaInterna();
      currentState = ABERTURA_MANUAL_ENTRADA;
      break;

    case ABERTURA_MANUAL_ENTRADA:
      if (portaEntradaAbertaManualmente()) {
        currentState = AGUARDAR_BOTAO;
      }
      break;
  }
}