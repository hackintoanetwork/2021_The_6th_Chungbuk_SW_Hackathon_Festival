#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "MFRC522.h"
#include "DHT.h"
#include "SPI.h"

#define SDA_PIN 10
#define RST_PIN 8                         
#define DHTPIN 2
#define DHTTYPE DHT11
#define TRIG 7
#define ECHO 6

unsigned long distance;

DHT dht(DHTPIN, DHTTYPE);

MFRC522 mfrc522(SDA_PIN, RST_PIN);
MFRC522::MIFARE_Key key; 

SoftwareSerial BTSerial(3, 4);

int main_led = 9;
int sub_led = 5;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  dht.begin();
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(TRIG, OUTPUT); 
  pinMode(ECHO, INPUT);
  pinMode (sub_led, OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(1);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  distance = pulseIn(ECHO, HIGH) / 29 / 2;
  Serial.println(distance);
  if (distance < 15) 
  {
    BTSerial.print("\nMotion detected.");
    BTSerial.print("\ndistance : ");
    BTSerial.println(distance);
    delay(5000);
  }
  delay(500);
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Serial.print((int)temperature); Serial.print(" *C, ");
  //Serial.print((int)humidity); Serial.println(" %");

  String humi = "Humi : ";
  humi += (String)humidity;
  humi += "%";

  String temp = "temp : ";
  temp += (String)temperature;
  temp += "C";

  lcd.setCursor(0, 0);
  lcd.print(humi);

  lcd.setCursor(0, 1);
  lcd.print(temp);

  delay(1500);
  if (!mfrc522.PICC_IsNewCardPresent())
  return;
  if (!mfrc522.PICC_ReadCardSerial())
  return;
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
  piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
  piccType != MFRC522::PICC_TYPE_MIFARE_4K){
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  String rfid = "";
  for (byte i = 0; i < 4; i++) 
  {
    rfid +=
    (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(mfrc522.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  digitalWrite(sub_led, HIGH);
  delay (1000);
  digitalWrite(sub_led, LOW);
  BTSerial.print("\nCard key : ");
  BTSerial.println(rfid);
  Serial.print("\nCard key : ");
  Serial.println(rfid);
  if (rfid == "d7:e9:f0:d8"){
    BTSerial.print("반려동물 이름 : 구찌\n 반려동물 생일 : 2013.01.13\n반려동물 나이: 8살\n");
    }
  else if (rfid == "e7:89:f7:d8"){
    BTSerial.print("반려동물 이름: 루이비통 \n반려동물 생일 : 2012.02.11\n반려동물 나이: 9살\n");
    }
  else{
    BTSerial.print("등록되지 않은 카드입니다.\n");
    }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
}
