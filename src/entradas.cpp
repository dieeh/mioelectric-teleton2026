// Archivo: src/entradas.cpp
#include "entradas.h"

void configurarEntradas() {
  pinMode(PIN_JOYSTICK_VRX, INPUT);
  pinMode(PIN_JOYSTICK_VRY, INPUT);
  pinMode(PIN_JOYSTICK_SW, INPUT_PULLUP);
}

int obtenerEjeX() { 
  return analogRead(PIN_JOYSTICK_VRX); 
}

int obtenerEjeY() { 
  return analogRead(PIN_JOYSTICK_VRY); 
}

bool leerBotonJoystick() { 
  return digitalRead(PIN_JOYSTICK_SW) == LOW; 
}

int leerEMG() { 
  return analogRead(PIN_EMG); 
}