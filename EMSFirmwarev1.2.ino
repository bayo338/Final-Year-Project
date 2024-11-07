//=====================================================================//
//                                                                     //
//         Solar Energy Management System(EMS) V1.2 Firmware           //
//                                                                     //
//               Developed by Igbaroola Samuel Adebayo                 //
//                                                                     //
//=====================================================================//

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <BH1750.h>
#include <dht.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
const int potPin = A3;    // Analog input pin for potentiometer
const int relay1Pin = 8;  // Replace with your actual relay pin numbers
const int relay2Pin = 7;
const int relay3Pin = 6;
const int relay4Pin = 5; 

BH1750 lightMeter(0x23);

const int currentPin = A2;
int sensitivity = 66;
int adcValue = 0;
int offsetVoltage = 2500;
double adcVoltage = 0;
double currentValue = 0;
// Define analog input
#define ANALOG_IN_PIN A0
#define ANALOG_IN_PIN1 A1
// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
float adc_voltage1 = 0.0;
float in_voltage1 = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 

// Float for Reference Voltage
float ref_voltage = 5.0;

// Integer for ADC value
int adc_value = 0;

dht DHT;
#define DHT11_PIN 4

#define RX 2 // TX of esp8266 in connected with Arduino pin 2
#define TX 3 // RX of esp8266 in connected with Arduino pin 3

String WIFI_SSID = "MACBOOK PRO MAX";    // WIFI NAME
String WIFI_PASS = "Airdrop2Me";    // WIFI PASSWORD
String API = "DRVHSSTV8TJFS8FC";    // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";

int countTrueCommand;
int countTimeCommand; 
boolean found = false;   

SoftwareSerial esp8266(RX,TX); 

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 4);
  lcd.clear();

   // Scroll "Final Year Project" on the first line
  lcd.setCursor(0, 2);
  lcd.print("Final Year Project");
  for (int i = 0; i < 16; i++) {
    lcd.scrollDisplayLeft();
    delay(300); // Adjust the delay for scroll speed
  }

  // Display "ENERGY MANAGEMENT SYSTEM BY SAMUEL"
  lcd.setCursor(0,0);
  lcd.print("     ENERGY   ");
  lcd.setCursor(0,1);
  lcd.print("   MANAGEMENT    ");
  lcd.setCursor(0,2);
  lcd.print("     SYSTEM    ");
  lcd.setCursor(0,3);
  lcd.print("    BY SAMUEL   ");

  // Delay to allow reading the initial message
  delay(3000); // 3 seconds delay
  
  // Clear the LCD and display the next message
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" SUPERVISED BY ");
  lcd.setCursor(0, 2);
  lcd.print("  DR. SOMEFUN   ");

  esp8266.begin(115200);
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + WIFI_PASS + "\"", 20, "OK");
  
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
  Wire.begin();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initializing BH1750"));
  }
}

void loop() {
  int potValue = analogRead(potPin);
  if (potValue >= 700) {
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
    digitalWrite(relay3Pin, LOW);
    digitalWrite(relay4Pin, LOW);
  } else if (potValue >= 600) {
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
    digitalWrite(relay3Pin, LOW);
    digitalWrite(relay4Pin, HIGH);
  } else if (potValue >= 500) {
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
    digitalWrite(relay3Pin, HIGH);
    digitalWrite(relay4Pin, HIGH);
  } else if (potValue >= 400) {
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, HIGH);
    digitalWrite(relay3Pin, HIGH);
    digitalWrite(relay4Pin, HIGH);
  } else {
    digitalWrite(relay1Pin, HIGH);
    digitalWrite(relay2Pin, HIGH);
    digitalWrite(relay3Pin, HIGH);
    digitalWrite(relay4Pin, HIGH);
  }
  delay(100);

  int sensorValue = analogRead(A3);
  delay(500);
  Serial.print("VOLTAGE=");
  Serial.println(sensorValue);
  delay(1000);

  adcValue = analogRead(currentPin);
  adcVoltage = (adcValue / 1024.0) * 5200;
  currentValue = ((adcVoltage - offsetVoltage) / sensitivity);
  delay(2000);
  Serial.print("\t Current = ");
  Serial.println(currentValue, 3);

  adc_value = analogRead(ANALOG_IN_PIN);
  adc_voltage = (adc_value * ref_voltage) / 1024.0;
  in_voltage = adc_voltage * (R1 + R2) / R2;
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);
  delay(500);

  adc_value = analogRead(ANALOG_IN_PIN1);
  adc_voltage1 = (adc_value * ref_voltage) / 1024.0;
  in_voltage1 = adc_voltage1 * (R1 + R2) / R2;
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage1, 2);
  delay(500);

  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(1000);

  float SOLARVOL =  in_voltage;
  float BATTERYVOL =  in_voltage1;
  float temp = DHT.temperature;
  float hum = DHT.humidity;
  float BATTERYCURRENT = currentValue;
  
  // Calculate power
  float power = BATTERYVOL * BATTERYCURRENT;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BATT VOLT=   ");
  lcd.print(BATTERYVOL);
  lcd.setCursor(0, 1);
  lcd.print("SOL VOLT=    ");
  lcd.print(SOLARVOL);
  lcd.setCursor(0, 2);
  lcd.print("BATT CUR=    ");
  lcd.print(BATTERYCURRENT);
  lcd.setCursor(0, 3);
  lcd.print("POWER=     ");
  lcd.print(power);

  // Send data to ThingSpeak
  String getData = "GET /update?api_key=" + API + "&field1=" + SOLARVOL + "&field2=" + BATTERYVOL + "&field3=" + BATTERYCURRENT + "&field4=" + temp + "&field5=" + hum + "&field6=" + power;
  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");
  esp8266.println(getData);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0", 5, "OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while (countTimeCommand < (maxTime * 1)) {
    esp8266.println(command);
    if (esp8266.find(readReplay)) {
      found = true;
      break;
    }
    countTimeCommand++;
  }
  if (found == true) {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  if (found == false) {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  found = false;
}
