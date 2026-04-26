# NEPEM REGEN

    Sistema de Controle Estocástico para Experimentação em Melhoramento de Linhaça (Linum usitatissimum)

📌 Visão Geral

O REGEN é um núcleo de controle em tempo real para mini-estufas de pesquisa. Diferente de controladores comerciais, o REGEN utiliza Edge Analytics para calcular variáveis fisiológicas invisíveis, como o VPD (Déficit de Pressão de Vapor), garantindo que a linhaça opere sempre em sua zona de máxima eficiência fotossintética sem risco de saturação fúngica.
🧬 Lógica de Controle Avançada
1. Dinâmica Hídrica (Anti-Overshooting)

O sistema implementa uma Máquina de Estados Não-Bloqueante para gerenciar a inércia do substrato.

    Threshold: Usolo​<50%.

    Pulso: Injeção de Q volume (mL) por t segundos.

    Lockdown Period: Bloqueio de re-irrigação baseado no Lag-Time de infiltração.

2. Algoritmo Termodinâmico (VPD Analytics)

Cálculo local no ESP32 utilizando a equação de Tetens para pressão de saturação:
es​=0.6108⋅exp(T+237.317.27⋅T​)
VPD=es​⋅(1−100UR​)
🛠️ Especificações de Hardware (BOM)
Componente	Especificação Técnica	Função
MCU	ESP32-WROOM-32 (Xtensa LX6)	Core de processamento
Ambiente	SHT31-D (I2C)	Temp/UR de alta precisão
Solo	Sonda Capacitiva V2.0 (Corrosion Resistant)	Umidade Volumétrica (VWC)
Energia	UPS 18650 Power Path	Backup de 5V ininterrupto
Atuadores	Relés Optoacoplados 5V	Isolação Galvânica de Motores
📂 Estrutura do Projeto (ESP-IDF Standard)
Plaintext

nepem-regen/
├── CMakeLists.txt
├── main/
│   ├── main.cpp            <-- Entry point (app_main)
│   ├── CMakeLists.txt
│   ├── idf_component.yml   <-- Dependências (SHT3x, MQTT)
│   ├── sensors.hpp         <-- Abstração de I2C e ADC
│   └── control_logic.cpp   <-- Algoritmos de Irrigação/VPD
├── components/             <-- Drivers customizados
├── partitions.csv          <-- Mapa de memória (NVS, LittleFS)
└── sdkconfig               <-- Configurações de RTOS e WiFi

🔌 Pinout & Conectividade (Mapeamento Crítico)
Pino ESP32	Função	Direção	Protocolo
GPIO 21	SDA (SHT31-D)	I/O	I2C
GPIO 22	SCL (SHT31-D)	O	I2C
GPIO 34	Sensor Solo 01	I	ADC (Analog)
GPIO 32	Detecção AC (UPS)	I	Digital
GPIO 26	Bomba Irrigação	O	Digital (Relé)
GPIO 27	Ventilador	O	Digital (Relé)
📊 Protocolo de Telemetria (CSV Layout)

Os dados são transmitidos via UART em 115200 bauds para captura por script Python host:

TIMESTAMP;EVENT_TYPE;T_AIR;RH_AIR;VPD;SOIL_MOIST;POWER_SRC

    Exemplo: 1714135200;LOG_PERIODIC;24.5;65.0;1.05;55.2;AC_POWER

🚀 Deployment Científico

    Configuração de Ambiente:
    Bash

    . $HOME/esp/esp-idf/export.sh
    idf.py set-target esp32

    Configuração do Eduroam (WPA2-Enterprise):
    Acesse idf.py menuconfig -> Component config -> Wi-Fi e insira suas credenciais do IdUFSC.

    Flash:
    Bash

    idf.py build flash monitor

⚠️ Integridade Estrutural & Segurança

    Fail-Safe: Em caso de falha crítica no sensor SHT31, o sistema força a ventilação a 50% de duty cycle por segurança biológica.

    Proteção de Atuadores: Uso obrigatório de diodos de flyback (ex: 1N4007) em paralelo com as bobinas dos relés para evitar EMI e danos ao SoC.

🤝 Créditos & Institucional

NEPEM - Núcleo de Estudos e Pesquisa em Experimentação e Melhoramento Vegetal
CCA - Centro de Ciências Agrárias | UFSC

Líder de Desenvolvimento: Matheus Lopes Machado