#include <ThingSpeak.h>

// Library Add
#include <SoftwareSerial.h>
#include <stdlib.h>  
#include <DHT.h>  
#include <dht11.h>
#include <LiquidCrystal.h>  

#define DHTPIN 8            // SDA PIN Setting
#define DHTTYPE DHT11         // DHT11 Sensor Select
DHT dht(DHTPIN, DHTTYPE);
 
// USER Thingspeak Chnnel "Write API key" 
String apiKey = "1JHHTCSMN2JIH4R8";

// Dust Adapter Module sSetting
int LED = 11;                 // LED 11pPIN Connect
int DUST = 0;                 // DUST Zero Reset 
int samplingTime = 280;  
int deltaTime = 40;  
int sleepTime = 9680;
float dustval = 0;            // Initializing Variables 
float voltage = 0;  
float dustug = 0;  
float dus = 0;  
float a = 0;  

LiquidCrystal lcd(13, 12, 9, 7, 5, 4);
SoftwareSerial ser(2, 3);     // WIFI Module RX,TX AND Serial Object Setting


////////////////////////////////////////////////////

 
void setup() {
  
  dht.begin();
  Serial.begin(9600);           // Serial Speed 9600 Setting    
  ser.begin(9600);              // Software Serial Setting
  ser.println("AT+RST");        // ESP8266 Reset
  pinMode(LED, OUTPUT);         // LED PIN
  pinMode(8, OUTPUT);           // Temperature , Huminity
  lcd.begin(16, 2);             // 16x2 LED
}


////////////////////////////////////////////////////

 
void loop() {
 
  // Dust sensor code
  digitalWrite(LED, LOW);             // LED ON
  delayMicroseconds(samplingTime);
  dustval = analogRead(DUST);         // Dust Value Read 
  delayMicroseconds(deltaTime);
  digitalWrite(LED, HIGH);            // LED OFF
  delayMicroseconds(sleepTime);
  voltage = dustval * (5.0 / 1024.0); // Voltage Value
  dustug = 0.17 * voltage;            // Dust ug Conversion
  dus = dustug * 100;                 // dus value
 
  // DHT11 temp, humi Variables
  int temp = dht.readTemperature();
  int humi = dht.readHumidity();
 
  // String Conversion
  char buf[16];
  String strTemp = dtostrf(temp, 4, 1, buf);
  String strHumi = dtostrf(humi, 4, 1, buf);
  String strdus = dtostrf(dus, 4, 1, buf);
  
  Serial.println(strTemp);
  Serial.println(strHumi);
  Serial.println(dus);

  // LCD Module Code
  lcd.clear();
  analogWrite(6, 120);
  lcd.setCursor(0, 0);         // First Line Print
  lcd.print("H: ");
  lcd.print(humi);
  lcd.print(" %  ");
  lcd.print("T: ");
  lcd.print(temp);
  lcd.print(" C");
  lcd.setCursor(0, 1);         // Second Line Print
  lcd.print("Dust: ");
  if (dus > 0) {
    a = dus;
    lcd.print(a);
  }
  else {
    lcd.print(a);
  } 
  lcd.print("ug");         


  // LED Code
  if (a >= 40 & a < 50) {        // Caution
    analogWrite(10, 20);
    delay(1000);
    analogWrite(10,0);
    delay(1000);
    analogWrite(10, 20);
    delay(1000);
    analogWrite(10,0);
    delay(1000);
    analogWrite(10, 20);
    delay(1000);
    analogWrite(10,0);
    delay(1000);
     analogWrite(10, 20);
    delay(1000);
    analogWrite(10,0);
    delay(1000);
  }
  if ( a >= 50) {                // Bad
    analogWrite(10, 20);
  }
  
  // TCP Connect
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149";              // api.thingspeak.com Connect IP
  cmd += "\",80";                        // api.thingspeak.com Connect Port 80
  ser.println(cmd);
   
  if(ser.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // GET Extract String, Data setting
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr +="&field2=";
  getStr += String(strHumi);
  getStr +="&field3=";
  getStr += String(strdus);
  getStr += "\r\n\r\n";
 
  // Send Data
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);
 
  if(ser.find (">")){
    ser.print(getStr);
  }
  else{
    ser.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");    // Alert user
  }

  // Thingspeak Upload delay
  delay(5000);  
}
