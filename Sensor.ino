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