#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "config.h"
#include "hardware.h"
#include "pagina_html.h"
#include "style_css.h"
#include "script_js.h"
#include "servidor.h"

WebServer server(80);

// Controle de sequência do joystick.
// Evita aceitar comandos antigos que chegaram atrasados.
long sessaoJoyAtual = -1;
long ultimoSeqJoyAceito = 0;

// ==========================
// Funções auxiliares
// ==========================

void enviarCabecalhosSemCache() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
}

// ==========================
// Wi-Fi
// ==========================

void configurarWiFi() {
  IPAddress local_IP(DEVKIT_IP_1, DEVKIT_IP_2, DEVKIT_IP_3, DEVKIT_IP_4);
  IPAddress gateway(DEVKIT_IP_1, DEVKIT_IP_2, DEVKIT_IP_3, DEVKIT_IP_4);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Rede Wi-Fi criada:");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("Senha: ");
  Serial.println(WIFI_PASSWORD);
  Serial.print("IP do ESP32 DevKit: ");
  Serial.println(WiFi.softAPIP());
}

// ==========================
// Handlers
// ==========================

void handlePagina() {
  Serial.println("Pagina HTML enviada ao navegador.");

  enviarCabecalhosSemCache();
  server.send_P(200, "text/html", paginaHTML);
}

void handleCSS() {
  enviarCabecalhosSemCache();
  server.send_P(200, "text/css", styleCSS);
}

void handleJS() {
  enviarCabecalhosSemCache();
  server.send_P(200, "application/javascript", scriptJS);
}

void handleComando() {
  if (server.hasArg("acao")) {
    String comando = server.arg("acao");
    executarComando(comando);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Comando ausente");
  }
}

void handleJoy() {
  if (!server.hasArg("x") || !server.hasArg("y")) {
    server.send(400, "text/plain", "Parametros X/Y ausentes");
    return;
  }

  // Proteção contra comandos antigos chegando atrasados.
  if (server.hasArg("seq") && server.hasArg("sess")) {
    long seqRecebido = server.arg("seq").toInt();
    long sessaoRecebida = server.arg("sess").toInt();

    if (sessaoJoyAtual < 0) {
      sessaoJoyAtual = sessaoRecebida;
      ultimoSeqJoyAceito = 0;
    }

    if (sessaoRecebida < sessaoJoyAtual) {
      Serial.print("JOY SESSAO ANTIGA IGNORADA sess=");
      Serial.print(sessaoRecebida);
      Serial.print(" atual=");
      Serial.println(sessaoJoyAtual);

      server.send(200, "text/plain", "JOY antigo ignorado");
      return;
    }

    if (sessaoRecebida > sessaoJoyAtual) {
      sessaoJoyAtual = sessaoRecebida;
      ultimoSeqJoyAceito = 0;

      Serial.print("NOVA SESSAO JOY: ");
      Serial.println(sessaoJoyAtual);
    }

    if (seqRecebido <= ultimoSeqJoyAceito) {
      Serial.print("JOY SEQ ANTIGO IGNORADO seq=");
      Serial.print(seqRecebido);
      Serial.print(" ultimo=");
      Serial.println(ultimoSeqJoyAceito);

      server.send(200, "text/plain", "JOY antigo ignorado");
      return;
    }

    ultimoSeqJoyAceito = seqRecebido;
  }

  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  aplicarJoystickXY(x, y);

  String resposta = "X=";
  resposta += String(joyXAtual);
  resposta += " Y=";
  resposta += String(joyYAtual);
  resposta += " Esq=";
  resposta += String(velocidadeEsteiraEsquerda);
  resposta += " Dir=";
  resposta += String(velocidadeEsteiraDireita);
  resposta += " PWMmax=";
  resposta += String(obterPWMMaximo());
  resposta += " Modo=";
  resposta += obterControlePIAtivo() ? "PI" : "PWM";

  server.send(200, "text/plain", resposta);
}

void handleSensores() {
  server.send(200, "application/json", obterJSONSensores());
}

void handleRoboConfig() {
  if (server.hasArg("pwm_max")) {
    int novoPWMMaximo = server.arg("pwm_max").toInt();
    definirPWMMaximo(novoPWMMaximo);
  }

  if (server.hasArg("tof_limiar_mm")) {
    uint16_t novoLimiarToF = server.arg("tof_limiar_mm").toInt();
    definirLimiarToFPerigoMM(novoLimiarToF);
  }

  if (server.hasArg("controle_pi")) {
    int valor = server.arg("controle_pi").toInt();
    definirControlePI(valor == 1);
  }

  if (server.hasArg("rpm_max")) {
    float novoRPMMax = server.arg("rpm_max").toFloat();
    definirRPMMaximo(novoRPMMax);
  }

  if (server.hasArg("kp_rpm")) {
    float novoKp = server.arg("kp_rpm").toFloat();
    definirKpRPM(novoKp);
  }

  if (server.hasArg("ki_rpm")) {
    float novoKi = server.arg("ki_rpm").toFloat();
    definirKiRPM(novoKi);
  }

  String resposta = "{";

  resposta += "\"pwm_max\":";
  resposta += String(obterPWMMaximo());

  resposta += ",\"tof_limiar_mm\":";
  resposta += String(obterLimiarToFPerigoMM());

  resposta += ",\"controle_pi\":";
  resposta += obterControlePIAtivo() ? "true" : "false";

  resposta += ",\"rpm_max\":";
  resposta += String(obterRPMMaximo(), 1);

  resposta += ",\"kp_rpm\":";
  resposta += String(obterKpRPM(), 2);

  resposta += ",\"ki_rpm\":";
  resposta += String(obterKiRPM(), 2);

  resposta += "}";

  server.send(200, "application/json", resposta);
}

// ==========================
// Configuração do servidor
// ==========================

void configurarServidor() {
  server.on("/", handlePagina);
  server.on("/style.css", handleCSS);
  server.on("/script.js", handleJS);
  server.on("/cmd", handleComando);
  server.on("/joy", handleJoy);
  server.on("/robo_config", handleRoboConfig);
  server.on("/sensores", handleSensores);

  server.begin();

  Serial.println("Servidor web iniciado.");
  Serial.println("Acesse no navegador:");
  Serial.println("http://192.168.4.1");
}

void processarServidorWeb() {
  server.handleClient();
}