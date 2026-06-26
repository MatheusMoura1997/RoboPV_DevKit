#ifndef SCRIPT_JS_H
#define SCRIPT_JS_H

#include <Arduino.h>

const char scriptJS[] PROGMEM = R"rawliteral(
var joystick = null;
var knob = null;
var statusCampo = null;

var joystickAtivo = false;

var joyXAtual = 0;
var joyYAtual = 0;

var ultimoXEnviado = 999;
var ultimoYEnviado = 999;

var intervaloEnvio = null;

var zonaMortaPx = 12;
var sistemaIniciado = false;
var intervaloSensores = null;

var escovaFrenteLigadaUI = false;
var escovaTraseiraLigadaUI = false;
var aguaLigadaUI = false;

// Sessão e sequência do joystick.
// Isso evita que comandos atrasados sejam aceitos depois de soltar o joystick.
var sessaoJoy = Math.floor(Date.now() % 2000000000);
var sequenciaJoy = 0;

function atualizarStatus(texto) {
  var campo = document.getElementById('status');
  if (campo) campo.innerText = texto;
}

function abrirConfigCamera() {
  var painel = document.getElementById('painelCameraConfig');
  var select = document.getElementById('selectTipoConfig');

  if (select) {
    select.value = 'robo';
  }

  if (painel) {
    painel.classList.add('ativo');
  }

  trocarTipoConfig();
  atualizarStatus('Menu de configurações aberto');
}

function fecharConfigCamera() {
  var painel = document.getElementById('painelCameraConfig');
  var iframe = document.getElementById('iframeCameraConfig');

  if (painel) {
    painel.classList.remove('ativo');
  }

  if (iframe) {
    iframe.setAttribute('src', '');
  }

  atualizarStatus('Menu de configurações fechado');
}

function carregarConfigCamera() {
  var iframe = document.getElementById('iframeCameraConfig');

  if (iframe && iframe.getAttribute('src') === '') {
    iframe.setAttribute('src', 'http://192.168.4.2');
  }
}

function trocarTipoConfig() {
  var select = document.getElementById('selectTipoConfig');
  var conteudoCamera = document.getElementById('conteudoConfigCamera');
  var conteudoRobo = document.getElementById('conteudoConfigRobo');
  var iframe = document.getElementById('iframeCameraConfig');

  var tipo = select ? select.value : 'robo';

  if (tipo === 'robo') {
    if (conteudoCamera) conteudoCamera.classList.add('oculto');
    if (conteudoRobo) conteudoRobo.classList.add('ativo');
    if (iframe) iframe.setAttribute('src', '');

    carregarConfigRobo();
    atualizarStatus('Configurações do robô');
  } else {
    if (conteudoCamera) conteudoCamera.classList.remove('oculto');
    if (conteudoRobo) conteudoRobo.classList.remove('ativo');

    carregarConfigCamera();
    atualizarStatus('Configurações da câmera');
  }
}

function atualizarCamposConfigRobo(dados) {
  var inputPWMMax = document.getElementById('inputPWMMax');
  var valorPWMMax = document.getElementById('valorPWMMax');

  var inputToFLimiar = document.getElementById('inputToFLimiar');
  var valorToFLimiar = document.getElementById('valorToFLimiar');

  var inputControlePI = document.getElementById('inputControlePI');
  var valorControlePI = document.getElementById('valorControlePI');

  var inputRPMMax = document.getElementById('inputRPMMax');
  var valorRPMMax = document.getElementById('valorRPMMax');

  var inputKpRPM = document.getElementById('inputKpRPM');
  var valorKpRPM = document.getElementById('valorKpRPM');

  var inputKiRPM = document.getElementById('inputKiRPM');
  var valorKiRPM = document.getElementById('valorKiRPM');

  if (dados && typeof dados.pwm_max !== 'undefined') {
    if (inputPWMMax) inputPWMMax.value = dados.pwm_max;
    if (valorPWMMax) valorPWMMax.innerText = dados.pwm_max;
  }

  if (dados && typeof dados.tof_limiar_mm !== 'undefined') {
    if (inputToFLimiar) inputToFLimiar.value = dados.tof_limiar_mm;
    if (valorToFLimiar) valorToFLimiar.innerText = dados.tof_limiar_mm;
  }

  if (dados && typeof dados.controle_pi !== 'undefined') {
    var ativo = dados.controle_pi === true || dados.controle_pi === 1 || dados.controle_pi === 'true';

    if (inputControlePI) inputControlePI.checked = ativo;
    if (valorControlePI) valorControlePI.innerText = ativo ? 'Controle PI' : 'PWM direto';
  }

  if (dados && typeof dados.rpm_max !== 'undefined') {
    if (inputRPMMax) inputRPMMax.value = Number(dados.rpm_max).toFixed(1);
    if (valorRPMMax) valorRPMMax.innerText = Number(dados.rpm_max).toFixed(1);
  }

  if (dados && typeof dados.kp_rpm !== 'undefined') {
    if (inputKpRPM) inputKpRPM.value = Number(dados.kp_rpm).toFixed(2);
    if (valorKpRPM) valorKpRPM.innerText = Number(dados.kp_rpm).toFixed(2);
  }

  if (dados && typeof dados.ki_rpm !== 'undefined') {
    if (inputKiRPM) inputKiRPM.value = Number(dados.ki_rpm).toFixed(2);
    if (valorKiRPM) valorKiRPM.innerText = Number(dados.ki_rpm).toFixed(2);
  }
}

function carregarConfigRobo() {
  var xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4) {
      if (xhr.status == 200) {
        try {
          var dados = JSON.parse(xhr.responseText);
          atualizarCamposConfigRobo(dados);
        } catch (e) {
          atualizarStatus('erro ao ler config do robô');
        }
      } else {
        atualizarStatus('erro HTTP config: ' + xhr.status);
      }
    }
  };

  xhr.open('GET', '/robo_config', true);
  xhr.send();
}

function salvarConfigRobo() {
  var inputPWMMax = document.getElementById('inputPWMMax');
  var inputToFLimiar = document.getElementById('inputToFLimiar');
  var inputControlePI = document.getElementById('inputControlePI');
  var inputRPMMax = document.getElementById('inputRPMMax');
  var inputKpRPM = document.getElementById('inputKpRPM');
  var inputKiRPM = document.getElementById('inputKiRPM');

  if (!inputPWMMax || !inputToFLimiar || !inputControlePI || !inputRPMMax || !inputKpRPM || !inputKiRPM) {
    atualizarStatus('campo de configuração não encontrado');
    return;
  }

  var pwm = parseInt(inputPWMMax.value, 10);
  var tofLimiar = parseInt(inputToFLimiar.value, 10);
  var controlePI = inputControlePI.checked ? 1 : 0;
  var rpmMax = parseFloat(inputRPMMax.value);
  var kpRPM = parseFloat(inputKpRPM.value);
  var kiRPM = parseFloat(inputKiRPM.value);

  if (isNaN(pwm)) {
    atualizarStatus('PWM inválido');
    return;
  }

  if (isNaN(tofLimiar)) {
    atualizarStatus('limiar ToF inválido');
    return;
  }

  if (isNaN(rpmMax)) {
    atualizarStatus('RPM máximo inválido');
    return;
  }

  if (isNaN(kpRPM)) {
    atualizarStatus('Kp inválido');
    return;
  }

  if (isNaN(kiRPM)) {
    atualizarStatus('Ki inválido');
    return;
  }

  pwm = limitar(pwm, 0, 255);
  tofLimiar = limitar(tofLimiar, 1, 2000);
  rpmMax = limitar(rpmMax, 1, 200);
  kpRPM = limitar(kpRPM, 0, 20);
  kiRPM = limitar(kiRPM, 0, 10);

  var url = '/robo_config?';
  url += 'pwm_max=' + encodeURIComponent(pwm);
  url += '&tof_limiar_mm=' + encodeURIComponent(tofLimiar);
  url += '&controle_pi=' + encodeURIComponent(controlePI);
  url += '&rpm_max=' + encodeURIComponent(rpmMax);
  url += '&kp_rpm=' + encodeURIComponent(kpRPM);
  url += '&ki_rpm=' + encodeURIComponent(kiRPM);

  var xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4) {
      if (xhr.status == 200) {
        try {
          var dados = JSON.parse(xhr.responseText);
          atualizarCamposConfigRobo(dados);

          var modo = dados.controle_pi ? 'PI' : 'PWM';
          atualizarStatus('Config salva | Modo: ' + modo + ' | PWM ' + dados.pwm_max + ' | RPMmax ' + Number(dados.rpm_max).toFixed(1));
        } catch (e) {
          atualizarStatus('config salva, erro ao ler resposta');
        }
      } else {
        atualizarStatus('erro HTTP salvar config: ' + xhr.status);
      }
    }
  };

  xhr.open('GET', url, true);
  xhr.send();
}

function atualizarAlertaToF(idElemento, ativo, rotulo, valorMM) {
  var elemento = document.getElementById(idElemento);

  if (!elemento) return;

  var valor = elemento.querySelector('.valor-alerta');

  if (valor) {
    valor.innerText = valorMM + ' mm';
  }

  if (ativo) {
    elemento.classList.add('ativo');
  } else {
    elemento.classList.remove('ativo');
  }
}

function atualizarPainelSensores(dados) {
  var velEsq = document.getElementById('velEsteiraEsq');
  var velDir = document.getElementById('velEsteiraDir');

  var setEsq = document.getElementById('setEsteiraEsq');
  var setDir = document.getElementById('setEsteiraDir');

  var pwmEsq = document.getElementById('pwmEsteiraEsq');
  var pwmDir = document.getElementById('pwmEsteiraDir');

  var modoControleAtual = document.getElementById('modoControleAtual');

  if (velEsq && typeof dados.vel_esq_rpm !== 'undefined') {
    velEsq.innerText = Number(dados.vel_esq_rpm).toFixed(1) + ' rpm';
  }

  if (velDir && typeof dados.vel_dir_rpm !== 'undefined') {
    velDir.innerText = Number(dados.vel_dir_rpm).toFixed(1) + ' rpm';
  }

  if (setEsq && typeof dados.set_esq_rpm !== 'undefined') {
    setEsq.innerText = Number(dados.set_esq_rpm).toFixed(1) + ' rpm';
  }

  if (setDir && typeof dados.set_dir_rpm !== 'undefined') {
    setDir.innerText = Number(dados.set_dir_rpm).toFixed(1) + ' rpm';
  }

  if (pwmEsq && typeof dados.pwm_esq !== 'undefined') {
    pwmEsq.innerText = dados.pwm_esq;
  }

  if (pwmDir && typeof dados.pwm_dir !== 'undefined') {
    pwmDir.innerText = dados.pwm_dir;
  }

  if (modoControleAtual && typeof dados.controle_pi !== 'undefined') {
    modoControleAtual.innerText = dados.controle_pi ? 'PI' : 'PWM';
  }

  atualizarAlertaToF('alertaTofFrontalEsq', dados.tof_fe_perigo, 'FE', dados.tof_fe_mm);
  atualizarAlertaToF('alertaTofFrontalDir', dados.tof_fd_perigo, 'FD', dados.tof_fd_mm);
  atualizarAlertaToF('alertaTofTraseiro', dados.tof_tras_perigo, 'TR', dados.tof_tras_mm);
}

function carregarSensores() {
  var xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4) {
      if (xhr.status == 200) {
        try {
          var dados = JSON.parse(xhr.responseText);
          atualizarPainelSensores(dados);
        } catch (e) {
          atualizarStatus('erro ao ler sensores');
        }
      }
    }
  };

  xhr.open('GET', '/sensores', true);
  xhr.send();
}

function iniciarLeituraSensores() {
  if (intervaloSensores) return;

  carregarSensores();

  intervaloSensores = setInterval(function() {
    carregarSensores();
  }, 300);
}

function atualizarBotoesEstados() {
  var btnEscovaFrente = document.getElementById('btnEscovaFrente');
  var btnEscovaTraseira = document.getElementById('btnEscovaTraseira');
  var btnAgua = document.getElementById('btnAgua');

  if (btnEscovaFrente) {
    btnEscovaFrente.innerText = escovaFrenteLigadaUI ? 'Escova Dianteira ON' : 'Escova Dianteira OFF';
  }

  if (btnEscovaTraseira) {
    btnEscovaTraseira.innerText = escovaTraseiraLigadaUI ? 'Escova Traseira ON' : 'Escova Traseira OFF';
  }

  if (btnAgua) {
    btnAgua.innerText = aguaLigadaUI ? 'Agua ON' : 'Agua OFF';
  }
}

function procurarElementos() {
  joystick = document.getElementById('joystick');
  knob = document.getElementById('knob');
  statusCampo = document.getElementById('status');
  return joystick && knob && statusCampo;
}

function iniciarSistema() {
  if (sistemaIniciado) return;

  if (!procurarElementos()) {
    setTimeout(iniciarSistema, 100);
    return;
  }

  sistemaIniciado = true;
  atualizarBotoesEstados();
  atualizarStatus('JS carregado - controle PWM/PI');
  iniciarLeituraSensores();

  document.addEventListener('mousemove', moverJoystick);
  document.addEventListener('mouseup', soltarJoystick);

  document.addEventListener('touchmove', moverJoystick, { passive: false });
  document.addEventListener('touchend', soltarJoystick);
  document.addEventListener('touchcancel', soltarJoystick);
}

setTimeout(iniciarSistema, 100);
document.addEventListener('DOMContentLoaded', iniciarSistema);

function entrarTelaCheia() {
  var elem = document.documentElement;

  if (document.body) {
    document.body.classList.add('fullscreen-active');
  }

  if (!document.fullscreenElement && !document.webkitFullscreenElement) {
    if (elem.requestFullscreen) {
      elem.requestFullscreen();
    } else if (elem.webkitRequestFullscreen) {
      elem.webkitRequestFullscreen();
    }

    if (screen.orientation && screen.orientation.lock) {
      screen.orientation.lock('landscape').catch(function() {
        atualizarStatus('Tela cheia ativa. Gire o celular para paisagem.');
      });
    }
  } else {
    if (document.exitFullscreen) {
      document.exitFullscreen();
    } else if (document.webkitExitFullscreen) {
      document.webkitExitFullscreen();
    }

    if (document.body) {
      document.body.classList.remove('fullscreen-active');
    }
  }
}

function limitar(valor, minimo, maximo) {
  if (valor < minimo) return minimo;
  if (valor > maximo) return maximo;
  return valor;
}

function enviarJoy(x, y) {
  x = limitar(Math.round(x), -100, 100);
  y = limitar(Math.round(y), -100, 100);

  sequenciaJoy = sequenciaJoy + 1;

  var xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4) {
      if (xhr.status == 200) {
        atualizarStatus(xhr.responseText);
      } else {
        atualizarStatus('erro HTTP: ' + xhr.status);
      }
    }
  };

  var url = '/joy?x=' + encodeURIComponent(x);
  url += '&y=' + encodeURIComponent(y);
  url += '&seq=' + encodeURIComponent(sequenciaJoy);
  url += '&sess=' + encodeURIComponent(sessaoJoy);

  xhr.open('GET', url, true);
  xhr.send();
}

function enviar(comando, callbackOk) {
  atualizarStatus('enviando: ' + comando);

  var xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function() {
    if (xhr.readyState == 4) {
      if (xhr.status == 200) {
        atualizarStatus(comando + ' - ' + xhr.responseText);

        if (callbackOk) {
          callbackOk();
        }
      } else {
        atualizarStatus('erro HTTP: ' + xhr.status);
      }
    }
  };

  xhr.open('GET', '/cmd?acao=' + encodeURIComponent(comando), true);
  xhr.send();
}

function alternarEscovaFrente() {
  var novoEstado = !escovaFrenteLigadaUI;
  var comando = novoEstado ? 'escova_frente_on' : 'escova_frente_off';

  enviar(comando, function() {
    escovaFrenteLigadaUI = novoEstado;
    atualizarBotoesEstados();
  });
}

function alternarEscovaTraseira() {
  var novoEstado = !escovaTraseiraLigadaUI;
  var comando = novoEstado ? 'escova_traseira_on' : 'escova_traseira_off';

  enviar(comando, function() {
    escovaTraseiraLigadaUI = novoEstado;
    atualizarBotoesEstados();
  });
}

function alternarAgua() {
  var novoEstado = !aguaLigadaUI;
  var comando = novoEstado ? 'agua_on' : 'agua_off';

  enviar(comando, function() {
    aguaLigadaUI = novoEstado;
    atualizarBotoesEstados();
  });
}

function enviarEmergencia() {
  enviar('emergencia', function() {
    escovaFrenteLigadaUI = false;
    escovaTraseiraLigadaUI = false;
    aguaLigadaUI = false;
    atualizarBotoesEstados();
  });

  enviarJoy(0, 0);
}

function pegarCoordenadas(evento) {
  if (evento.touches && evento.touches.length > 0) {
    return {
      x: evento.touches[0].clientX,
      y: evento.touches[0].clientY
    };
  }

  return {
    x: evento.clientX,
    y: evento.clientY
  };
}

function iniciarJoystick(evento) {
  if (!procurarElementos()) {
    atualizarStatus('erro: joystick nao encontrado');
    return;
  }

  evento.preventDefault();

  joystickAtivo = true;

  if (document.body) {
    document.body.style.overflow = 'hidden';
  }

  moverJoystick(evento);

  if (!intervaloEnvio) {
    intervaloEnvio = setInterval(function() {
      if (joystickAtivo) {
        enviarJoy(joyXAtual, joyYAtual);
      }
    }, 150);
  }
}

function moverJoystick(evento) {
  if (!joystickAtivo) return;
  if (!joystick || !knob) return;

  evento.preventDefault();

  var ponto = pegarCoordenadas(evento);
  var rect = joystick.getBoundingClientRect();

  var centroX = rect.left + rect.width / 2;
  var centroY = rect.top + rect.height / 2;

  var dx = ponto.x - centroX;
  var dy = ponto.y - centroY;

  var distancia = Math.sqrt(dx * dx + dy * dy);
  var limite = Math.min(rect.width, rect.height) * 0.35;

  if (distancia > limite) {
    dx = dx / distancia * limite;
    dy = dy / distancia * limite;
    distancia = limite;
  }

  knob.style.left = (rect.width / 2 + dx) + 'px';
  knob.style.top = (rect.height / 2 + dy) + 'px';

  var x = 0;
  var y = 0;

  if (distancia > zonaMortaPx) {
    x = Math.round((dx / limite) * 100);
    y = Math.round((-dy / limite) * 100);
  }

  x = limitar(x, -100, 100);
  y = limitar(y, -100, 100);

  joyXAtual = x;
  joyYAtual = y;

  if (Math.abs(x - ultimoXEnviado) >= 3 || Math.abs(y - ultimoYEnviado) >= 3) {
    ultimoXEnviado = x;
    ultimoYEnviado = y;
    enviarJoy(x, y);
  }
}

function soltarJoystick(evento) {
  if (!joystickAtivo) return;

  if (evento) evento.preventDefault();

  joystickAtivo = false;

  if (document.body) {
    document.body.style.overflow = '';
  }

  if (knob) {
    knob.style.left = '50%';
    knob.style.top = '50%';
  }

  joyXAtual = 0;
  joyYAtual = 0;

  ultimoXEnviado = 0;
  ultimoYEnviado = 0;

  if (intervaloEnvio) {
    clearInterval(intervaloEnvio);
    intervaloEnvio = null;
  }

  enviarJoy(0, 0);
}

document.addEventListener('fullscreenchange', function() {
  if (document.fullscreenElement) {
    document.body.classList.add('fullscreen-active');
  } else {
    document.body.classList.remove('fullscreen-active');
  }
});

document.addEventListener('webkitfullscreenchange', function() {
  if (document.webkitFullscreenElement) {
    document.body.classList.add('fullscreen-active');
  } else {
    document.body.classList.remove('fullscreen-active');
  }
});
)rawliteral";

#endif