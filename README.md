# AutomationIoTSample
## Projeto de automação simples
Projeto de automação usando ESP8266 (NodeMCU), lendo dados de sensor de temperatura e umidade e enviando dados para um broker MQTT.

## Hardware
* Placa: ESP8266 (NodeMCU lolin) 
* Sensor: DHT22
* Protoboard
* Cabo usb
* Jumpers


## O que este projeto faz ?
* Conexão com wifi;
* Lê dados de um sensor de temperatura;
* Envia informações para um broker MQTT;
* Pode ser usado o OTA (On The Air) para fazer upload do código via wifi;

OBS: Portas utilizadas no NodeMCU:
D5 - Sensor de temperatura (DHT22)

Pinos DHT22: </br>
1 - VCC </br>
2 - Data </br>
3 - NC </br>
4 - GND </br>
