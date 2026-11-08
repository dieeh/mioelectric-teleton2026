# Firmware ESP32 para prótesis biomecánica de mano

Firmware desarrollado en C++ con PlatformIO para controlar una prótesis biomecánica de mano accionada por cuatro motores DC acoplados a microactuadores lineales. La implementación utiliza un ESP32 y FreeRTOS para ejecutar tareas concurrentes de control, adquisición de señales y comunicación.

## Arquitectura del sistema

El firmware aprovecha el microcontrolador ESP32 de doble núcleo y FreeRTOS para separar la adquisición de entradas de la lógica principal de control. La adquisición de señales se ejecuta de forma concurrente, evitando bloquear la máquina de estados encargada del movimiento.

El sistema mecánico utiliza cuatro motores DC N20, controlados mediante dos drivers puente H L298N. La arquitectura de control descrita en el firmware considera un dedo índice como referencia (maestro), cuyo movimiento es monitoreado mediante un encoder magnético de efecto Hall H949; los otros tres motores siguen la señal de control del maestro.

## Entradas y control

- **Encoder Hall H949:** retroalimentación de movimiento mediante interrupciones del ESP32.
- **Joystick analógico:** control de movimientos de agarre, con filtrado de zona muerta.
- **Sensor EMG:** lectura de señal electromiográfica para adquisición y evaluación de futuras estrategias de control mioeléctrico.
- **Consola serial:** interfaz de comandos y telemetría para pruebas y calibración.

## Funcionalidades implementadas

- Control de motores mediante drivers L298N.
- Coordinación de motores en una arquitectura maestro-esclavo.
- Lectura del encoder para monitorear el movimiento del motor de referencia.
- Lectura de entradas analógicas del joystick y del sensor EMG.
- Detección de atasco/contacto a partir de la ausencia de pulsos del encoder durante el movimiento, con detención del accionamiento.
- Calibración de límites de recorrido, inversión de sentido y ajuste de posición mediante comandos seriales.
- Telemetría por puerto serie a 115200 baudios.

## Hardware

- Microcontrolador ESP32.
- Cuatro motores DC N20 con mecanismo de actuador lineal.
- Dos controladores puente H L298N.
- Encoder magnético de efecto Hall H949.
- Joystick analógico.
- Sensor electromiográfico (EMG).

Las conexiones y asignaciones de pines se encuentran en los archivos de `include/` y `src/`. Deben revisarse junto con el montaje físico antes de energizar el sistema.

## Tecnologías y herramientas

- C++
- ESP32
- FreeRTOS
- PlatformIO
- Control PWM, interrupciones y lectura analógica

## Estructura del repositorio

- `src/main.cpp`: punto de entrada y coordinación del firmware.
- `src/motor_control.cpp`: lógica de control de motores.
- `src/entradas.cpp`: adquisición y procesamiento de entradas.
- `src/sensor_emg.cpp`: lectura del sensor EMG.
- `include/`: declaraciones e interfaces de los módulos.
- `platformio.ini`: configuración de compilación y plataforma.

## Compilación

1. Instala Visual Studio Code y la extensión PlatformIO IDE.
2. Abre el directorio del repositorio como proyecto de PlatformIO.
3. Conecta una placa ESP32 compatible.
4. Revisa la configuración de placa, pines y periféricos en `platformio.ini` y en los archivos fuente.
5. Compila y carga el firmware desde PlatformIO.

## Contexto

Proyecto desarrollado en el contexto del desafío Teletón-USS 2026. El repositorio contiene el firmware de control y adquisición para el prototipo de prótesis; no representa por sí solo un dispositivo médico certificado.
