# Projeto Arduino: Monitoramento de Velocidade, Umidade e Temperatura para Corridas de Fórmula E

Este projeto utiliza um Arduino para monitorar a velocidade dos carros em corridas de Fórmula E, além de medir as condições ambientais como umidade e temperatura da pista. O sistema registra a data e hora dos eventos usando um módulo RTC (Real-Time Clock).

## Objetivo

O objetivo deste projeto é fornecer uma solução precisa para medir a velocidade dos carros de Fórmula E em um ponto específico da pista e monitorar as condições ambientais, que podem influenciar no desempenho dos carros.

## Componentes Necessários

- Arduino Uno
- Sensor de Umidade e Temperatura DHT11
- Dois Sensores de Ultrassom HC-SR04
- Módulo RTC DS3231
- Módulo Bluetooth (HC-05 ou HC-06)
- Jumpers
- Protoboard

## Conexões

### Conexões do DHT11

| DHT11 | Arduino Uno |
|-------|-------------|
| VCC   | 5V          |
| GND   | GND         |
| DATA  | A0          |

### Conexões dos Sensores de Ultrassom

#### Sensor de Ultrassom 1

| HC-SR04      | Arduino Uno |
|--------------|-------------|
| VCC          | 5V          |
| GND          | GND         |
| TRIGGER_PIN  | 4           |
| ECHO_PIN     | 5           |

#### Sensor de Ultrassom 2

| HC-SR04      | Arduino Uno |
|--------------|-------------|
| VCC          | 5V          |
| GND          | GND         |
| TRIGGER_PIN  | 6           |
| ECHO_PIN     | 7           |

### Conexões do RTC DS3231

| RTC DS3231 | Arduino Uno |
|------------|-------------|
| VCC        | 5V          |
| GND        | GND         |
| SDA        | A4          |
| SCL        | A5          |

### Conexões do Módulo Bluetooth

| Bluetooth | Arduino Uno |
|-----------|-------------|
| VCC       | 5V          |
| GND       | GND         |
| TX        | 2           |
| RX        | 3           |

## Código

```cpp
#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>

SoftwareSerial bluetooth(2, 3);
RTC_DS3231 rtc;

#define HT A0
#define TipoDHT DHT11
DHT dht(HT, TipoDHT);

#define TRIGGER_PIN_1 4
#define ECHO_PIN_1 5
#define TRIGGER_PIN_2 6
#define ECHO_PIN_2 7
#define DISTANCE_BETWEEN_SENSORS 10 // Distância entre os dois sensores em centímetros

void setup() {
  Serial.begin(9600);
  Serial.println("Teste de Ultrassom, DHT e RTC");
  bluetooth.begin(9600);
  
  if (!rtc.begin()) {
    Serial.println("RTC não encontrado!");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC perdeu o poder, ajustando o tempo!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajuste o RTC para a data e hora de compilação do código
  }

  dht.begin();
  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
}

long readUltrasonicDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH);
}

void loop() {
  // Leitura do sensor de umidade e temperatura
  float u = dht.readHumidity();
  float t = dht.readTemperature();

  // Medição de tempo para os sensores de ultrassom
  long duration_1 = readUltrasonicDistance(TRIGGER_PIN_1, ECHO_PIN_1);
  long duration_2 = readUltrasonicDistance(TRIGGER_PIN_2, ECHO_PIN_2);

  float speed_kmh = 0;
  bool ultrassomConectado = (duration_1 != 0 && duration_2 != 0);

  if (ultrassomConectado) {
    // Calcular a velocidade
    float distance = DISTANCE_BETWEEN_SENSORS; // Distância entre os sensores em centímetros
    float time_diff_seconds = (duration_2 - duration_1) / 1000000.0; // Diferença de tempo em segundos
    float speed_cm_per_sec = distance / time_diff_seconds; // Velocidade em cm/s
    speed_kmh = speed_cm_per_sec * 0.036; // Converter velocidade para km/h
  }

  // Obter a data e hora atuais
  DateTime now = rtc.now();

  // Criando a string com todas as leituras em linhas separadas
  String mensagem = "Umidade: " + String(u) + " %\n" +
                    "Temperatura: " + String(t) + " C\n" +
                    (ultrassomConectado ? 
                     "Velocidade: " + String(speed_kmh) + " km/h\n" : 
                     "Sensores de ultrassom não conectados\n") +
                    "Data/Hora: " + String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + " " +
                    String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  // Imprimir a mensagem no Serial Monitor
  Serial.println(mensagem);

  // Enviar a mensagem via Bluetooth
  bluetooth.println(mensagem); // Usar println para adicionar uma nova linha ao final da mensagem

  delay(3000); // Aguardar 3 segundos antes da próxima leitura
}
```

## Bibliotecas Necessárias

Certifique-se de ter as seguintes bibliotecas instaladas no seu Arduino IDE:

- [SoftwareSerial](https://www.arduino.cc/en/Reference/SoftwareSerial)
- [RTClib](https://github.com/adafruit/RTClib)
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)

## Instalação

1. Conecte todos os componentes conforme descrito na seção de conexões.
2. Carregue o código no Arduino.
3. Abra o Serial Monitor para ver as leituras.
4. Conecte-se ao módulo Bluetooth usando um aplicativo de terminal Bluetooth para ver os dados sendo enviados.

## Funcionamento

### Sensores de Ultrassom

Os sensores de ultrassom HC-SR04 são usados para medir a velocidade de um carro de Fórmula E que passa por um ponto específico da pista. A distância entre os dois sensores é conhecida (10 cm), e a diferença de tempo que o carro leva para passar de um sensor ao outro é usada para calcular a velocidade em km/h.

### Sensor de Umidade e Temperatura

O sensor DHT11 mede a umidade e a temperatura da pista, fornecendo informações importantes que podem afetar o desempenho dos carros.

### Módulo RTC DS3231

O módulo RTC DS3231 registra a data e hora exatas em que os eventos ocorrem, garantindo que cada leitura de velocidade, umidade e temperatura seja precisa e marcada temporalmente.

### Módulo Bluetooth

Os dados coletados são enviados via Bluetooth para um aplicativo de terminal Bluetooth, onde podem ser monitorados em tempo real.

## Exemplo de Saída

Quando o código está em execução, você verá uma saída semelhante a esta no Serial Monitor e no aplicativo de terminal Bluetooth:

```
Umidade: 55.0 %
Temperatura: 24.0 C
Velocidade: 45.0 km/h
Data/Hora: 12/6/2024 14:35:50
```

Se os sensores de ultrassom não estiverem conectados ou não detectarem um objeto, a saída será:

```
Umidade: 55.0 %
Temperatura: 24.0 C
Sensores de ultrassom não conectados
Data/Hora: 12/6/2024 14:35:50
```

## Autor

- Geronimo Augusto Nascimento Santos - 557170
- Ana Laura Torres Loureiro - 554375
- Vitor Augusto França de Oliveira - 555469
- Murilo Cordeiro Ferreira - 556727
- Mateus da Costa Leme - 557803

