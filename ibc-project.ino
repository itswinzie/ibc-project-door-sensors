
#define REMOTEXY_MODE__WIFI_POINT
#include <ESP8266WiFi.h>
#include <RemoteXY.h>

#define REMOTEXY_WIFI_SSID "ibc project"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] = {
  255,1,0,10,0,92,0,19,0,0,0,0,31,1,106,200,1,1,5,0,
  2,25,10,58,35,1,3,26,149,1,79,78,0,79,70,70,0,72,33,135,
  48,48,4,166,140,2,26,0,0,0,0,0,0,200,66,0,0,0,0,70,
  72,73,18,18,16,26,148,0,70,72,100,18,18,16,26,37,0,73,20,76,
  21,44,4,128,0,178,26,0,0,0,0,0,0,200,66,0,0,0,0
};

struct {
  uint8_t switch_relay;
  float gas_sensor;
  uint8_t led_green;
  uint8_t led_red;
  float ultrasonic;
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define GAS_PIN     A0   
#define TRIG_PIN    D5   
#define ECHO_PIN    D4  
#define RELAY_PIN   D7   
#define LED_GREEN   D0   
#define LED_RED     D8   

LiquidCrystal_I2C lcd(0x27, 20, 4); 

float jarakCm = 0.0;      
int nilaiGas = 0;      
bool ralatJarak = false;  
bool sistemAktif = false;  
bool kondisiBuka = false; 

unsigned long sebelumMillis = 0;
const long selangMasa = 500;

float bacaJarakCM();
void bacaSemuaSensor();
void prosesLogikUtama();
void kawalOutputFizikal();
void hantarDataKeRemoteXY();
void paparKeLCD();
void paparSerial(); 

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nMemulakan Setup...");
  RemoteXY_Init();
  Serial.println("RemoteXY Dimulakan.");

  Wire.begin(D2, D1);
  lcd.init();
  Serial.println("LCD init...");
  lcd.backlight();
  Serial.println("LCD backlight ON.");
  lcd.setCursor(0, 0);
  lcd.print("Sistem Memulakan...");
  delay(500);

  pinMode(GAS_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  Serial.println("Pin Mode Ditetapkan.");

  digitalWrite(TRIG_PIN, LOW); 

  digitalWrite(RELAY_PIN, LOW);   
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  Serial.println("Keadaan Awal: Pintu Kunci, LED Merah ON.");

  lcd.clear();
  lcd.print("Sistem Sedia");
  delay(1000);
  lcd.clear();
  Serial.println("Setup selesai.");
}

void loop() {
  RemoteXY_Handler(); 

  unsigned long sekarang = millis();
  if (sekarang - sebelumMillis >= selangMasa) {
    sebelumMillis = sekarang; 

    bacaSemuaSensor();
    prosesLogikUtama();
    kawalOutputFizikal();
    hantarDataKeRemoteXY();
    paparKeLCD();
    paparSerial(); 
  }
}

float bacaJarakCM() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long durasi = pulseIn(ECHO_PIN, HIGH, 25000UL);

  if (durasi > 0) {
    ralatJarak = false;
    return durasi * 0.0343 / 2.0;
  } else {
    ralatJarak = true;
    return -1.0; 
  }
}

void bacaSemuaSensor() {
  nilaiGas = analogRead(GAS_PIN);
  jarakCm = bacaJarakCM();

}


void prosesLogikUtama() {
  sistemAktif = (RemoteXY.switch_relay == 1);
  kondisiBuka = (!ralatJarak && jarakCm > 10.0 && jarakCm < 100.0);
}

void kawalOutputFizikal() {
  if (!sistemAktif) {
    digitalWrite(RELAY_PIN, LOW);   
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
  } else {
    if (kondisiBuka) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);
    } else {
      digitalWrite(RELAY_PIN, LOW); 
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
    }
  }
}

void hantarDataKeRemoteXY() {
  RemoteXY.gas_sensor = (float)nilaiGas;
  RemoteXY.ultrasonic = ralatJarak ? -1.0 : jarakCm; 
  RemoteXY.led_green = digitalRead(LED_GREEN);
  RemoteXY.led_red = digitalRead(LED_RED);     
}

void paparKeLCD() {

  String line1 = "";
  String line2 = "";
  String line3 = "Jarak: ";
  String line4 = "Gas: " + String(nilaiGas);

  if (!sistemAktif) {
    line1 = "Sistem: OFF";
    line2 = "Pintu: DIKUNCI";
    line3 += "--";
  } else {
    line1 = "Sistem: ON";
    if (kondisiBuka) {
      line2 = "Pintu: TERBUKA";
    } else {
      line2 = "Pintu: TERTUTUP";
    }
    if (ralatJarak) {
      line3 += "Ralat";
    } else {
      line3 += String(jarakCm, 1) + " cm"; 
    }
  }

  lcd.setCursor(0, 0); lcd.print(line1.substring(0, 20) + "                    ");
  lcd.setCursor(0, 1); lcd.print(line2.substring(0, 20) + "                    ");
  lcd.setCursor(0, 2); lcd.print(line3.substring(0, 20) + "                    ");
  lcd.setCursor(0, 3); lcd.print(line4.substring(0, 20) + "                    ");
}

void paparSerial() {
  Serial.println("--------------------");
  Serial.print("Masa: "); Serial.println(millis() / 1000.0, 1);
  Serial.print("Suis Utama (App): "); Serial.println(sistemAktif ? "ON" : "OFF");
  Serial.print("Gas (raw): "); Serial.println(nilaiGas);
  Serial.print("Jarak: ");
  if(ralatJarak) Serial.print("Ralat"); else {Serial.print(jarakCm, 1); Serial.print(" cm");}
  Serial.print(" | Kondisi Jarak (>10 & <100): "); Serial.println(kondisiBuka ? "YA" : "TIDAK");
  Serial.print("Status Relay Sebenar: "); Serial.println(digitalRead(RELAY_PIN) ? "AKTIF" : "TIDAK AKTIF");
  Serial.print("Status LED Hijau: "); Serial.println(digitalRead(LED_GREEN) ? "ON" : "OFF");
  Serial.print("Status LED Merah: "); Serial.println(digitalRead(LED_RED) ? "ON" : "OFF");
}
