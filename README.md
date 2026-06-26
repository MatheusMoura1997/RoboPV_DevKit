# RoboPV DevKit

Firmware principal do protótipo robótico **RoboPV**, desenvolvido para limpeza de módulos fotovoltaicos de pequeno porte. Este código é executado em um **ESP32 DevKit**, responsável pelo controle dos atuadores, leitura dos sensores, hospedagem da interface web e execução do controle em malha fechada de velocidade das esteiras.

O sistema foi desenvolvido como parte do Trabalho de Conclusão de Curso:

**Desenvolvimento de um protótipo robótico para limpeza de sistemas fotovoltaicos de pequeno porte**

## Visão geral

O RoboPV é um protótipo robótico operado remotamente, projetado para auxiliar a limpeza de módulos fotovoltaicos. O robô possui locomoção por esteiras, escovas rotativas, pulverização de água, sensores embarcados e interface web para operação remota.

Este repositório contém o firmware do **ESP32 DevKit**, que atua como controlador principal do robô.

A transmissão de imagem é realizada separadamente por uma **ESP32-CAM**, conectada à mesma rede Wi-Fi local criada pelo ESP32 DevKit.

## Funcionalidades

- Criação de rede Wi-Fi local para operação remota.
- Hospedagem de interface web embarcada.
- Controle de movimentação por joystick virtual.
- Acionamento independente das esteiras esquerda e direita.
- Acionamento das escovas rotativas.
- Acionamento da válvula solenoide de água.
- Leitura de sensores magnéticos AS5600 para medição de RPM das esteiras.
- Leitura de sensores ToF VL53L0X V2 para detecção de bordas ou ausência de superfície.
- Uso de multiplexador I²C TCA9548A.
- Controle em malha fechada de velocidade das esteiras com controlador PI.
- Rotina de segurança por perda de comando.
- Botão de emergência pela interface web.

## Arquitetura do sistema

O ESP32 DevKit concentra as seguintes funções:

- controle dos motores de tração por drivers BTS7960;
- acionamento das escovas e da válvula solenoide por módulos MOSFET D4184;
- leitura dos sensores AS5600 e VL53L0X V2 via barramento I²C;
- comunicação com o multiplexador TCA9548A;
- hospedagem da interface web;
- interpretação dos comandos enviados pelo navegador;
- cálculo da velocidade das esteiras;
- aplicação do controle PI de velocidade.

## Estrutura dos arquivos

```text
RoboPV_DevKit/
├── RoboPV_DevKit.ino
├── config.h
├── hardware.h
├── hardware.cpp
├── servidor.h
├── servidor.cpp
├── pagina_html.h
├── style_css.h
└── script_js.h
