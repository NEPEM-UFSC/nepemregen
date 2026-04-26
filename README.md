# NEPEM REGEN

## Sistema de Controle Ambiental de Precisão para Mini-estufas.
## Desenvolvido pelo o Núcleo de Estudos e Pesquisa em Experimentação e Melhoramento Vegetal (NEPEM/UFSC)

O REGEN é um sistema embarcado de malha fechada projetado para a automação e monitoramento de estufas de pesquisa (_Nursery_), especificamente calibrado para o cultivo de Linhaça (_Linum usitatissimum_). O foco do projeto é a manutenção do Déficit de Pressão de Vapor (VPD) e a estabilidade da tensão matricial do solo, garantindo a repetibilidade experimental necessária para programas de melhoramento.

## O Problema Científico

Experimentos com linhaça em ambiente controlado sofrem com a alta sensibilidade à umidade relativa e ao estresse hídrico. A ventilação inadequada gera condensação foliar (risco de Botrytis), enquanto a irrigação baseada apenas em timers ignora a inércia hídrica do substrato, levando ao overshooting hídrico. O REGEN resolve isso através de Edge Analytics e Interlocks de Segurança.

## Arquitetura do Sistema
Hardware (Bill of Materials)

    MCU: ESP32-WROOM-32 (Target esp32).

    Sensor Termodinâmico: SHT31-D (I2C) com filtro sinterizado.

    Sensores Hídricos: 2x Sensores Capacitivos de Umidade do Solo (Resistentes à corrosão).

    Gestão de Energia: Módulo UPS 18650 com Gerenciamento de Caminho de Energia.

    Atuação: Shield de Relés de 2 Canais com isolamento por optoacopladores.

    I/O Peripheral: Shield de expansão de IO para facilitar conexões e reduzir ruído.

Software Stack

    Firmware: C++ Nativo sobre ESP-IDF (FreeRTOS).

    Analytics: Cálculo local de VPD e Ponto de Orvalho.

    Logging: CSV estruturado via UART/Serial (integrável com Python Host).

    Resiliência: Detecção de Brownout e persistência em memória NVS.

🧬 Lógica de Controle (The "Regen" Core)
1. Irrigação com Calibração de Lag-Time

O sistema utiliza um algoritmo de Pulso & Espera. Ao detectar solo abaixo de 50%, dispara um volume fixo (calibrado em mL) e entra em estado de Lockdown por X minutos, permitindo que o sensor detecte a água percolada antes de nova atuação.
2. Ventilação Estratégica (VPD-Driven)

A ventilação não é apenas horária; ela é inteligente:

    Quota de Renovação: Garante 5 min de fluxo a cada 30 min (janela das 06h às 18h).

    Gatilho de VPD: Aciona emergencialmente se o VPD cair abaixo de 0.4 kPa (evitando fungos) ou se o Ponto de Orvalho se aproximar da temperatura foliar.

3. Telemetria de Falhas

    Monitoramento ativo da rede elétrica da UFSC via divisor de tensão.

    Registro de BROWNOUT_RESET para invalidar dados climáticos durante instabilidades elétricas.

🚀 Como Executar (Ambiente de Pesquisa)
Requisitos

    Espressif IoT Development Framework (ESP-IDF) v5.x+.

    Ferramenta CMake e Ninja.

Build & Flash
Bash

# Configurar o target
idf.py set-target esp32

# Abrir menu de configuração (Flash 4MB, PSRAM se disponível)
idf.py menuconfig

# Compilar e Gravar
idf.py build flash monitor

📊 Estrutura de Logs

O sistema cospe dados no formato .csv via Serial para fácil importação em R ou Python:
[TIMESTAMP], [EVENTO], [TEMP_AR], [UMID_AR], [VPD], [UMID_SOLO], [POWER_STATUS]

⚠️ Notas de Integridade Estrutural (Aviso do Mentor)

    Rigor Técnico: Este código foi desenhado para ambiente de laboratório. Toda atuação de carga indutiva (motores) deve utilizar diodos de flyback.

🤝 Contribuição e Créditos

Este é um projeto do NEPEM/UFSC. Se você encontrar falhas na lógica de histerese ou nos cálculos termodinâmicos, abra uma Issue com o embasamento agronômico correspondente.

Desenvolvedor: Matheus Lopes Machado