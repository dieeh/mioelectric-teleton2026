// Archivo: src/sensor_emg.cpp
#include "sensor_emg.h"

void configurarSensores() {
  pinMode(PIN_EMG_ANALOGICO, INPUT);
  pinMode(PIN_BOTON_EMULADOR, INPUT_PULLUP);
}

bool detectarIntencionCierre() {
  uint16_t lecturaMuscular = analogRead(PIN_EMG_ANALOGICO);
  bool botonPresionado = (digitalRead(PIN_BOTON_EMULADOR) == LOW);
  
  if (lecturaMuscular > UMBRAL_CONTRACCION || botonPresionado) {
    return true;
  }
  return false;
}