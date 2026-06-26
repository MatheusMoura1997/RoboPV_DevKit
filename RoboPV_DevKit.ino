#include <Arduino.h>

#include "config.h"
#include "hardware.h"
#include "servidor.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Iniciando RoboPV DevKit...");

  configurarHardware();
  configurarWiFi();
  configurarServidor();

  Serial.println("Sistema iniciado.");
}

void loop() {
  processarServidorWeb();

  atualizarSensores();
  atualizarControlePI();
  verificarSegurancaMovimento();
}