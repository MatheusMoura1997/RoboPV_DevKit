#ifndef PAGINA_HTML_H
#define PAGINA_HTML_H

#include <Arduino.h>

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>RoboPV - Controle</title>

  <link rel="stylesheet" href="/style.css">
  <script src="/script.js"></script>
</head>

<body>
  <div class="app">
    <div class="camera-bg">
      <img src="http://192.168.4.2:81/stream" alt="Camera ESP32-CAM">
    </div>

    <div class="sombra-overlay"></div>

    <button id="btnCameraConfig" class="btn-camera-config" onclick="abrirConfigCamera()">⚙</button>
    <button id="btnFullscreen" class="btn-fullscreen" onclick="entrarTelaCheia()">⛶</button>

    <div id="painelCameraConfig" class="painel-camera-config">
      <div class="camera-config-topo">
        <select id="selectTipoConfig" class="select-tipo-config" onchange="trocarTipoConfig()">
          <option value="robo">Configurações do robô</option>
          <option value="camera">Configurações da câmera</option>
        </select>

        <button class="btn-fechar-config" onclick="fecharConfigCamera()">X</button>
      </div>

      <div id="conteudoConfigCamera" class="conteudo-config-camera">
        <iframe id="iframeCameraConfig" class="camera-config-frame" src=""></iframe>
      </div>

      <div id="conteudoConfigRobo" class="conteudo-config-robo">
        <div class="config-robo-card">
          <h2>Configurações do robô</h2>

          <div class="grupo-config-robo">
            <label for="inputPWMMax">PWM máximo dos motores</label>
            <div class="linha-config-robo">
              <input id="inputPWMMax" class="input-config-robo" type="number" min="0" max="255" step="1" value="140">
            </div>
            <p class="valor-config-robo">PWM atual: <span id="valorPWMMax">140</span></p>
          </div>

          <div class="grupo-config-robo">
            <label for="inputToFLimiar">Limiar de perigo dos sensores ToF (mm)</label>
            <div class="linha-config-robo">
              <input id="inputToFLimiar" class="input-config-robo" type="number" min="1" max="2000" step="1" value="80">
            </div>
            <p class="valor-config-robo">Limiar atual: <span id="valorToFLimiar">80</span> mm</p>
          </div>

          <div class="grupo-config-robo">
            <label>Controle de velocidade</label>
            <div class="linha-config-robo">
              <label style="display:flex; align-items:center; gap:8px; font-size:13px;">
                <input id="inputControlePI" type="checkbox">
                Ativar controle PI por RPM
              </label>
            </div>
            <p class="valor-config-robo">Modo atual: <span id="valorControlePI">PWM direto</span></p>
          </div>

          <div class="grupo-config-robo">
            <label for="inputRPMMax">RPM máximo das esteiras</label>
            <div class="linha-config-robo">
              <input id="inputRPMMax" class="input-config-robo" type="number" min="1" max="200" step="1" value="65">
            </div>
            <p class="valor-config-robo">RPM máximo atual: <span id="valorRPMMax">65</span> rpm</p>
          </div>

          <div class="grupo-config-robo">
            <label for="inputKpRPM">Ganho proporcional Kp</label>
            <div class="linha-config-robo">
              <input id="inputKpRPM" class="input-config-robo" type="number" min="0" max="20" step="0.05" value="1.00">
            </div>
            <p class="valor-config-robo">Kp atual: <span id="valorKpRPM">1.00</span></p>
          </div>

          <div class="grupo-config-robo">
            <label for="inputKiRPM">Ganho integral Ki</label>
            <div class="linha-config-robo">
              <input id="inputKiRPM" class="input-config-robo" type="number" min="0" max="10" step="0.05" value="0.25">
            </div>
            <p class="valor-config-robo">Ki atual: <span id="valorKiRPM">0.25</span></p>
          </div>

          <div class="acoes-config-robo">
            <button class="btn-salvar-config" onclick="salvarConfigRobo()">Salvar</button>
          </div>
        </div>
      </div>
    </div>

    <div class="topo">
      <h1 id="tituloPagina">RoboPV1</h1>
      <p id="subtituloPagina">Controle joystick X/Y</p>
    </div>

    <div class="painel-controle">

      <div class="joystick-area">
        <div 
          id="joystick" 
          class="joystick"
          onmousedown="iniciarJoystick(event)"
          ontouchstart="iniciarJoystick(event)"
        >
          <div id="knob" class="knob"></div>
        </div>

        <p class="dica">X/Y</p>
      </div>

      <div class="extra">
        <button id="btnEscovaFrente" onclick="alternarEscovaFrente()">Escova Dianteira OFF</button>
        <button id="btnEscovaTraseira" onclick="alternarEscovaTraseira()">Escova Traseira OFF</button>
        <button id="btnAgua" onclick="alternarAgua()">Agua OFF</button>
        <button class="parar" onclick="enviarEmergencia()">EMERGENCIA</button>
      </div>

    </div>

    <div id="alertaTofFrontalEsq" class="alerta-tof alerta-frontal-esq">
      <div class="linha-alerta"></div>
      <div class="valor-alerta">-- mm</div>
    </div>

    <div id="alertaTofFrontalDir" class="alerta-tof alerta-frontal-dir">
      <div class="linha-alerta"></div>
      <div class="valor-alerta">-- mm</div>
    </div>

    <div id="alertaTofTraseiro" class="alerta-tof alerta-traseiro">
      <div class="valor-alerta">-- mm</div>
      <div class="linha-alerta"></div>
    </div>

    <div class="velocidades-esteiras">
      <span>Esteira Esq: <strong id="velEsteiraEsq">-- rpm</strong></span>
      <span>Esteira Dir: <strong id="velEsteiraDir">-- rpm</strong></span>
      <span>Set Esq: <strong id="setEsteiraEsq">-- rpm</strong></span>
      <span>Set Dir: <strong id="setEsteiraDir">-- rpm</strong></span>
      <span>PWM Esq: <strong id="pwmEsteiraEsq">--</strong></span>
      <span>PWM Dir: <strong id="pwmEsteiraDir">--</strong></span>
      <span>Modo: <strong id="modoControleAtual">PWM</strong></span>
    </div>

    <div class="status">
      Último comando: <span id="status">HTML carregado</span>
    </div>
  </div>
</body>
</html>
)rawliteral";

#endif