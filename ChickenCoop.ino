#include <SoftwareSerial.h>
#include <SD.h>
#include <RTClib.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

RTC_DS1307 rtc;

const int CO2_SENSOR = A0;
const int NH3_SENSOR = A1;
const int CH4_SENSOR = A2;
const int H2S_SENSOR = A3;

const int MAX_CO2 = 800;
const int MAX_NH3 = 10;
const int MAX_CH4 = 100;
const int MAX_H2S = 10;

const int SD_CS = 10;
const int GSM_RX = 8;
const int GSM_TX = 9;

SoftwareSerial gsmSerial(GSM_RX, GSM_TX);

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed");
    return;
  }
  
  if (!rtc.begin()) {
    Serial.println("RTC initialization failed");
    return;
  }
  
  if (!rtc.isrunning()) {
    Serial.println("RTC is not running");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int co2 = analogRead(CO2_SENSOR);
  int nh3 = analogRead(NH3_SENSOR);
  int ch4 = analogRead(CH4_SENSOR);
  int h2s = analogRead(H2S_SENSOR);
  DateTime now = rtc.now();
  
  String data = String(now.year(), DEC) + "-" + String(now.month(), DEC) + "-" + String(now.day(), DEC) + " " + String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + "," + String(temperature, 1) + "," + String(humidity, 1) + "," + String(co2, DEC) + "," + String(nh3, DEC) + "," + String(ch4, DEC) + "," + String(h2s, DEC);
  
  Serial.println(data);
  
  File dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  }
  
  if (co2 > MAX_CO2 || nh3 > MAX_NH3 || ch4 > MAX_CH4 || h2s > MAX_H2S) {
    sendSMS(data);
  }
  
  displayData(temperature, humidity, co2, nh3, ch4, h2s);
  
  delay(300000);
}

void sendSMS(String data) {
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  
  gsmSerial.println("AT+CMGS=\"+1234567890\"");
  delay(1000);
  
  gsmSerial.print("Alert: Sensor values exceeded maximum limit. Data: ");
  gsmSerial.println(data);
  delay(1000);
  
  gsmSerial.write(26);
  delay(1000);
}

void displayData(float temperature, float humidity, int co2, int nh3, int ch4, int h2s) {
  // Code to display data on the TFT 3.5 inch touch screen
}
// Check if any sensor value exceeds the maximum set parameter
if (temp > MAX_TEMP || humidity > MAX_HUMIDITY || co2 > MAX_CO2 || nox > MAX_NOX || methane > MAX_METHANE || h2s > MAX_H2S) {
    // Send SMS message containing sensor data to owner
    String smsText = "Alert: Sensor values exceed maximum limit!\n\n";
    smsText += "Temperature: " + String(temp) + "°C\n";
    smsText += "Humidity: " + String(humidity) + "%\n";
    smsText += "CO2: " + String(co2) + " ppm\n";
    smsText += "NOx: " + String(nox) + " ppm\n";
    smsText += "Methane: " + String(methane) + " ppm\n";
    smsText += "H2S: " + String(h2s) + " ppm";
    gsmModule.sendSMS(ownerNumber, smsText);
}

// Check if owner requested to refresh the sensor data
if (refreshData) {
    // Take new current data and display to the TFT display
    readSensors();
    displayData();
    refreshData = false;
}

// Check if there is any SMS message received from owner
if (gsmModule.isMessageAvailable()) {
    // Read the SMS message and check if it has the right keyword
    String smsText = gsmModule.readMessage();
    if (smsText.indexOf("REFRESH") >= 0) {
        // Take new current data, save it to the SD card, display it to the TFT display, and send it back to the owner
        readSensors();
        saveDataToSDCard();
        displayData();
        gsmModule.sendSMS(ownerNumber, "Current sensor data:");
        gsmModule.sendSMS(ownerNumber, "Temperature: " + String(temp) + "°C");
        gsmModule.sendSMS(ownerNumber, "Humidity: " + String(humidity) + "%");
        gsmModule.sendSMS(ownerNumber, "CO2: " + String(co2) + " ppm");
        gsmModule.sendSMS(ownerNumber, "NOx: " + String(nox) + " ppm");
        gsmModule.sendSMS(ownerNumber, "Methane: " + String(methane) + " ppm");
        gsmModule.sendSMS(ownerNumber, "H2S: " + String(h2s) + " ppm");
    }
}

// Sleep for 5 minutes to conserve battery
delay(300000);
}
