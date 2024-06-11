#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial bluetooth(2, 3);

#define HT A0
#define TipoDHT DHT11

DHT dht(HT, TipoDHT);

const int ldrPin = A1;  // Pino analógico onde o LDR está conectado

void setup(){
  Serial.begin(9600);
  Serial.println("teste de DHT e LDR");
  bluetooth.begin(9600);
  dht.begin();
}

void loop(){
  float u = dht.readHumidity();
  float t = dht.readTemperature();
  int ldrValue = analogRead(ldrPin);

  if(isnan(u) || isnan(t)){
    Serial.println("erro com DHT");
    return;
  }

  // Conversão da leitura do LDR para porcentagem de luminosidade
  float luminosidade = (ldrValue / 1023.0) * 100.0;

  // Criando a string com todas as leituras em linhas separadas
  String mensagem = "Umidade: " + String(u) + " %\n" +
                    "Temperatura: " + String(t) + " C\n" +
                    "Luminosidade: " + String(luminosidade) + " %";

  // Imprimir a mensagem no Serial Monitor
  Serial.println(mensagem);

  // Enviar a mensagem via Bluetooth
  bluetooth.print(mensagem); // Usar print em vez de println para evitar nova linha adicional

  delay(3000);
}