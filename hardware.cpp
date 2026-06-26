#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <math.h>

#include "config.h"
#include "hardware.h"

// ==========================
// Variáveis globais de controle
// ==========================

String ultimoComando = "parar";
String ultimoComandoMovimento = "parar";

unsigned long ultimoTempoMovimento = 0;
unsigned long bloquearComandoJoyAte = 0;

bool escovaLigada = false;
bool escovaFrenteLigada = false;
bool escovaTraseiraLigada = false;
bool aguaLigada = false;

// Joystick
int joyXAtual = 0;
int joyYAtual = 0;

// Velocidade percentual de cada esteira
int velocidadeEsteiraEsquerda = 0;
int velocidadeEsteiraDireita  = 0;

// Reduz repetição no Serial
int ultimoJoyXSerial = 999;
int ultimoJoyYSerial = 999;

// PWM máximo ajustável em tempo de execução
int pwmMaxAtual = PWM_MAX;

// Limiar ajustável dos sensores ToF
uint16_t tofLimiarPerigoAtualMM = TOF_LIMIAR_PERIGO_MM;

// ==========================
// Sensores via TCA9548A
// ==========================

Adafruit_VL53L0X tofFrontalEsq = Adafruit_VL53L0X();
Adafruit_VL53L0X tofFrontalDir = Adafruit_VL53L0X();
Adafruit_VL53L0X tofTraseiro   = Adafruit_VL53L0X();

bool tofFrontalEsqOK = false;
bool tofFrontalDirOK = false;
bool tofTraseiroOK = false;

float velocidadeSensorEsteiraEsquerdaRPM = 0.0;
float velocidadeSensorEsteiraDireitaRPM = 0.0;

uint16_t distanciaToFFrontalEsqMM = 0;
uint16_t distanciaToFFrontalDirMM = 0;
uint16_t distanciaToFTraseiroMM = 0;

bool perigoToFFrontalEsq = false;
bool perigoToFFrontalDir = false;
bool perigoToFTraseiro = false;

int ultimoAnguloAS5600Esq = -1;
int ultimoAnguloAS5600Dir = -1;

unsigned long ultimoTempoAS5600Esq = 0;
unsigned long ultimoTempoAS5600Dir = 0;
unsigned long ultimoTempoLeituraSensores = 0;

// ==========================
// Controle PI
// ==========================

bool controlePIAtivo = CONTROLE_PI_INICIAL;

float rpmMaxAtual = RPM_MAX_INICIAL;
float kpRPMAtual = KP_RPM_INICIAL;
float kiRPMAtual = KI_RPM_INICIAL;

float setpointRPMEsq = 0.0;
float setpointRPMDir = 0.0;

float erroIntegralEsq = 0.0;
float erroIntegralDir = 0.0;

int pwmControleEsq = 0;
int pwmControleDir = 0;

unsigned long ultimoTempoControlePI = 0;

// ==========================
// Tabelas de calibração PWM x RPM
// ==========================

// Os pontos abaixo vieram do ensaio experimental que você fez.
// A função de interpolação usa essas tabelas para estimar o PWM base.

static const int NUM_PONTOS_CALIB = 19;

static const int pwmTabelaCalib[NUM_PONTOS_CALIB] = {
  0, 80, 90, 100, 110, 120, 130, 140, 150, 160,
  170, 180, 190, 200, 210, 220, 230, 240, 250
};

// Frente - esteira esquerda
static const float rpmFrenteEsq[NUM_PONTOS_CALIB] = {
  0.00, 2.30, 12.86, 16.80, 21.78, 24.69, 28.04, 31.27, 34.43, 37.48,
  40.82, 44.95, 49.47, 54.40, 60.95, 67.55, 74.03, 75.71, 75.96
};

// Frente - esteira direita
static const float rpmFrenteDir[NUM_PONTOS_CALIB] = {
  0.00, 9.30, 13.35, 16.88, 21.22, 24.44, 27.22, 31.03, 33.66, 36.68,
  40.38, 43.64, 46.84, 50.80, 55.06, 60.90, 67.88, 71.71, 71.84
};

// Ré - esteira esquerda
static const float rpmReEsq[NUM_PONTOS_CALIB] = {
  0.00, 15.62, 17.98, 21.59, 24.93, 28.11, 31.37, 34.60, 37.78, 41.14,
  44.71, 48.25, 51.63, 55.10, 59.02, 64.34, 70.37, 75.68, 75.88
};

// Ré - esteira direita
static const float rpmReDir[NUM_PONTOS_CALIB] = {
  0.00, 8.08, 12.48, 15.09, 19.33, 22.55, 25.19, 29.10, 32.11, 35.59,
  38.82, 42.04, 45.97, 49.56, 55.81, 60.53, 68.78, 70.23, 70.54
};

// ==========================
// Funções auxiliares
// ==========================

int limitarInt(int valor, int minimo, int maximo) {
  if (valor < minimo) return minimo;
  if (valor > maximo) return maximo;
  return valor;
}

int obterPWMMaximo() {
  return pwmMaxAtual;
}

void definirPWMMaximo(int novoPWMMaximo) {
  pwmMaxAtual = limitarInt(novoPWMMaximo, 0, 255);

  Serial.print("PWM MAXIMO ALTERADO: ");
  Serial.println(pwmMaxAtual);
}

uint16_t obterLimiarToFPerigoMM() {
  return tofLimiarPerigoAtualMM;
}

void definirLimiarToFPerigoMM(uint16_t novoLimiarMM) {
  tofLimiarPerigoAtualMM = limitarInt(novoLimiarMM, 1, 2000);

  Serial.print("LIMIAR TOF ALTERADO: ");
  Serial.print(tofLimiarPerigoAtualMM);
  Serial.println(" mm");
}

// ==========================
// Controle PI - configurações
// ==========================

bool obterControlePIAtivo() {
  return controlePIAtivo;
}

void definirControlePI(bool ativo) {
  controlePIAtivo = ativo;
  resetarControlePI();
  pararMovimento();

  Serial.print("CONTROLE PI: ");
  Serial.println(controlePIAtivo ? "ATIVO" : "DESATIVADO");
}

float obterRPMMaximo() {
  return rpmMaxAtual;
}

void definirRPMMaximo(float novoRPMMaximo) {
  if (novoRPMMaximo < 1.0) novoRPMMaximo = 1.0;
  if (novoRPMMaximo > 200.0) novoRPMMaximo = 200.0;

  rpmMaxAtual = novoRPMMaximo;
  resetarControlePI();

  Serial.print("RPM MAXIMO ALTERADO: ");
  Serial.println(rpmMaxAtual);
}

float obterKpRPM() {
  return kpRPMAtual;
}

void definirKpRPM(float novoKp) {
  if (novoKp < 0.0) novoKp = 0.0;
  if (novoKp > 20.0) novoKp = 20.0;

  kpRPMAtual = novoKp;
  resetarControlePI();

  Serial.print("KP RPM ALTERADO: ");
  Serial.println(kpRPMAtual);
}

float obterKiRPM() {
  return kiRPMAtual;
}

void definirKiRPM(float novoKi) {
  if (novoKi < 0.0) novoKi = 0.0;
  if (novoKi > 10.0) novoKi = 10.0;

  kiRPMAtual = novoKi;
  resetarControlePI();

  Serial.print("KI RPM ALTERADO: ");
  Serial.println(kiRPMAtual);
}

void resetarControlePI() {
  erroIntegralEsq = 0.0;
  erroIntegralDir = 0.0;

  pwmControleEsq = 0;
  pwmControleDir = 0;

  ultimoTempoControlePI = millis();
}

// ==========================
// Interpolação da tabela de calibração
// ==========================

float interpolarPWM(float rpmDesejado, const float tabelaRPM[], const int tabelaPWM[], int tamanho) {
  if (rpmDesejado <= 0.5) {
    return 0.0;
  }

  if (rpmDesejado >= tabelaRPM[tamanho - 1]) {
    return (float)tabelaPWM[tamanho - 1];
  }

  for (int i = 1; i < tamanho; i++) {
    if (rpmDesejado <= tabelaRPM[i]) {
      float rpm0 = tabelaRPM[i - 1];
      float rpm1 = tabelaRPM[i];

      float pwm0 = (float)tabelaPWM[i - 1];
      float pwm1 = (float)tabelaPWM[i];

      if (fabs(rpm1 - rpm0) < 0.01) {
        return pwm1;
      }

      float proporcao = (rpmDesejado - rpm0) / (rpm1 - rpm0);
      return pwm0 + proporcao * (pwm1 - pwm0);
    }
  }

  return (float)tabelaPWM[tamanho - 1];
}

int calcularPWMBasePorTabela(float setpointRPM, bool esteiraEsquerda) {
  float rpmAbs = fabs(setpointRPM);

  if (rpmAbs <= 0.5) {
    return 0;
  }

  float pwmBase = 0.0;

  if (setpointRPM > 0.0) {
    if (esteiraEsquerda) {
      pwmBase = interpolarPWM(rpmAbs, rpmFrenteEsq, pwmTabelaCalib, NUM_PONTOS_CALIB);
    } else {
      pwmBase = interpolarPWM(rpmAbs, rpmFrenteDir, pwmTabelaCalib, NUM_PONTOS_CALIB);
    }
  } else {
    if (esteiraEsquerda) {
      pwmBase = interpolarPWM(rpmAbs, rpmReEsq, pwmTabelaCalib, NUM_PONTOS_CALIB);
    } else {
      pwmBase = interpolarPWM(rpmAbs, rpmReDir, pwmTabelaCalib, NUM_PONTOS_CALIB);
    }
  }

  int pwmInteiro = (int)round(pwmBase);

  if (setpointRPM < 0.0) {
    pwmInteiro = -pwmInteiro;
  }

  return limitarInt(pwmInteiro, -pwmMaxAtual, pwmMaxAtual);
}

// ==========================
// Sensores TCA9548A / AS5600 / VL53L0X
// ==========================

void selecionarCanalTCA(uint8_t canal) {
  if (canal > 7) return;

  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << canal);
  Wire.endTransmission();
}

int lerAS5600Raw(uint8_t canal) {
  selecionarCanalTCA(canal);

  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(AS5600_REG_ANGULO);

  if (Wire.endTransmission(false) != 0) {
    return -1;
  }

  uint8_t bytesRecebidos = Wire.requestFrom((uint8_t)AS5600_ADDR, (uint8_t)2);

  if (bytesRecebidos != 2) {
    return -1;
  }

  uint16_t msb = Wire.read();
  uint16_t lsb = Wire.read();

  return ((msb & 0x0F) << 8) | lsb;
}

float calcularRPMAS5600(int anguloAtual, int &ultimoAngulo, unsigned long &ultimoTempo) {
  unsigned long agora = millis();

  if (anguloAtual < 0) {
    return 0.0;
  }

  if (ultimoAngulo < 0 || ultimoTempo == 0) {
    ultimoAngulo = anguloAtual;
    ultimoTempo = agora;
    return 0.0;
  }

  unsigned long dtMs = agora - ultimoTempo;

  if (dtMs == 0) {
    return 0.0;
  }

  int delta = anguloAtual - ultimoAngulo;

  if (delta > 2048) {
    delta -= 4096;
  } else if (delta < -2048) {
    delta += 4096;
  }

  ultimoAngulo = anguloAtual;
  ultimoTempo = agora;

  float voltas = delta / 4096.0;
  float dtS = dtMs / 1000.0;

  return (voltas / dtS) * 60.0;
}

uint16_t lerToFCanal(uint8_t canal) {
  Adafruit_VL53L0X *sensor = nullptr;
  bool sensorOK = false;

  if (canal == CANAL_TOF_FRONTAL_ESQ) {
    sensor = &tofFrontalEsq;
    sensorOK = tofFrontalEsqOK;
  } else if (canal == CANAL_TOF_FRONTAL_DIR) {
    sensor = &tofFrontalDir;
    sensorOK = tofFrontalDirOK;
  } else if (canal == CANAL_TOF_TRASEIRO) {
    sensor = &tofTraseiro;
    sensorOK = tofTraseiroOK;
  }

  if (sensor == nullptr || !sensorOK) {
    return TOF_VALOR_INVALIDO_MM;
  }

  selecionarCanalTCA(canal);

  VL53L0X_RangingMeasurementData_t medida;
  sensor->rangingTest(&medida, false);

  if (medida.RangeStatus != 4) {
    return medida.RangeMilliMeter;
  }

  return TOF_VALOR_INVALIDO_MM;
}

void configurarSensores() {
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("Inicializando sensores I2C via TCA9548A...");

  selecionarCanalTCA(CANAL_TOF_FRONTAL_ESQ);
  tofFrontalEsqOK = tofFrontalEsq.begin();

  Serial.print("ToF frontal esquerdo: ");
  Serial.println(tofFrontalEsqOK ? "OK" : "FALHA");

  selecionarCanalTCA(CANAL_TOF_FRONTAL_DIR);
  tofFrontalDirOK = tofFrontalDir.begin();

  Serial.print("ToF frontal direito: ");
  Serial.println(tofFrontalDirOK ? "OK" : "FALHA");

  selecionarCanalTCA(CANAL_TOF_TRASEIRO);
  tofTraseiroOK = tofTraseiro.begin();

  Serial.print("ToF traseiro: ");
  Serial.println(tofTraseiroOK ? "OK" : "FALHA");

  int as5600Esq = lerAS5600Raw(CANAL_AS5600_ESQ);
  int as5600Dir = lerAS5600Raw(CANAL_AS5600_DIR);

  Serial.print("AS5600 esquerdo inicial: ");
  if (as5600Esq >= 0) Serial.println(as5600Esq);
  else Serial.println("FALHA");

  Serial.print("AS5600 direito inicial: ");
  if (as5600Dir >= 0) Serial.println(as5600Dir);
  else Serial.println("FALHA");
}

void atualizarSensores() {
  unsigned long agora = millis();

  if (agora - ultimoTempoLeituraSensores < INTERVALO_LEITURA_SENSORES_MS) {
    return;
  }

  ultimoTempoLeituraSensores = agora;

  int anguloEsq = lerAS5600Raw(CANAL_AS5600_ESQ);
  int anguloDir = lerAS5600Raw(CANAL_AS5600_DIR);

  float rpmInstEsq = calcularRPMAS5600(anguloEsq, ultimoAnguloAS5600Esq, ultimoTempoAS5600Esq);
  float rpmInstDir = calcularRPMAS5600(anguloDir, ultimoAnguloAS5600Dir, ultimoTempoAS5600Dir);

  if (INVERTER_RPM_ESQ) rpmInstEsq = -rpmInstEsq;
  if (INVERTER_RPM_DIR) rpmInstDir = -rpmInstDir;

  velocidadeSensorEsteiraEsquerdaRPM =
    ALFA_FILTRO_RPM * rpmInstEsq + (1.0 - ALFA_FILTRO_RPM) * velocidadeSensorEsteiraEsquerdaRPM;

  velocidadeSensorEsteiraDireitaRPM =
    ALFA_FILTRO_RPM * rpmInstDir + (1.0 - ALFA_FILTRO_RPM) * velocidadeSensorEsteiraDireitaRPM;

  distanciaToFFrontalEsqMM = lerToFCanal(CANAL_TOF_FRONTAL_ESQ);
  distanciaToFFrontalDirMM = lerToFCanal(CANAL_TOF_FRONTAL_DIR);
  distanciaToFTraseiroMM   = lerToFCanal(CANAL_TOF_TRASEIRO);

  perigoToFFrontalEsq = distanciaToFFrontalEsqMM >= tofLimiarPerigoAtualMM;
  perigoToFFrontalDir = distanciaToFFrontalDirMM >= tofLimiarPerigoAtualMM;
  perigoToFTraseiro   = distanciaToFTraseiroMM >= tofLimiarPerigoAtualMM;
}

String obterJSONSensores() {
  String resposta = "{";

  resposta += "\"vel_esq_rpm\":";
  resposta += String(velocidadeSensorEsteiraEsquerdaRPM, 1);

  resposta += ",\"vel_dir_rpm\":";
  resposta += String(velocidadeSensorEsteiraDireitaRPM, 1);

  resposta += ",\"set_esq_rpm\":";
  resposta += String(setpointRPMEsq, 1);

  resposta += ",\"set_dir_rpm\":";
  resposta += String(setpointRPMDir, 1);

  resposta += ",\"pwm_esq\":";
  resposta += String(pwmControleEsq);

  resposta += ",\"pwm_dir\":";
  resposta += String(pwmControleDir);

  resposta += ",\"controle_pi\":";
  resposta += controlePIAtivo ? "true" : "false";

  resposta += ",\"rpm_max\":";
  resposta += String(rpmMaxAtual, 1);

  resposta += ",\"kp_rpm\":";
  resposta += String(kpRPMAtual, 2);

  resposta += ",\"ki_rpm\":";
  resposta += String(kiRPMAtual, 2);

  resposta += ",\"tof_fe_mm\":";
  resposta += String(distanciaToFFrontalEsqMM);

  resposta += ",\"tof_fd_mm\":";
  resposta += String(distanciaToFFrontalDirMM);

  resposta += ",\"tof_tras_mm\":";
  resposta += String(distanciaToFTraseiroMM);

  resposta += ",\"tof_fe_perigo\":";
  resposta += perigoToFFrontalEsq ? "true" : "false";

  resposta += ",\"tof_fd_perigo\":";
  resposta += perigoToFFrontalDir ? "true" : "false";

  resposta += ",\"tof_tras_perigo\":";
  resposta += perigoToFTraseiro ? "true" : "false";

  resposta += ",\"limiar_tof_mm\":";
  resposta += String(tofLimiarPerigoAtualMM);

  resposta += "}";

  return resposta;
}

// ==========================
// Configuração geral do hardware
// ==========================

void configurarHardware() {
  configurarMotores();
  configurarSensores();

  pinMode(PIN_ESCOVA_FRENTE, OUTPUT);
  pinMode(PIN_ESCOVA_TRAS, OUTPUT);
  pinMode(PIN_AGUA, OUTPUT);

  digitalWrite(PIN_ESCOVA_FRENTE, LOW);
  digitalWrite(PIN_ESCOVA_TRAS, LOW);
  digitalWrite(PIN_AGUA, LOW);
}

// ==========================
// Configuração dos motores
// ==========================

void configurarMotores() {
  pinMode(L_REN, OUTPUT);
  pinMode(L_LEN, OUTPUT);
  pinMode(R_REN, OUTPUT);
  pinMode(R_LEN, OUTPUT);

  digitalWrite(L_REN, HIGH);
  digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH);
  digitalWrite(R_LEN, HIGH);

  ledcAttach(L_RPWM, PWM_FREQ, PWM_RES);
  ledcAttach(L_LPWM, PWM_FREQ, PWM_RES);
  ledcAttach(R_RPWM, PWM_FREQ, PWM_RES);
  ledcAttach(R_LPWM, PWM_FREQ, PWM_RES);

  ledcWrite(L_RPWM, 0);
  ledcWrite(L_LPWM, 0);
  ledcWrite(R_RPWM, 0);
  ledcWrite(R_LPWM, 0);
}

void motorEsquerdoPWM(int pwm) {
  pwm = limitarInt(pwm, -255, 255);

  if (pwm > 0) {
    ledcWrite(L_RPWM, pwm);
    ledcWrite(L_LPWM, 0);
  } else if (pwm < 0) {
    ledcWrite(L_RPWM, 0);
    ledcWrite(L_LPWM, -pwm);
  } else {
    ledcWrite(L_RPWM, 0);
    ledcWrite(L_LPWM, 0);
  }
}

void motorDireitoPWM(int pwm) {
  pwm = limitarInt(pwm, -255, 255);

  if (pwm > 0) {
    ledcWrite(R_RPWM, pwm);
    ledcWrite(R_LPWM, 0);
  } else if (pwm < 0) {
    ledcWrite(R_RPWM, 0);
    ledcWrite(R_LPWM, -pwm);
  } else {
    ledcWrite(R_RPWM, 0);
    ledcWrite(R_LPWM, 0);
  }
}

void aplicarMotores(int esquerdaPercent, int direitaPercent) {
  esquerdaPercent = limitarInt(esquerdaPercent, -100, 100);
  direitaPercent  = limitarInt(direitaPercent, -100, 100);

  velocidadeEsteiraEsquerda = esquerdaPercent;
  velocidadeEsteiraDireita  = direitaPercent;

  if (!controlePIAtivo) {
    int pwmEsq = map(abs(esquerdaPercent), 0, 100, 0, pwmMaxAtual);
    int pwmDir = map(abs(direitaPercent),  0, 100, 0, pwmMaxAtual);

    if (esquerdaPercent < 0) pwmEsq = -pwmEsq;
    if (direitaPercent  < 0) pwmDir = -pwmDir;

    setpointRPMEsq = 0.0;
    setpointRPMDir = 0.0;

    pwmControleEsq = pwmEsq;
    pwmControleDir = pwmDir;

    motorEsquerdoPWM(pwmEsq);
    motorDireitoPWM(pwmDir);

    return;
  }

  setpointRPMEsq = (esquerdaPercent / 100.0) * rpmMaxAtual;
  setpointRPMDir = (direitaPercent  / 100.0) * rpmMaxAtual;

  if (esquerdaPercent == 0 && direitaPercent == 0) {
    pararMovimento();
  }
}

void atualizarControlePI() {
  if (!controlePIAtivo) {
    return;
  }

  unsigned long agora = millis();

  if (agora - ultimoTempoControlePI < INTERVALO_CONTROLE_PI_MS) {
    return;
  }

  float dt = (agora - ultimoTempoControlePI) / 1000.0;
  ultimoTempoControlePI = agora;

  if (dt <= 0.0) {
    return;
  }

  bool setEsqZero = (setpointRPMEsq > -0.5 && setpointRPMEsq < 0.5);
  bool setDirZero = (setpointRPMDir > -0.5 && setpointRPMDir < 0.5);

  if (setEsqZero && setDirZero) {
    resetarControlePI();
    motorEsquerdoPWM(0);
    motorDireitoPWM(0);
    return;
  }

  // ==========================
  // Esteira esquerda
  // ==========================

  if (setEsqZero) {
    erroIntegralEsq = 0.0;
    pwmControleEsq = 0;
    motorEsquerdoPWM(0);
  } else {
    float erroEsq = setpointRPMEsq - velocidadeSensorEsteiraEsquerdaRPM;

    erroIntegralEsq += erroEsq * dt;
    erroIntegralEsq = constrain(erroIntegralEsq, -INTEGRAL_RPM_MAX, INTEGRAL_RPM_MAX);

    int pwmBaseEsq = calcularPWMBasePorTabela(setpointRPMEsq, true);
    float correcaoEsq = kpRPMAtual * erroEsq + kiRPMAtual * erroIntegralEsq;

    int pwmSaidaEsq = (int)(pwmBaseEsq + correcaoEsq);
    pwmSaidaEsq = limitarInt(pwmSaidaEsq, -pwmMaxAtual, pwmMaxAtual);

    pwmControleEsq = pwmSaidaEsq;
    motorEsquerdoPWM(pwmSaidaEsq);
  }

  // ==========================
  // Esteira direita
  // ==========================

  if (setDirZero) {
    erroIntegralDir = 0.0;
    pwmControleDir = 0;
    motorDireitoPWM(0);
  } else {
    float erroDir = setpointRPMDir - velocidadeSensorEsteiraDireitaRPM;

    erroIntegralDir += erroDir * dt;
    erroIntegralDir = constrain(erroIntegralDir, -INTEGRAL_RPM_MAX, INTEGRAL_RPM_MAX);

    int pwmBaseDir = calcularPWMBasePorTabela(setpointRPMDir, false);
    float correcaoDir = kpRPMAtual * erroDir + kiRPMAtual * erroIntegralDir;

    int pwmSaidaDir = (int)(pwmBaseDir + correcaoDir);
    pwmSaidaDir = limitarInt(pwmSaidaDir, -pwmMaxAtual, pwmMaxAtual);

    pwmControleDir = pwmSaidaDir;
    motorDireitoPWM(pwmSaidaDir);
  }
}

// ==========================
// Movimento
// ==========================

void pararMovimento() {
  if (ultimoComandoMovimento != "parar") {
    Serial.println("MOVIMENTO: PARAR");
  }

  ultimoComandoMovimento = "parar";
  ultimoTempoMovimento = millis();

  bloquearComandoJoyAte = millis() + BLOQUEIO_JOY_APOS_PARADA_MS;

  joyXAtual = 0;
  joyYAtual = 0;

  velocidadeEsteiraEsquerda = 0;
  velocidadeEsteiraDireita = 0;

  setpointRPMEsq = 0.0;
  setpointRPMDir = 0.0;

  erroIntegralEsq = 0.0;
  erroIntegralDir = 0.0;

  pwmControleEsq = 0;
  pwmControleDir = 0;

  motorEsquerdoPWM(0);
  motorDireitoPWM(0);
}

void aplicarJoystickXY(int x, int y) {
  x = limitarInt(x, -100, 100);
  y = limitarInt(y, -100, 100);

  if (abs(x) <= ZONA_MORTA_JOY) x = 0;
  if (abs(y) <= ZONA_MORTA_JOY) y = 0;

  if (x == 0 && y == 0) {
    pararMovimento();
    return;
  }

  if (millis() < bloquearComandoJoyAte) {
    Serial.print("JOY RESIDUAL IGNORADO X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.println(y);
    return;
  }

  joyXAtual = x;
  joyYAtual = y;

  ultimoComandoMovimento = "joy";
  ultimoTempoMovimento = millis();

  int esquerda = y + x;
  int direita  = y - x;

  int maior = max(abs(esquerda), abs(direita));

  if (maior > 100) {
    esquerda = esquerda * 100 / maior;
    direita  = direita  * 100 / maior;
  }

  velocidadeEsteiraEsquerda = limitarInt(esquerda, -100, 100);
  velocidadeEsteiraDireita  = limitarInt(direita, -100, 100);

  aplicarMotores(velocidadeEsteiraEsquerda, velocidadeEsteiraDireita);

  if (x != ultimoJoyXSerial || y != ultimoJoyYSerial) {
    ultimoJoyXSerial = x;
    ultimoJoyYSerial = y;

    Serial.print("JOY X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.print(y);
    Serial.print(" | Esteira Esq: ");
    Serial.print(velocidadeEsteiraEsquerda);
    Serial.print(" | Esteira Dir: ");
    Serial.print(velocidadeEsteiraDireita);
    Serial.print(" | Modo: ");
    Serial.println(controlePIAtivo ? "PI" : "PWM");
  }
}

void verificarSegurancaMovimento() {
  if (ultimoComandoMovimento != "parar") {
    if (millis() - ultimoTempoMovimento > tempoSeguranca) {
      Serial.println("SEGURANCA: sem comando X/Y recente. Parando tracao.");
      pararMovimento();
    }
  }
}

// ==========================
// Escovas e água
// ==========================

void ligarEscova() {
  ligarEscovaFrente();
  ligarEscovaTraseira();
}

void desligarEscova() {
  desligarEscovaFrente();
  desligarEscovaTraseira();
}

void ligarEscovaFrente() {
  escovaFrenteLigada = true;
  escovaLigada = escovaFrenteLigada || escovaTraseiraLigada;
  digitalWrite(PIN_ESCOVA_FRENTE, HIGH);
  Serial.println("ESCOVA FRENTE: LIGADA");
}

void desligarEscovaFrente() {
  escovaFrenteLigada = false;
  escovaLigada = escovaFrenteLigada || escovaTraseiraLigada;
  digitalWrite(PIN_ESCOVA_FRENTE, LOW);
  Serial.println("ESCOVA FRENTE: DESLIGADA");
}

void ligarEscovaTraseira() {
  escovaTraseiraLigada = true;
  escovaLigada = escovaFrenteLigada || escovaTraseiraLigada;
  digitalWrite(PIN_ESCOVA_TRAS, HIGH);
  Serial.println("ESCOVA TRASEIRA: LIGADA");
}

void desligarEscovaTraseira() {
  escovaTraseiraLigada = false;
  escovaLigada = escovaFrenteLigada || escovaTraseiraLigada;
  digitalWrite(PIN_ESCOVA_TRAS, LOW);
  Serial.println("ESCOVA TRASEIRA: DESLIGADA");
}

void ligarAgua() {
  aguaLigada = true;
  digitalWrite(PIN_AGUA, HIGH);
  Serial.println("AGUA: LIGADA");
}

void desligarAgua() {
  aguaLigada = false;
  digitalWrite(PIN_AGUA, LOW);
  Serial.println("AGUA: DESLIGADA");
}

void pararTudo() {
  Serial.println("EMERGENCIA: PARANDO TUDO");

  pararMovimento();
  desligarEscova();
  desligarAgua();
}

// ==========================
// Comandos
// ==========================

void executarComando(String comando) {
  ultimoComando = comando;

  Serial.print("COMANDO RECEBIDO: ");
  Serial.println(comando);

  if (comando == "parar") {
    pararMovimento();
  }
  else if (comando == "escova_on") {
    ligarEscova();
  }
  else if (comando == "escova_off") {
    desligarEscova();
  }
  else if (comando == "escova_frente_on") {
    ligarEscovaFrente();
  }
  else if (comando == "escova_frente_off") {
    desligarEscovaFrente();
  }
  else if (comando == "escova_traseira_on") {
    ligarEscovaTraseira();
  }
  else if (comando == "escova_traseira_off") {
    desligarEscovaTraseira();
  }
  else if (comando == "agua_on") {
    ligarAgua();
  }
  else if (comando == "agua_off") {
    desligarAgua();
  }
  else if (comando == "emergencia") {
    pararTudo();
  }
}