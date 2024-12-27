#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AFMotor.h>

#define pinDT 6 // pinos para a balanca
#define pinSCK 7 // pinos para a balanca
#define botao 9 // pino do botão
#define fdccima 2 // fim de curso que identifica a janela fechada.
#define fdcbaixo 3 // fim de curso que identifica a janela aberta.
#define fdcfrente 4 // fim de curso que fecha a janela.
#define fdcatras 5 // fim de curso que abre a janela.

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;
AF_DCMotor motor(1); // Define o motor no conector M1

enum State {
  AGUARDAR_BOTAO,
  MEDIR_PESO,
  ABRIR_PORTA,
  EMPURRAR_PRODUTO,
  FECHAR_PORTA
};

State currentState = AGUARDAR_BOTAO;
float medida = 0;

void inicializarBalanca() {
  scale.begin(pinDT, pinSCK); // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale(-186567); // LIMPANDO O VALOR DA ESCALA

  delay(2000);
  scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA

  Serial.println("Balança Zerada");
}

void medirPeso() {
  medida = scale.get_units(5); // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS
  Serial.println(medida, 3); // ENVIANDO PARA MONITOR SERIAL A MEDIDA COM 3 CASAS DECIMAIS

  lcd.setCursor(0, 0); // Define o cursor na primeira linha, primeira coluna
  lcd.print(" Peso / Kg"); // Exibe a mensagem

  lcd.setCursor(0, 1); // Define o cursor na segunda linha, primeira coluna
  lcd.print(medida, 3); // Exibe o valor da medida com 3 casas decimais

  scale.power_down(); // DESLIGANDO O SENSOR
  delay(1000); // AGUARDA 1 SEGUNDO
  scale.power_up(); // LIGANDO O SENSOR

  currentState = ABRIR_PORTA; // Muda para o estado de abrir a porta
}

void abrirPorta() {
  Serial.println("Abrindo a porta...");
  motor.setSpeed(150); // Define a velocidade do motor para abrir a porta
  motor.run(BACKWARD); // Faz o motor girar para abrir a porta
  while (digitalRead(fdcbaixo) == LOW || digitalRead(fdcatras) == LOW) {
    // Aguarda até que ambos os fins de curso estejam HIGH
  }
  motor.setSpeed(0); // Para o motor quando a porta estiver aberta
  motor.run(RELEASE); // Libera o motor
  Serial.println("Porta aberta.");

  delay(1000); // Aguarda 1 segundo antes de empurrar o produto

  currentState = EMPURRAR_PRODUTO; // Muda para o estado de empurrar o produto
}

void empurrarProduto() {
  Serial.println("Empurrando o produto...");
  motor.setSpeed(150); // Define a velocidade do motor para empurrar o produto
  motor.run(FORWARD); // Faz o motor girar para empurrar o produto
  delay(2000); // Aguarda 2 segundos para empurrar o produto
  motor.setSpeed(0); // Para o motor após empurrar o produto
  motor.run(RELEASE); // Libera o motor
  Serial.println("Produto empurrado.");

  currentState = FECHAR_PORTA; // Muda para o estado de fechar a porta
}

void fecharPorta() {
  Serial.println("Fechando a porta...");
  motor.setSpeed(150); // Define a velocidade do motor para fechar a porta
  motor.run(FORWARD); // Faz o motor girar para fechar a porta
  while (digitalRead(fdccima) == LOW || digitalRead(fdcfrente) == LOW) {
    // Aguarda até que ambos os fins de curso estejam HIGH
  }
  motor.setSpeed(0); // Para o motor quando a porta estiver fechada
  motor.run(RELEASE); // Libera o motor
  Serial.println("Porta fechada.");

  currentState = AGUARDAR_BOTAO; // Muda para o estado de aguardar o botão
}

void setup() {
  Serial.begin(57600);
  pinMode(fdccima, INPUT);
  pinMode(fdcfrente, INPUT);
  pinMode(fdcbaixo, INPUT);
  pinMode(fdcatras, INPUT);
  pinMode(botao, INPUT_PULLUP); // Configura o pino do botão com pull-up interno
  lcd.begin(16, 2); // Inicializa o display com 16 colunas e 2 linhas
  lcd.backlight(); // Liga o backlight
  inicializarBalanca(); // inicializa balança
  currentState = AGUARDAR_BOTAO; // Inicia no estado de aguardar o botão
}

void loop() {
  switch (currentState) {
    case AGUARDAR_BOTAO:
      if (digitalRead(botao) == LOW) { // Verifica se o botão foi pressionado
        delay(50); // Debounce
        if (digitalRead(botao) == LOW) {
          currentState = MEDIR_PESO; // Muda para o estado de medir peso
        }
      }
      break;
    case MEDIR_PESO:
      medirPeso();
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