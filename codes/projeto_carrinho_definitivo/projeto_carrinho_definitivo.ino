#include <LiquidCrystal.h>
#include <HX711.h>
#include <AFMotor.h>

// Definição do LCD nos pinos do Arduino
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

HX711 scale;
AF_DCMotor motor(1);

#define pinDT 6
#define pinSCK 7
#define fdccima 8
#define fdcbaixo 9
#define fdcfrente 10
#define fdcatras 13

enum statePorta { FECHADA, ABRINDO, ABERTA, FECHANDO };
statePorta estadoAtual = FECHADA;

void inicializarBalanca() {
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-186567);
  delay(2000);
  scale.tare();
  Serial.println("Balança Zerada");
}

float medida = 0, ultimaMedida = -1;

void medirPeso() {
  medida = scale.get_units(5);
  Serial.print("Peso: ");
  Serial.print(medida, 2);
  Serial.println(" Kg");
}

void imprimirPeso() {
  if (fabs(medida - ultimaMedida) > 0.01) {
    ultimaMedida = medida;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Peso: ");
    lcd.print(medida, 2);
    lcd.print(" Kg");
  }
}

void setup() {
  Serial.begin(57600);

  // Inicializa o LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(2000);

  pinMode(fdccima, INPUT);
  pinMode(fdcfrente, INPUT);
  pinMode(fdcbaixo, INPUT);
  pinMode(fdcatras, INPUT);

  inicializarBalanca();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Balança Pronta!");
  delay(1000);
}

void loop() {
  medirPeso();
  imprimirPeso();
  delay(500);
}
