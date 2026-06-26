#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>

// ==========================
// Variáveis globais de controle
// ==========================

extern String ultimoComando;
extern String ultimoComandoMovimento;

extern unsigned long ultimoTempoMovimento;

extern bool escovaLigada;
extern bool escovaFrenteLigada;
extern bool escovaTraseiraLigada;
extern bool aguaLigada;

extern int joyXAtual;
extern int joyYAtual;

extern int velocidadeEsteiraEsquerda;
extern int velocidadeEsteiraDireita;

extern int ultimoJoyXSerial;
extern int ultimoJoyYSerial;

extern int pwmMaxAtual;
extern uint16_t tofLimiarPerigoAtualMM;

// ==========================
// Variáveis dos sensores
// ==========================

extern float velocidadeSensorEsteiraEsquerdaRPM;
extern float velocidadeSensorEsteiraDireitaRPM;

extern uint16_t distanciaToFFrontalEsqMM;
extern uint16_t distanciaToFFrontalDirMM;
extern uint16_t distanciaToFTraseiroMM;

extern bool perigoToFFrontalEsq;
extern bool perigoToFFrontalDir;
extern bool perigoToFTraseiro;

// ==========================
// Variáveis do controle PI
// ==========================

extern bool controlePIAtivo;

extern float rpmMaxAtual;
extern float kpRPMAtual;
extern float kiRPMAtual;

extern float setpointRPMEsq;
extern float setpointRPMDir;

extern float erroIntegralEsq;
extern float erroIntegralDir;

extern int pwmControleEsq;
extern int pwmControleDir;

// ==========================
// Funções auxiliares
// ==========================

int limitarInt(int valor, int minimo, int maximo);

int obterPWMMaximo();
void definirPWMMaximo(int novoPWMMaximo);

uint16_t obterLimiarToFPerigoMM();
void definirLimiarToFPerigoMM(uint16_t novoLimiarMM);

// ==========================
// Controle PI
// ==========================

bool obterControlePIAtivo();
void definirControlePI(bool ativo);

float obterRPMMaximo();
void definirRPMMaximo(float novoRPMMaximo);

float obterKpRPM();
void definirKpRPM(float novoKp);

float obterKiRPM();
void definirKiRPM(float novoKi);

void resetarControlePI();
void atualizarControlePI();

// ==========================
// Hardware
// ==========================

void configurarHardware();

void configurarMotores();
void motorEsquerdoPWM(int pwm);
void motorDireitoPWM(int pwm);
void aplicarMotores(int esquerdaPercent, int direitaPercent);

// ==========================
// Sensores
// ==========================

void configurarSensores();
void atualizarSensores();
String obterJSONSensores();

void selecionarCanalTCA(uint8_t canal);
int lerAS5600Raw(uint8_t canal);
float calcularRPMAS5600(int anguloAtual, int &ultimoAngulo, unsigned long &ultimoTempo);
uint16_t lerToFCanal(uint8_t canal);

// ==========================
// Movimento
// ==========================

void pararMovimento();
void aplicarJoystickXY(int x, int y);
void verificarSegurancaMovimento();

// ==========================
// Escovas e água
// ==========================

void ligarEscova();
void desligarEscova();

void ligarEscovaFrente();
void desligarEscovaFrente();

void ligarEscovaTraseira();
void desligarEscovaTraseira();

void ligarAgua();
void desligarAgua();

void pararTudo();

void executarComando(String comando);

#endif