#pragma once
#include <Arduino.h>

const uint8_t M1_IN1 = 17; const uint8_t M1_IN2 = 16;
const uint8_t M2_IN3 = 26; const uint8_t M2_IN4 = 27;
const uint8_t M3_IN1 = 19; const uint8_t M3_IN2 = 18;
const uint8_t M4_IN3 = 33; const uint8_t M4_IN4 = 32;

const uint8_t M1_ENC_A = 21; 
const uint8_t M1_ENC_B = 22;

const uint32_t FREQ = 1000;
const uint8_t RESOLUTION = 8;
const uint8_t MAX_SPEED = 255;

const int32_t TOPE_MAXIMO_PULSOS_M1 = 50000; 

void configurarMotoresYEncoder();
void procesarAccionMano(bool activarPinza, bool activarPuno);
int32_t obtenerPosicionEncoder(); 

void procesarComandosSerial();
void resetearEncoder();
void moverMotorIndividuo(uint8_t i, int direccion, uint8_t velocidad);