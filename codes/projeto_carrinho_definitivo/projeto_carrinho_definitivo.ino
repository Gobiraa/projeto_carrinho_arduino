#include <HX711.h>
#include <AFMotor.h>

#define fdccima 1 // fim de curso que identifica a janela fechada.
#define fdcbaixo 2 // fim de curso que identifica a janela aberta.
#define fdcfrente 3 // fim de curso que fecha a janela.
#define fdcatras 4 // fim de curso que abre a janela.

#define motor 5 // motor corrente-continua.
#define pinDT 6 // pinos para a balanca
#define pinSCK 7 // pinos para a balanca
#define fechadura 8 // trava eletronica para assegurar carrinhos.

HX711 scale;


void verficarPorta() {
  if (digitalRead(fdccima) == HIGH && digitalRead(fdcfrente) == HIGH) {
    analogWrite(motor, 0); // Para o motor
    Serial.println("Motor parado, janela fechada.");
  } else {
    fecharPorta();
  }
}

void fecharPorta() {
  Serial.println("Fechando a porta...");
  analogWrite(motor, 255); // Define a velocidade máxima do motor para fechar a porta
  while (digitalRead(fdccima) == LOW || digitalRead(fdcfrente) == LOW) {
    // Aguarda até que ambos os fins de curso estejam HIGH
  }
  analogWrite(motor, 0); // Para o motor quando a porta estiver fechada
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

  scale.power_down(); // DESLIGANDO O SENSOR
  delay(1000); // AGUARDA 1 SEGUNDO
  scale.power_up(); // LIGANDO O SENSOR
}


void setup() {
  Serial.begin(57600);
  pinMode(fdccima, INPUT);
  pinMode(fdcfrente, INPUT);
  pinMode(motor, OUTPUT);
  inicializarBalanca(); // inicializa balança
  fecharPorta(); // certifica que a porta estará fechada
}

void loop() {
  verficarPorta();
  medirPeso();
}

