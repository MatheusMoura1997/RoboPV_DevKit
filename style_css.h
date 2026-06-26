#ifndef STYLE_CSS_H
#define STYLE_CSS_H

#include <Arduino.h>

const char styleCSS[] PROGMEM = R"rawliteral(
* {
  box-sizing: border-box;
  -webkit-tap-highlight-color: transparent;
}

html, body {
  width: 100%;
  height: 100%;
  margin: 0;
  padding: 0;
}

body {
  font-family: Arial, sans-serif;
  background: #000;
  color: white;
  text-align: center;
  user-select: none;
  overflow: hidden;
}

.app {
  width: 100vw;
  height: 100vh;
  position: relative;
  overflow: hidden;
  background: #000;
}

.camera-bg {
  position: fixed;
  inset: 0;
  width: 100vw;
  height: 100vh;
  z-index: 1;
  background: #000;
  overflow: hidden;
  pointer-events: none;
}

.camera-bg img {
  width: 100%;
  height: 100%;
  object-fit: cover;
  display: block;
}

.sombra-overlay {
  position: fixed;
  inset: 0;
  z-index: 2;
  pointer-events: none;
  background:
    linear-gradient(to bottom, rgba(0,0,0,0.35), rgba(0,0,0,0.05) 35%, rgba(0,0,0,0.55) 100%);
}

.topo {
  position: fixed;
  top: 8px;
  left: 60px;
  z-index: 20;
  text-align: left;
  background: rgba(0,0,0,0.45);
  padding: 6px 10px;
  border-radius: 10px;
  backdrop-filter: blur(3px);
}

h1 {
  margin: 0;
  font-size: 17px;
  line-height: 1.1;
}

#subtituloPagina {
  margin: 2px 0 0 0;
  font-size: 11px;
  color: #d0d0d0;
}

.btn-camera-config {
  position: fixed;
  top: 8px;
  left: 8px;
  z-index: 30;
  width: 42px;
  height: 42px;
  padding: 0;
  border-radius: 12px;
  border: 1px solid rgba(255,255,255,0.20);
  background: rgba(0,0,0,0.62);
  color: white;
  font-size: 22px;
  font-weight: bold;
  backdrop-filter: blur(3px);
}

.btn-camera-config:active {
  background: rgba(60,60,60,0.9);
  transform: scale(0.96);
}

.btn-fullscreen {
  position: fixed;
  top: 8px;
  right: 8px;
  z-index: 30;
  width: 42px;
  height: 42px;
  padding: 0;
  border-radius: 12px;
  border: 1px solid rgba(255,255,255,0.20);
  background: rgba(0,0,0,0.62);
  color: white;
  font-size: 23px;
  font-weight: bold;
  backdrop-filter: blur(3px);
}

.btn-fullscreen:active {
  background: rgba(60,60,60,0.9);
  transform: scale(0.96);
}

.painel-camera-config {
  position: fixed;
  inset: 0;
  z-index: 80;
  display: none;
  background: rgba(0,0,0,0.82);
  padding: 50px 8px 8px 8px;
}

.painel-camera-config.ativo {
  display: block;
}

.camera-config-topo {
  position: absolute;
  top: 8px;
  left: 8px;
  right: 8px;
  height: 36px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  background: rgba(0,0,0,0.75);
  border: 1px solid rgba(255,255,255,0.18);
  border-radius: 10px;
  padding: 0 8px;
  color: white;
  font-size: 14px;
  font-weight: bold;
}

.select-tipo-config {
  width: calc(100% - 44px);
  height: 28px;
  border: 1px solid rgba(255,255,255,0.25);
  border-radius: 8px;
  background: rgba(20,20,20,0.95);
  color: white;
  font-size: 13px;
  font-weight: bold;
  padding: 0 8px;
  outline: none;
}

.btn-fechar-config {
  width: 34px;
  height: 28px;
  padding: 0;
  border-radius: 8px;
  background: rgba(214,40,40,0.95);
  font-size: 16px;
}


.conteudo-config-camera,
.conteudo-config-robo {
  width: 100%;
  height: 100%;
}

.conteudo-config-robo {
  display: none;
  overflow-y: auto;
  color: white;
  text-align: left;
}

.conteudo-config-robo.ativo {
  display: flex;
  align-items: flex-start;
  justify-content: center;
  padding-top: 24px;
  padding-bottom: 16px;
}

.conteudo-config-camera.oculto {
  display: none;
}

.camera-config-frame {
  width: 100%;
  height: 100%;
  border: none;
  border-radius: 12px;
  background: white;
}

.config-robo-card {
  width: min(420px, 92vw);
  background: rgba(20,20,20,0.94);
  border: 1px solid rgba(255,255,255,0.18);
  border-radius: 14px;
  padding: 16px;
  box-shadow: 0 4px 18px rgba(0,0,0,0.5);
}

.config-robo-card h2 {
  margin: 0 0 14px 0;
  font-size: 18px;
}

.grupo-config-robo {
  margin-top: 14px;
  padding-top: 12px;
  border-top: 1px solid rgba(255,255,255,0.12);
}

.grupo-config-robo:first-of-type {
  margin-top: 0;
  padding-top: 0;
  border-top: none;
}

.config-robo-card label {
  display: block;
  margin-bottom: 8px;
  font-size: 14px;
  color: #e0e0e0;
}

.linha-config-robo {
  display: flex;
  gap: 8px;
  align-items: center;
}

.input-config-robo {
  flex: 1;
  min-width: 0;
  height: 38px;
  border-radius: 10px;
  border: 1px solid rgba(255,255,255,0.25);
  background: rgba(0,0,0,0.55);
  color: white;
  font-size: 16px;
  padding: 0 10px;
  outline: none;
}

.acoes-config-robo {
  display: flex;
  justify-content: flex-end;
  margin-top: 16px;
}

.btn-salvar-config {
  width: 110px;
  height: 38px;
  padding: 0;
}

.valor-config-robo {
  margin: 12px 0 6px 0;
  font-size: 13px;
  color: #d0d0d0;
}

.texto-config-robo {
  margin: 0;
  font-size: 12px;
  line-height: 1.35;
  color: #bdbdbd;
}

.painel-controle {
  position: fixed;
  inset: 0;
  z-index: 20;
  width: 100vw;
  height: 100vh;
  pointer-events: none;
}

.joystick-area,
.extra {
  pointer-events: auto;
}

.joystick-area {
  position: fixed;
  left: 22px;
  top: 50%;
  transform: translateY(-50%);
  width: 150px;
  display: flex;
  flex-direction: column;
  align-items: center;
  background: transparent;
  padding: 0;
  border-radius: 0;
  backdrop-filter: none;
  border: none;
}

.joystick {
  width: 122px;
  height: 122px;
  background: radial-gradient(circle, rgba(70,70,70,0.75) 0%, rgba(20,20,20,0.78) 70%);
  border: 3px solid rgba(255,255,255,0.28);
  border-radius: 50%;
  position: relative;
  touch-action: none;
  user-select: none;
  box-shadow:
    inset 0 0 18px rgba(0,0,0,0.85),
    0 0 12px rgba(0,0,0,0.55);
}

.knob {
  width: 50px;
  height: 50px;
  background: #2d89ef;
  border-radius: 50%;
  position: absolute;
  left: 50%;
  top: 50%;
  transform: translate(-50%, -50%);
  box-shadow:
    0 0 16px rgba(45,137,239,0.95),
    inset 0 0 8px rgba(255,255,255,0.25);
  pointer-events: none;
}

.dica {
  display: none;
}

.extra {
  position: fixed;
  right: 22px;
  top: 50%;
  transform: translateY(-50%);
  width: 165px;
  max-width: 36vw;
  display: grid;
  grid-template-columns: 1fr;
  gap: 8px;
  background: rgba(0,0,0,0.32);
  padding: 10px;
  border-radius: 18px;
  backdrop-filter: blur(4px);
  border: 1px solid rgba(255,255,255,0.12);
}

.extra button {
  white-space: nowrap;
}

button {
  font-size: 13px;
  padding: 9px 7px;
  border: none;
  border-radius: 11px;
  background: rgba(45,137,239,0.92);
  color: white;
  font-weight: bold;
  touch-action: manipulation;
  box-shadow: 0 2px 6px rgba(0,0,0,0.38);
}

button:active {
  background: #1b5fa7;
  transform: scale(0.96);
}

.parar {
  background: rgba(214,40,40,0.95);
}

.parar:active {
  background: #9d1f1f;
}

.extra .parar {
  grid-column: span 1;
}


.velocidades-esteiras {
  position: fixed;
  right: 10px;
  bottom: 8px;
  z-index: 25;
  display: flex;
  gap: 12px;
  align-items: center;
  justify-content: center;
  color: #d7d7d7;
  background: rgba(0,0,0,0.55);
  padding: 7px 12px;
  border-radius: 12px;
  font-size: 12px;
  white-space: nowrap;
  pointer-events: none;
  border: 1px solid rgba(255,255,255,0.10);
  backdrop-filter: blur(3px);
}

.velocidades-esteiras span + span {
  border-left: 1px solid rgba(255,255,255,0.28);
  padding-left: 12px;
}

.velocidades-esteiras strong {
  color: #2d89ef;
  font-weight: bold;
  font-size: 15px;
}

.alerta-tof {
  position: fixed;
  z-index: 26;
  display: none;
  color: #ffffff;
  font-size: 15px;
  font-weight: bold;
  text-shadow: 0 1px 5px rgba(0,0,0,0.95);
  pointer-events: none;
  text-align: center;
}

.alerta-tof.ativo {
  display: block;
}

.linha-alerta {
  height: 18px;
  border-radius: 999px;
  background:
    linear-gradient(to right,
      rgba(255,0,0,0.00) 0%,
      rgba(255,0,0,0.16) 16%,
      rgba(255,40,40,0.68) 46%,
      rgba(255,60,60,0.78) 50%,
      rgba(255,40,40,0.68) 54%,
      rgba(255,0,0,0.16) 84%,
      rgba(255,0,0,0.00) 100%),
    radial-gradient(ellipse at center,
      rgba(255,0,0,0.48) 0%,
      rgba(255,0,0,0.24) 42%,
      rgba(255,0,0,0.00) 76%);
  filter: blur(1.8px);
  opacity: 0.92;
}

.valor-alerta {
  display: block;
  margin-bottom: 7px;
  background: transparent;
  padding: 0;
  border-radius: 0;
  color: #ffffff;
  font-size: 16px;
  font-weight: 700;
  text-shadow:
    0 1px 3px rgba(0,0,0,1),
    0 0 6px rgba(0,0,0,0.85);
}

.alerta-frontal-esq {
  top: 44px;
  left: 19vw;
  width: 17vw;
  min-width: 150px;
  transform: rotate(-3deg);
  transform-origin: center center;
}

.alerta-frontal-esq .valor-alerta {
  transform: rotate(3deg);
}

.alerta-frontal-dir {
  top: 44px;
  right: 19vw;
  width: 17vw;
  min-width: 150px;
  transform: rotate(3deg);
  transform-origin: center center;
}

.alerta-frontal-dir .valor-alerta {
  transform: rotate(-3deg);
}

.alerta-traseiro {
  left: 50%;
  bottom: 14px;
  width: 30vw;
  min-width: 220px;
  transform: translateX(-50%);
  text-align: center;
}

.alerta-traseiro .valor-alerta {
  margin-top: 0;
  margin-bottom: 6px;
}

.status {
  position: fixed;
  left: 8px;
  bottom: 6px;
  font-size: 11px;
  color: #e0e0e0;
  z-index: 25;
  text-align: left;
  background: rgba(0,0,0,0.55);
  padding: 4px 8px;
  border-radius: 8px;
  max-width: 60vw;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  pointer-events: none;
}

@media (orientation: landscape) {
  .topo {
    top: 7px;
    left: 58px;
    padding: 5px 9px;
  }

  h1 {
    font-size: 16px;
  }

  #subtituloPagina {
    font-size: 10px;
  }

  .btn-camera-config {
    top: 7px;
    left: 8px;
  }

  .joystick-area {
    left: 24px;
    top: 50%;
    transform: translateY(-50%);
    width: 142px;
    padding: 0;
  }

  .joystick {
    width: 116px;
    height: 116px;
  }

  .knob {
    width: 47px;
    height: 47px;
  }

  .extra {
    right: 24px;
    top: 50%;
    transform: translateY(-50%);
    width: 160px;
    max-width: 34vw;
    gap: 7px;
    padding: 9px;
    grid-template-columns: 1fr;
  }

  button {
    font-size: 12px;
    padding: 8px 6px;
  }

  .velocidades-esteiras {
    right: 10px;
    bottom: 8px;
    font-size: 11px;
  }

  .alerta-frontal-esq {
    top: 38px;
    left: 19vw;
    width: 17vw;
  }

  .alerta-frontal-dir {
    top: 38px;
    right: 19vw;
    width: 17vw;
  }

  .alerta-traseiro {
    bottom: 14px;
    width: 30vw;
  }

}

body.fullscreen-active .topo {
  display: none;
}

body.fullscreen-active .camera-bg img {
  object-fit: cover;
}

body.fullscreen-active .joystick-area {
  left: 20px;
  top: 50%;
  transform: translateY(-50%);
  width: 128px;
  padding: 0;
  background: transparent;
  border: none;
  backdrop-filter: none;
}

body.fullscreen-active .joystick {
  width: 104px;
  height: 104px;
}

body.fullscreen-active .knob {
  width: 43px;
  height: 43px;
}

body.fullscreen-active .dica {
  display: none;
}

body.fullscreen-active .extra {
  right: 20px;
  top: 50%;
  transform: translateY(-50%);
  width: 150px;
  max-width: 32vw;
  gap: 6px;
  padding: 8px;
  background: rgba(0,0,0,0.28);
  grid-template-columns: 1fr;
}

body.fullscreen-active .extra button {
  font-size: 11px;
  padding: 7px 5px;
  border-radius: 10px;
}

body.fullscreen-active .btn-fullscreen {
  width: 38px;
  height: 38px;
  font-size: 21px;
  top: 6px;
  right: 6px;
}

body.fullscreen-active .btn-camera-config {
  width: 38px;
  height: 38px;
  font-size: 20px;
  top: 6px;
  left: 6px;
}

body.fullscreen-active .status {
  font-size: 10px;
  bottom: 5px;
  left: 6px;
  max-width: 52vw;
  opacity: 0.85;
}


body.fullscreen-active .velocidades-esteiras {
  right: 8px;
  bottom: 6px;
  font-size: 10px;
  opacity: 0.90;
}

body.fullscreen-active .alerta-frontal-esq {
  top: 28px;
}

body.fullscreen-active .alerta-frontal-dir {
  top: 28px;
}

body.fullscreen-active .alerta-traseiro {
  bottom: 10px;
}

@media (orientation: portrait) {
  .camera-bg img {
    object-fit: cover;
  }

  .topo {
    top: 8px;
    left: 58px;
    right: 58px;
  }

  .joystick-area {
    left: 14px;
    top: 50%;
    transform: translateY(-50%);
    width: 150px;
  }

  .joystick {
    width: 122px;
    height: 122px;
  }

  .knob {
    width: 50px;
    height: 50px;
  }

  .extra {
    right: 14px;
    top: 50%;
    transform: translateY(-50%);
    width: 165px;
    max-width: 44vw;
    grid-template-columns: 1fr;
  }

  .extra .parar {
    grid-column: span 1;
  }

  .status {
    max-width: 85vw;
  }

  body.fullscreen-active .extra {
    right: 14px;
    top: 50%;
    transform: translateY(-50%);
    width: 160px;
    max-width: 43vw;
  }

  .velocidades-esteiras {
    right: 8px;
    bottom: 6px;
    font-size: 9px;
    gap: 6px;
    max-width: 60vw;
    overflow: hidden;
    text-overflow: ellipsis;
  }

  .alerta-frontal-esq {
    top: 48px;
    left: 14vw;
    width: 26vw;
    min-width: 110px;
  }

  .alerta-frontal-dir {
    top: 48px;
    right: 14vw;
    width: 26vw;
    min-width: 110px;
  }

  .alerta-traseiro {
    bottom: 12px;
    width: 42vw;
    min-width: 150px;
  }

}
)rawliteral";

#endif
