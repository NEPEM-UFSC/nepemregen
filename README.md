# Edge Appliance - ESP32 C++ Project

Estrutura base para desenvolvimento de dispositivos de borda utilizando ESP32, C++ e o framework ESP-IDF.

## Estrutura de Pastas

- `main/`: Ponto de entrada da aplicação.
- `components/`: Módulos reutilizáveis e lógica de negócio.
  - `app_core/`: Lógica central do appliance.
  - `hal/`: Camada de Abstração de Hardware.
  - `utils/`: Utilitários genéricos.
- `test/`: Projeto paralelo para execução de testes unitários.

## Como Buildar

### Aplicação Principal
```bash
idf.py build
```

### Testes
```bash
cd test
idf.py build
```

## Requisitos
- ESP-IDF v4.4 ou superior.
- Toolchain C++ configurada.

Documentação Técnica: Sistema Automático de Regulação Ambiental (REGEN)
1. Objetivo Científico

Desenvolver um sistema embarcado (ESP32) para regulação autônoma de umidade matricial e déficit de pressão de vapor em um microambiente controlado (mini-estufa). O sistema utiliza fusão de dados sensoriais para evitar falhas catastróficas e aplica leis de histerese para compensar o atraso (lag time) de infiltração da água no substrato.
2. Fusão de Sensores e Tratamento de Dados

O sistema não confia em leituras brutas isoladas. A arquitetura exige tolerância a falhas de hardware:

    Matriz Hídrica (Capacitivos TDR/FDR): Utiliza-se um mínimo de dois sensores no mesmo tratamento. A ESP32 lê ambos. Se a diferença entre eles for maior que uma variância aceitável (ex: >15%), o sistema sinaliza erro de hardware no log. Se a leitura for coerente, aplica-se a média aritmética. Leituras fora do escopo físico (<0% ou >100%) são sumariamente descartadas (Filtro de Outliers).

    Termodinâmica (SHT31-D): Comunicação I2C garantindo integridade digital da leitura de Temperatura e Umidade Relativa do Ar (UR).

3. Lógica de Controle de Atuadores
3.1. Controle Hídrico (Irrigação de Precisão)

A irrigação não é contínua; ela é quantizada e bloqueada por tempo.

    Condição de Disparo: Umidade do solo cai abaixo do Setpoint Crítico (ex: 50%).

    Ação: A bomba é acionada por um tempo tp​ (Tempo de Pulso), calculado para entregar um volume exato Vp​, dada a vazão conhecida Q do sistema:
    Vp​=Q⋅tp​

    Interlock (Cooldown): Imediatamente após o pulso, a rotina de irrigação é bloqueada por um período tlag​ (Atraso de Infiltração). Mesmo que o sensor continue registrando 49%, a bomba não ligará até que tlag​ expire, garantindo que a água percolou até a zona de leitura do sensor, evitando encharcamento.

3.2. Controle Termodinâmico (Ventilação Cumulativa)

A estufa exige renovação gasosa para manter a Evapotranspiração (EvP) e evitar condensação foliar. A lógica opera em duas frentes não-excludentes:

    Regra de Manutenção (Agendamento Cumulativo): O sistema exige 5 minutos de ventilação a cada janela de 30 minutos, operando apenas entre 06:00 e 18:00. O cronômetro acumula o tempo em que o ventilador esteve ligado por qualquer motivo. Se, ao final da janela de 30 minutos, o tempo acumulado for menor que 5 minutos, o sistema força a ligação pelo tempo faltante.

    Regra de Emergência Térmica/Fúngica:
    Se a temperatura ultrapassar o limite crítico (T>Tmax​) E a umidade do ar estiver alta (UR>URmax​), o ventilador liga imediatamente, independentemente do horário ou da regra de manutenção.
    Comportamento do Cronômetro: Todo segundo em que o ventilador opera na emergência é abatido da cota da Regra de Manutenção.

4. Arquitetura do Protocolo de Telemetria (LOGS)

O sistema de armazenamento de dados (via SD Card ou requisição HTTP para nuvem) deve seguir uma estrutura retangular tabular rigorosa. A geração do log ocorre sob duas condições:

    Base (Time-Driven): A cada 30 minutos exatos (via interrupção de timer ou NTP).

    Evento (Event-Driven): No exato momento de qualquer mudança de estado nos relés (Bomba LIGOU, Bomba DESLIGOU, Vent LIGOU, Vent DESLIGOU, Falha de Sensor).

Estrutura do Arquivo .csv
Timestamp (Epoch)	Trigger_Origem	Evento_Atuador	Temp_Ar (°C)	Umid_Ar (%)	Umid_Solo_Media (%)	Lag_Ativo (bool)
1714140000	Tempo_30m	NENHUM	24.5	65.2	55.0	0
1714141200	Sensor_Solo	BOMBA_LIGOU	25.1	64.0	49.5	0
1714141210	Timer_Pulso	BOMBA_DESLIGOU	25.1	64.1	49.5	1
1714142500	Emergencia_Ar	VENT_LIGOU	32.4	82.0	60.1	0
5. Implementação da Lógica de Tempo (Pseudo-Código C++)

Para implementar a regra do ventilador sem usar a função delay() (que travaria o processador e arruinaria o experimento), a técnica de rastreamento de milissegundos é obrigatória.
C++

unsigned long janela_inicio = millis();
unsigned long tempo_acumulado_vent = 0;
unsigned long tempo_ligado_inicio = 0;
const unsigned long JANELA_30M = 1800000;
const unsigned long META_5M = 300000;

void gerenciarVentilacao() {
  unsigned long agora = millis();
  bool condicao_emergencia = (temp_ar > 32.0 && umid_ar > 80.0);
  
  // Reseta a janela a cada 30 minutos
  if (agora - janela_inicio >= JANELA_30M) {
    janela_inicio = agora;
    tempo_acumulado_vent = 0; 
  }

  // Verifica déficit de ventilação no fim da janela
  bool deficit_ventilacao = ((agora - janela_inicio) > (JANELA_30M - META_5M)) && 
                            (tempo_acumulado_vent < META_5M);

  if (condicao_emergencia || (deficit_ventilacao && horarioComercial())) {
    if (!ventilador_ligado) {
      ligarVentilador(); // Esta função dispara o LOG do evento
      tempo_ligado_inicio = agora;
    }
  } else {
    if (ventilador_ligado && !condicao_emergencia) {
      desligarVentilador(); // Dispara o LOG do evento
      tempo_acumulado_vent += (agora - tempo_ligado_inicio);
    }
  }
}