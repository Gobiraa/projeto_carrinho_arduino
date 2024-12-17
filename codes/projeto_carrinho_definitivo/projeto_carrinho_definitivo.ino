#include <HX711.h>
#include <AFMotor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

AF_DCMotor motor(1); // Define o motor no conector M1
#define fdccima 2 // fim de curso que identifica a janela fechada.
#define fdcbaixo 3 // fim de curso que identifica a janela aberta.
#define fdcfrente 4 // fim de curso que fecha a janela.
#define fdcatras 5 // fim de curso que abre a janela.
#define pinDT 6 // pinos para a balanca
#define pinSCK 7 // pinos para a balanca
#define fechadura 8 // trava eletronica para assegurar carrinhos.

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;

void verificarPorta() {
  if (digitalRead(fdccima) == HIGH && digitalRead(fdcfrente) == HIGH) {
    motor.setSpeed(0); // Para o motor
    Serial.println("Motor parado, janela fechada.");
  } else {
    fecharPorta();
  }
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
}

void inicializarBalanca() {
  scale.begin(pinDT, pinSCK); // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale(-186567); // LIMPANDO O VALOR DA ESCALA

  delay(2000);
  scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA

  Serial.println("Balança Zerada");
}

float medida = 0;
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
}

void setup() {
  Serial.begin(57600);
  pinMode(fdccima, INPUT);
  pinMode(fdcfrente, INPUT);
  lcd.begin(16, 2); // Inicializa o display com 16 colunas e 2 linhas
  lcd.backlight(); // Liga o backlight
  inicializarBalanca(); // inicializa balança
  fecharPorta(); // certifica que a porta estará fechada
}

void loop() {
  verificarPorta();
  medirPeso();
}