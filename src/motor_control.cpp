#include "motor_control.h"

const uint8_t pinesINA[4] = {M1_IN1, M2_IN3, M3_IN1, M4_IN3};
const uint8_t pinesINB[4] = {M1_IN2, M2_IN4, M3_IN2, M4_IN4};

volatile int32_t posicionEncoderM1 = 0;

enum EstadoMano { 
  REPOSO, 
  PINZA_ACTIVA, 
  PUNO_ACTIVO, 
  RETORNO_PINZA, 
  RETORNO_PUNO, 
  CALIBRACION_RETRACCION,
  CALIBRACION_AVANCE,
  CALIBRACION_INDIVIDUAL
};
EstadoMano estadoActual = REPOSO;

int32_t posicionAnterior = 0;
unsigned long tiempoUltimoMovimiento = 0;
bool objetoSujetado = false;
const unsigned long TIEMPO_BLOQUEO_MS = 150; 

uint8_t motorSeleccionado = 0; 
int direccionMotoresCalibracion[4] = {0, 0, 0, 0}; 

void IRAM_ATTR ISR_M1() { 
  if (gpio_get_level((gpio_num_t)M1_ENC_B)) {
    posicionEncoderM1++;
  } else {
    posicionEncoderM1--;
  }
}

int32_t obtenerPosicionEncoder() {
  return posicionEncoderM1;
}

void resetearEncoder() {
  posicionEncoderM1 = 0;
  Serial.println(F(">> [ENCODER] Cero absoluto reestablecido (posicionEncoderM1 = 0)."));
}

void moverMotorIndividuo(uint8_t i, int direccion, uint8_t velocidad) {
  if (i >= 4) return;

  if (direccion == 0) {
    ledcWrite(pinesINA[i], 0);
    ledcWrite(pinesINB[i], 0);
  } else if (direccion > 0) { // Avanzar -> Apretar / Cerrar mano
    ledcWrite(pinesINA[i], velocidad);
    ledcWrite(pinesINB[i], 0);
  } else { // Retraer -> Estirar / Abrir mano
    ledcWrite(pinesINA[i], 0);
    ledcWrite(pinesINB[i], velocidad);
  }
}

void procesarComandosSerial() {
  while (Serial.available() > 0) {
    char cmd = toupper(Serial.read());

    if (cmd >= '0' && cmd <= '4') {
      motorSeleccionado = cmd - '0';
      if (motorSeleccionado == 0) {
        Serial.println(F(">> [SELECCION] Modo global: TODOS los dedos seleccionados."));
      } else {
        Serial.print(F(">> [SELECCION] Dedo en foco: Motor M"));
        Serial.println(motorSeleccionado);
      }
      continue;
    }

    switch (cmd) {
      case 'A':
        if (motorSeleccionado == 0) {
          estadoActual = CALIBRACION_AVANCE;
          posicionAnterior = posicionEncoderM1;
          tiempoUltimoMovimiento = millis();
          for (int i = 0; i < 4; i++) direccionMotoresCalibracion[i] = 1;
          Serial.println(F(">> [CALIBRACION] Apretando TODOS los actuadores..."));
        } else {
          estadoActual = CALIBRACION_INDIVIDUAL;
          direccionMotoresCalibracion[motorSeleccionado - 1] = 1;
          Serial.print(F(">> [MANUAL] Apretando Motor M"));
          Serial.println(motorSeleccionado);
        }
        break;

      case 'R':
        if (motorSeleccionado == 0) {
          estadoActual = CALIBRACION_RETRACCION;
          for (int i = 0; i < 4; i++) direccionMotoresCalibracion[i] = -1;
          Serial.println(F(">> [MANUAL] Estirando TODOS los actuadores..."));
        } else {
          estadoActual = CALIBRACION_INDIVIDUAL;
          direccionMotoresCalibracion[motorSeleccionado - 1] = -1;
          Serial.print(F(">> [MANUAL] Estirando Motor M"));
          Serial.println(motorSeleccionado);
        }
        break;

      case 'S':
        if (motorSeleccionado == 0) {
          for (int i = 0; i < 4; i++) direccionMotoresCalibracion[i] = 0;
          estadoActual = REPOSO;
          Serial.println(F(">> [MANUAL] TODOS los motores detenidos."));
        } else {
          direccionMotoresCalibracion[motorSeleccionado - 1] = 0;
          Serial.print(F(">> [MANUAL] Motor M"));
          Serial.print(motorSeleccionado);
          Serial.println(F(" en pausa."));

          bool algunActivo = false;
          for (int i = 0; i < 4; i++) {
            if (direccionMotoresCalibracion[i] != 0) algunActivo = true;
          }
          if (!algunActivo) estadoActual = REPOSO;
        }
        break;

      case 'Z':
        resetearEncoder();
        break;

      case 'F':
        for (int i = 0; i < 4; i++) direccionMotoresCalibracion[i] = 0;
        estadoActual = REPOSO;
        resetearEncoder();
        Serial.println(F(">> [MANUAL] Detención total y punto cero fijado."));
        break;

      default:
        break;
    }
  }
}

void configurarMotoresYEncoder() {
  for (int i = 0; i < 4; i++) {
    ledcAttach(pinesINA[i], FREQ, RESOLUTION);
    ledcAttach(pinesINB[i], FREQ, RESOLUTION);
  }

  pinMode(M1_ENC_A, INPUT_PULLUP); 
  pinMode(M1_ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(M1_ENC_A), ISR_M1, RISING);
}

void procesarAccionMano(bool activarPinza, bool activarPuno) {
  
  switch (estadoActual) {
    case REPOSO:
      if (activarPuno) {
        estadoActual = PUNO_ACTIVO;
        objetoSujetado = false;
        posicionAnterior = posicionEncoderM1;
        tiempoUltimoMovimiento = millis();
      } else if (activarPinza) {
        estadoActual = PINZA_ACTIVA;
        objetoSujetado = false;
        posicionAnterior = posicionEncoderM1;
        tiempoUltimoMovimiento = millis();
      }
      break;

    case PINZA_ACTIVA:
      if (!activarPinza && !activarPuno) estadoActual = RETORNO_PINZA;
      break;

    case PUNO_ACTIVO:
      if (!activarPuno && !activarPinza) estadoActual = RETORNO_PUNO;
      break;

    case RETORNO_PINZA:
    case RETORNO_PUNO:
      if (posicionEncoderM1 <= 0) {
        posicionEncoderM1 = 0; 
        estadoActual = REPOSO;
      }
      break;

    case CALIBRACION_RETRACCION:
    case CALIBRACION_AVANCE:
    case CALIBRACION_INDIVIDUAL:
      if (activarPuno) estadoActual = PUNO_ACTIVO;
      else if (activarPinza) estadoActual = PINZA_ACTIVA;
      break;
  }

  switch (estadoActual) {
    case REPOSO:
      for (int i = 0; i < 4; i++) {
        moverMotorIndividuo(i, 0, 0);
      }
      break;

    case CALIBRACION_AVANCE:
      if (posicionEncoderM1 != posicionAnterior) {
        posicionAnterior = posicionEncoderM1;
        tiempoUltimoMovimiento = millis();
      }
      
      if (millis() - tiempoUltimoMovimiento > TIEMPO_BLOQUEO_MS) {
        for (int i = 0; i < 4; i++) {
          moverMotorIndividuo(i, 0, 0);
          direccionMotoresCalibracion[i] = 0;
        }
        Serial.print(F(">> [CALIBRACION CONCLUIDA] Tope superior detectado: "));
        Serial.print(posicionEncoderM1);
        Serial.println(F(" pulsos. Asigna este valor a TOPE_MAXIMO_PULSOS_M1."));
        estadoActual = REPOSO;
      } else {
        for (int i = 0; i < 4; i++) {
          moverMotorIndividuo(i, 1, MAX_SPEED);
        }
      }
      break;

    case PUNO_ACTIVO:
      if (posicionEncoderM1 != posicionAnterior) {
        posicionAnterior = posicionEncoderM1;
        tiempoUltimoMovimiento = millis();
      }
      
      if ((millis() - tiempoUltimoMovimiento > TIEMPO_BLOQUEO_MS) && !objetoSujetado) {
        objetoSujetado = true;
      }

      if (posicionEncoderM1 < TOPE_MAXIMO_PULSOS_M1 && !objetoSujetado) {
        for (int i = 0; i < 4; i++) {
          moverMotorIndividuo(i, 1, MAX_SPEED);
        }
      } else {
        for (int i = 0; i < 4; i++) {
          moverMotorIndividuo(i, 0, 0);
        }
      }
      break;

    case PINZA_ACTIVA:
      if (posicionEncoderM1 != posicionAnterior) {
        posicionAnterior = posicionEncoderM1;
        tiempoUltimoMovimiento = millis();
      }
      
      if ((millis() - tiempoUltimoMovimiento > TIEMPO_BLOQUEO_MS) && !objetoSujetado) {
        objetoSujetado = true;
      }

      if (posicionEncoderM1 < TOPE_MAXIMO_PULSOS_M1 && !objetoSujetado) {
        moverMotorIndividuo(0, 1, MAX_SPEED);
      } else {
        moverMotorIndividuo(0, 0, 0);
      }
      
      for (int i = 1; i < 4; i++) {
        moverMotorIndividuo(i, 0, 0);
      }
      break;

    case RETORNO_PUNO:
      if (posicionEncoderM1 > 0) {
        for (int i = 0; i < 4; i++) {
          moverMotorIndividuo(i, -1, MAX_SPEED);
        }
      } else {
        for (int i = 0; i < 4; i++) {
          moverMotorIndividuo(i, 0, 0);
        }
      }
      break;

    case RETORNO_PINZA:
      if (posicionEncoderM1 > 0) {
        moverMotorIndividuo(0, -1, MAX_SPEED);
      } else {
        moverMotorIndividuo(0, 0, 0);
      }
      for (int i = 1; i < 4; i++) {
        moverMotorIndividuo(i, 0, 0);
      }
      break;

    case CALIBRACION_RETRACCION:
      for (int i = 0; i < 4; i++) {
        moverMotorIndividuo(i, -1, MAX_SPEED);
      }
      break;

    case CALIBRACION_INDIVIDUAL:
      for (int i = 0; i < 4; i++) {
        moverMotorIndividuo(i, direccionMotoresCalibracion[i], MAX_SPEED);
      }
      break;
  }
}