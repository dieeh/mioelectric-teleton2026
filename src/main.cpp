#include <Arduino.h>
#include "motor_control.h"
#include "entradas.h"

volatile bool ordenPinza = false;
volatile bool ordenPuno = false;
volatile int valorEMG = 0;

void tareaLecturaSensores(void * parametro) {
  unsigned long ultimoAviso = 0;

  for(;;) {
    valorEMG = leerEMG();
    int posX = obtenerEjeX();
    int posY = obtenerEjeY();
    bool botonPresionado = leerBotonJoystick();

    ordenPuno = botonPresionado;
    ordenPinza = (posX > ZONA_MUERTA_MAX || posX < ZONA_MUERTA_MIN || 
                  posY > ZONA_MUERTA_MAX || posY < ZONA_MUERTA_MIN);

    if (millis() - ultimoAviso > 500) {
      Serial.print("Raw X: ");
      Serial.print(posX);
      Serial.print(" | Raw Y: ");
      Serial.print(posY);
      Serial.print(" | Orden Pinza: ");
      Serial.print(ordenPinza ? "SI" : "NO");
      Serial.print(" | Orden Puno: ");
      Serial.print(ordenPuno ? "SI" : "NO");
      Serial.print(" | POSICION ENCODER M1: ");
      Serial.println(obtenerPosicionEncoder());
      ultimoAviso = millis();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  configurarMotoresYEncoder();
  configurarEntradas();
  procesarAccionMano(false, false);

  xTaskCreatePinnedToCore(
    tareaLecturaSensores, "LecturaSensores", 2048, NULL, 1, NULL, 0
  );

  Serial.println("=========================================");
  Serial.println("MODO CALIBRACION Y ALINEACION INDIVIDUAL");
  Serial.println("Seleccion de motor: '1', '2', '3', '4' o '0' (Todos)");
  Serial.println("Acciones: 'A' (Avanzar), 'R' (Retraer), 'S' (Pausa)");
  Serial.println("Control general: 'Z' (Reset Cero), 'F' (Frenado Total)");
  Serial.println("=========================================");
}

void loop() {
  procesarComandosSerial();
  procesarAccionMano(ordenPinza, ordenPuno);
  delay(10);
}