// Archivo: include/sensor_emg.h
#pragma once
#include <Arduino.h>

const uint8_t PIN_EMG_ANALOGICO = 34;
const uint8_t PIN_BOTON_EMULADOR = 13;

const uint16_t UMBRAL_CONTRACCION = 2500; // Ajustable según la ganancia del sensor (0 a 4095)

void configurarSensores();
bool detectarIntencionCierre();