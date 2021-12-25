#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000
#define pino A0
#define Tilt 4
#define FSR A5
long Freq, Tempo;
int PulsoNumero, LeituraFSR;
bool pulso;
int Estado = 0;
int Temp = 0;
int Bpm = 0;
int SpO2 = 0;
PulseOximeter pox;
uint32_t tsLastReport = 0;
void onBeatDetected() {
}
void setup() {
  pinMode(Tilt, INPUT);
  Serial.begin(57600);
  if (!pox.begin()) {
    for (;;);
  } else {
  }
  pinMode(FSR, INPUT);
  pulso = 1;             //Seta variável de controle
}
void loop() {
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Bpm = pox.getHeartRate();
    if ((Bpm < 20) || (Bpm > 200)) {
      Bpm = 65;
    }
    SpO2 = pox.getSpO2();
    if ((SpO2 < 80) || (SpO2 > 100)) {
      SpO2 = 95;
    }
    Serial.print(Bpm);
    Serial.print("?");
    Serial.print(SpO2);
    Serial.print("*");
    Estado = digitalRead(Tilt);
    Serial.print(Estado);
    /*
       Cada Grau.C :10mV | 10mV/C
       0V = 0
       5V = 1023

       Logo,podemos concluir que:

       Tensão em A0 = [(Valor_Lido_de_A0)*(5.0)]/1023
       Temp = Tensão em A0/10mV
    */
    Temp = (analogRead(pino) * (5.0 / 1023)) / 0.01;
    Serial.print(";");
    Serial.print(Temp);
    Serial.print("~");
    Serial.print(Freq);
    Serial.print("#");
    tsLastReport = millis();
  }
  LeituraFSR = analogRead(FSR);
  Tempo = millis();      //counter recebe o valor do tempo em ms
  if (LeituraFSR >= 500) { //Entrada de frequência em nível alto?                        //Sim...
    if ((pulso) == 1) {
      PulsoNumero++;  //incrementa pulseCount se variável de controle for verdadeira
      pulso = 0;            //limpa variável de controle
    }
  }
  else                     //Não...
  {
    pulso = 1;          //Seta variável de controle
  }
  if (Tempo % 20000 == 0) { //Passaram-se 1000 s?
    //Sim...
    Freq = PulsoNumero * 3; // Transforma os dados da contagem de numero em 10s em 60s
    PulsoNumero = 0;     //Reinicia contador
  }
}
