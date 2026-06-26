#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================
// WIFI
// ==========================

static const char WIFI_SSID[] = "RoboPV";
static const char WIFI_PASSWORD[] = "12345678";

// IP do ESP32 DevKit
static const uint8_t DEVKIT_IP_1 = 192;
static const uint8_t DEVKIT_IP_2 = 168;
static const uint8_t DEVKIT_IP_3 = 4;
static const uint8_t DEVKIT_IP_4 = 1;

// ==========================
// PINOS - BTS7960
// ==========================

// Motor esquerdo
static const int L_RPWM = 25;
static const int L_LPWM = 26;
static const int L_REN  = 32;
static const int L_LEN  = 33;

// Motor direito
static const int R_RPWM = 27;
static const int R_LPWM = 14;
static const int R_REN  = 18;
static const int R_LEN  = 5;

// ==========================
// PINOS - D4184
// ==========================

static const int PIN_ESCOVA_FRENTE = 23;
static const int PIN_ESCOVA_TRAS   = 19;
static const int PIN_AGUA          = 4;

// ==========================
// CONFIGURAÇÃO PWM
// ==========================

static const int PWM_FREQ = 20000;
static const int PWM_RES  = 8;

// Limite inicial de PWM.
// Pode ser alterado depois na tela de configurações.
static const int PWM_MAX = 140;

// ==========================
// I2C / TCA9548A / SENSORES
// ==========================

static const int I2C_SDA = 21;
static const int I2C_SCL = 22;

static const uint8_t TCA_ADDR = 0x70;

static const uint8_t CANAL_AS5600_ESQ = 0;
static const uint8_t CANAL_AS5600_DIR = 1;

static const uint8_t CANAL_TOF_FRONTAL_ESQ = 2;
static const uint8_t CANAL_TOF_FRONTAL_DIR = 3;
static const uint8_t CANAL_TOF_TRASEIRO    = 4;

static const uint8_t AS5600_ADDR = 0x36;
static const uint8_t AS5600_REG_ANGULO = 0x0C;

static const uint16_t TOF_LIMIAR_PERIGO_MM = 80;
static const uint16_t TOF_VALOR_INVALIDO_MM = 8190;
static const unsigned long INTERVALO_LEITURA_SENSORES_MS = 200;

// ==========================
// CONTROLE PI DE VELOCIDADE
// ==========================

// Começa desligado. Assim o robô inicia em PWM direto.
static const bool CONTROLE_PI_INICIAL = false;

// RPM máximo inicial para o joystick em modo PI.
static const float RPM_MAX_INICIAL = 40.0;

// Ganhos iniciais suaves.
static const float KP_RPM_INICIAL = 0.50;
static const float KI_RPM_INICIAL = 0.05;

// Limite do termo integral para evitar acúmulo excessivo.
static const float INTEGRAL_RPM_MAX = 50.0;

// Período do controle PI.
static const unsigned long INTERVALO_CONTROLE_PI_MS = 100;

// Filtro da leitura de RPM.
static const float ALFA_FILTRO_RPM = 0.20;

// Inversão dos RPM.
// No seu teste:
// Frente -> RPM negativo nos dois lados.
// Ré     -> RPM positivo nos dois lados.
static const bool INVERTER_RPM_ESQ = true;
static const bool INVERTER_RPM_DIR = true;

// Zona morta adicional no ESP32 para evitar comando pequeno indesejado.
static const int ZONA_MORTA_JOY = 4;

// Após parar, ignora comandos residuais por alguns milissegundos.
static const unsigned long BLOQUEIO_JOY_APOS_PARADA_MS = 350;

// ==========================
// SEGURANÇA
// ==========================

static const unsigned long tempoSeguranca = 800; // ms sem comando -> para tração

#endif