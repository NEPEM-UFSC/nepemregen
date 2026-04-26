# NEPEM REGEN | Precision Environmental Control
> **Sistema de Controle Estocástico para Experimentação em Melhoramento de Linhaça (_Linum usitatissimum_)**

![ESP-IDF Version](https://img.shields.io/badge/ESP--IDF-v5.2.1-blueviolet?style=for-the-badge&logo=espressif)
![Hardware](https://img.shields.io/badge/Hardware-ESP32--WROOM--32-E74C3C?style=for-the-badge&logo=espressif)
![Status](https://img.shields.io/badge/Status-Academic--Alpha-yellow?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-0051a8?style=for-the-badge)

---

## 📌 Visão Geral

O **REGEN** (Regulação de Geração Ambiental) é um núcleo de controle em tempo real projetado especificamente para o **NEPEM/UFSC**. Diferente de controladores comerciais genéricos, o REGEN utiliza **Edge Analytics** para calcular variáveis fisiológicas críticas, como o **VPD (Déficit de Pressão de Vapor)**, garantindo que o cultivo de linhaça opere em sua zona de máxima eficiência fotossintética.

---

## 🧬 Lógica de Controle Avançada

### 1. Dinâmica Hídrica (Anti-Overshooting)
O sistema implementa uma **Máquina de Estados Não-Bloqueante** para gerenciar a inércia hídrica do substrato.
* **Threshold de Ativação:** $U_{solo} < 50\%$.
* **Estratégia:** Injeção de volume quantizado por pulso temporal.
* **Lockdown Period:** Bloqueio de re-irrigação baseado no *Lag-Time* de infiltração calibrado.

### 2. Algoritmo Termodinâmico (VPD Analytics)
Cálculo local em tempo real utilizando a equação de saturação de vapor de Tetens:

$$e_s = 0.6108 \cdot \exp\left(\frac{17.27 \cdot T}{T + 237.3}\right)$$
$$VPD = e_s \cdot (1 - \frac{UR}{100})$$

---

## 🛠️ Especificações de Hardware (BOM)

| Componente | Especificação Técnica | Função |
| :--- | :--- | :--- |
| **MCU** | ESP32-WROOM-32 (Xtensa LX6) | Core de processamento e RTOS |
| **Sensor de Ambiente** | Adafruit SHT31-D (I2C) | Termodinâmica de alta precisão |
| **Sensores de Solo** | 2x Sondas Capacitivas V2.0 | Umidade Volumétrica (VWC) |
| **Gestão de Energia** | UPS 18650 Power Path | Backup de 5V ininterrupto (No-Break) |
| **Atuadores** | Relés Optoacoplados 5V | Isolação Galvânica de Cargas |
| **Interface USB** | Chip CH9102X | Comunicação Serial |

---

## 📂 Estrutura do Projeto (ESP-IDF Standard)

```text
nepem-regen/
├── CMakeLists.txt          # Configuração global do projeto
├── partitions.csv          # Tabela de partições (NVS, App, LittleFS)
├── main/
│   ├── main.cpp            # Entry point (app_main) e loop principal
│   ├── CMakeLists.txt      # Registro de fontes e dependências
│   ├── sensors.hpp         # Abstração de I2C, ADC e filtros EMA
│   └── control_logic.cpp   # Algoritmos de Irrigação e cálculos de VPD
├── components/             # Drivers específicos (SHT31, MQTT)
└── sdkconfig               # Definições do FreeRTOS e WPA2-Enterprise
```

---

## 🔌 Pinout & Conectividade

| Pino ESP32 | Função | Direção | Protocolo |
| :--- | :--- | :--- | :--- |
| **GPIO 21** | SDA (SHT31-D) | I/O | I2C |
| **GPIO 22** | SCL (SHT31-D) | O | I2C |
| **GPIO 34** | Sensor Solo 01 | I | ADC (Analog) |
| **GPIO 32** | Detecção AC (UPS) | I | Digital |
| **GPIO 26** | Relé Bomba | O | Digital |
| **GPIO 27** | Relé Ventilador | O | Digital |

---

## 📊 Protocolo de Telemetria (CSV Layout)

Os dados são transmitidos via UART (115200 bauds) para captura e análise em Python/R:

`TIMESTAMP;EVENT_TYPE;T_AIR;RH_AIR;VPD;SOIL_MOIST;POWER_SRC`

> **Exemplo:** `1714135200;LOG_PERIODIC;24.5;65.0;1.05;55.2;AC_POWER`

---

## 🚀 Deployment Científico

### Configuração de Ambiente
```bash
. $HOME/esp/esp-idf/export.sh
idf.py set-target esp32
```

### Autenticação Eduroam
Acesse `idf.py menuconfig` -> `Component config` -> `Wi-Fi` e configure as identidades EAP-PEAP (Credenciais IdUFSC).

### Compilação e Flash
```bash
idf.py build flash monitor
```

---

## ⚠️ Integridade Estrutural & Segurança

* **Failsafe Operacional:** Caso o sensor SHT31 falhe, o sistema força a ventilação para mitigar picos térmicos.
* **Proteção Elétrica:** Uso obrigatório de **diodos de flyback** (ex: 1N4007) em paralelo com as bobinas dos relés para suprimir EMI.
* **Power Awareness:** O firmware monitora a transição para modo bateria para ajustar a taxa de amostragem.

---

## 🤝 Créditos & Institucional

**NEPEM - Núcleo de Estudos e Pesquisa em Experimentação e Melhoramento Vegetal** **CCA - Centro de Ciências Agrárias | UFSC**

**Líder de Desenvolvimento:** Matheus Lopes Machado  
**Orientação:** Laboratório de Melhoramento Vegetal.

---
*Este software é fornecido 'as-is' para fins estritamente acadêmicos e de pesquisa científica.*
