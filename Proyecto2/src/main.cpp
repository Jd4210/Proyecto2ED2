#include <Arduino.h>
#include <ESP32SPISlave.h>
#include <stdio.h>
#include <stdint.h>
#include <driver/ledc.h>
#include <driver/gpio.h>
#include "config.h"
#include <LiquidCrystal.h>
#include <Wire.h>


// put function declarations here:

//i2c
#define I2C_ADDR 0x32 // Dirección del esclavo I2C

// pot
#define POT1 35 // Potenciometro

// Variables pines
#define LED 27
#define LED1 33
#define LED2 32

// LCD
#define en 4
#define rs 2
#define d0 16
#define d1 17
#define d2 5
#define d3 18
#define d4 19
#define d5 23
#define d6 26
#define d7 25

float valorPot1 = 0;
uint8_t codPot1 = 0;

// uint8_t datoLED = 0;

// Inicializar LCD con los pines correspondientes
LiquidCrystal lcd(rs, en, d0, d1, d2, d3, d4, d5, d6, d7);

int num = 1;

// DEFINICIONES de spi
const int QUEUE_SIZE = 1;

// VARIABLES GLOBALES
uint8_t datosRx[3] = {0, 0, 0};
uint8_t datosTx[3] = {0, 0, 0};

uint8_t datoLED;
uint16_t tiempo;

ESP32SPISlave slave;

// Funciones

void configurarPines()
{
  pinMode(POT1, INPUT);
}

//i2c
//void onReceive(int len);
// void onRequest();

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32 INICIO");

  // LEDS
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);

  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);

  // LCD
  configurarPines();
  lcd.begin(16, 2);
  lcd.clear();

  // configuracion de SPI
  slave.setDataMode(SPI_MODE0);
  // hspi: cs:15 clk:14 mosi:13 miso:12
  slave.setQueueSize(QUEUE_SIZE);
  slave.begin(HSPI);
  delay(500);
  Serial.println("SPI Slave iniciado");

  //configuracion de I2C
  Wire.begin((uint8_t)I2C_ADDR);
}

void loop()
{

  //*************LCD******************* */
  // ADC READ del potenciometro
  valorPot1 = analogRead(POT1);

  float voltajePot1 = valorPot1 * 3.3 / 4095.0;

  codPot1 = valorPot1 / 16; // Dividir por 16 para mapear de 0-4095 a 0-255

  //I2C: Enviar el valor del potenciometro al maestro
  Wire.slaveWrite(&codPot1, 1); //mantiene el valor guardado y listo, no delay
  
  lcd.setCursor(0, 0); // Coloca el cursor en la primera fila, primera columna
  lcd.print("Pot1: Pot1: LED:");
  lcd.setCursor(0, 1);       // Coloca el cursor en la segunda fila, primera columna
  lcd.print(voltajePot1, 2); // Imprime el valor del voltaje con 2 decimales
  lcd.print("   ");
  lcd.setCursor(6, 1); // Coloca el cursor en la segunda fila, sexta columna
  lcd.print(codPot1);
  lcd.print("   ");
   // Coloca el cursor en la segunda fila, duodécima columna

  // put your main code here, to run repeatedly:

  if (slave.hasTransactionsCompletedAndAllResultsHandled())
  {
    slave.queue(NULL, datosRx, 3);
    slave.trigger();
  }
  if (slave.hasTransactionsCompletedAndAllResultsReady(QUEUE_SIZE))
  {
    slave.numBytesReceivedAll();

    datoLED = datosRx[0];
    tiempo = ((uint16_t)datosRx[1] << 8) | datosRx[2];

    Serial.print("LED recibido: ");
    Serial.println(datoLED);

    Serial.print("Tiempo recibido: ");
    Serial.println(tiempo);

    Serial.print("RAW: ");
    Serial.print(datosRx[0]);
    Serial.print(" ");
    Serial.print(datosRx[1]);
    Serial.print(" ");
    Serial.println(datosRx[2]);

    if (datoLED == 1)
    {
      lcd.setCursor(12, 1);
      lcd.print("1");
      digitalWrite(LED, HIGH);
      delay(tiempo);
      digitalWrite(LED, LOW);

    }

    if (datoLED == 2)
    {
      lcd.setCursor(12, 1);
      lcd.print("2");
      digitalWrite(LED1, HIGH);
      delay(tiempo);
      digitalWrite(LED1, LOW);
    }

    if (datoLED == 3)
    {
      lcd.setCursor(12, 1);
      lcd.print("3");
      digitalWrite(LED2, HIGH);
      delay(tiempo);
      digitalWrite(LED2, LOW);
    }

  }

if (slave.hasTransactionsCompletedAndAllResultsReady(QUEUE_SIZE))
{
    Serial.println("NUEVA TRANSACCION");

    slave.numBytesReceivedAll();

    datoLED = datosRx[0];
    tiempo = ((uint16_t)datosRx[1] << 8) | datosRx[2];

    Serial.print("RAW: ");
    Serial.print(datosRx[0]);
    Serial.print(" ");
    Serial.print(datosRx[1]);
    Serial.print(" ");
    Serial.println(datosRx[2]);

    Serial.print("LED: ");
    Serial.println(datoLED);

    Serial.print("Tiempo: ");
    Serial.println(tiempo);
}

}