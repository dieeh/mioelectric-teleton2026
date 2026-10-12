// Archivo: include/entradas.h
#pragma once
#include <Arduino.h>

const uint8_t PIN_EMG = 34;
const uint8_t PIN_JOYSTICK_VRX = 35;
const uint8_t PIN_JOYSTICK_VRY = 39;
const uint8_t PIN_JOYSTICK_SW = 13;

// Zona muerta calibrada para centro en 3000
const int ZONA_MUERTA_MIN = 2600;
const int ZONA_MUERTA_MAX = 3300;

void configurarEntradas();
int obtenerEjeX();
int obtenerEjeY();
bool leerBotonJoystick();
int leerEMG();