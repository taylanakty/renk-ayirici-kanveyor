#include <Servo.h>
#include <LiquidCrystal.h> 

// --- PIN TANIMLAMALARI ---
const int MOTOR_PIN         = 3;   // BC337 Transistör
const int MESAFE_SENSOR_PIN = A3;  // LogicState veya BUTON
const int HIZ_POT_PIN       = A0;  // Hız Potu
const int SERVO_PIN         = 9;   // Servo

// Butonlar
const int BTN_KIRMIZI = 13;
const int BTN_MAVI    = 12;
const int BTN_YESIL   = 11;

// LCD (Direkt Bağlantı: RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(8, 7, 6, 5, 4, 2);

Servo ayiriciServo;

// Sayaçlar
int sayacKirmizi = 0;
int sayacMavi    = 0;
int sayacYesil   = 0;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(MESAFE_SENSOR_PIN, INPUT); 
  pinMode(HIZ_POT_PIN, INPUT);       
  
  pinMode(BTN_KIRMIZI, INPUT_PULLUP);
  pinMode(BTN_MAVI, INPUT_PULLUP);
  pinMode(BTN_YESIL, INPUT_PULLUP);

  // Motoru Durdur (Ters Mantık: HIGH = Dur)
  analogWrite(MOTOR_PIN, 255); 

  ayiriciServo.attach(SERVO_PIN);
  lcd.begin(20, 4);
  
  lcd.setCursor(0, 0); 
  lcd.print("RENK AYIRICI");
  delay(1000);

  ayiriciServo.write(90); // Servo başlangıç konumu
  ekranSablonuYaz();
}

void loop() {
  // Okumalar
  int cisimSinyali = digitalRead(MESAFE_SENSOR_PIN); 
  int hizPotDegeri = analogRead(HIZ_POT_PIN);      
  
  // Hız Hesabı (Ters Mantık)
  int motorPWM = map(hizPotDegeri, 0, 1023, 255, 0);
  int yuzdeHiz = map(hizPotDegeri, 0, 1023, 0, 100);

  lcd.setCursor(0, 3);
  lcd.print("Hiz: %"); lcd.print(yuzdeHiz); lcd.print("   ");

  // --- NORMAL ÇALIŞMA ---
  // Cisim sinyali YOKSA motor dönsün
  if (cisimSinyali == 0) {
    analogWrite(MOTOR_PIN, motorPWM);
    lcd.setCursor(0, 0); lcd.print("BANT AKIYOR...    ");
  }

  // --- CİSİM ALGILAMA ---
  // Sensörden sinyal geldiği an (LogicState=1 veya Butona Basıldı)
  if (cisimSinyali == 1) {
    
    // 1. Motoru ANINDA Durdur
    analogWrite(MOTOR_PIN, 255); 
    
    durumYaz("CISIM YAKALANDI!");
    delay(500);

    durumYaz("RENK SECINIZ...");

    // 2. Renk Seçimi Bekle
    bool islemTamam = false;
    while (!islemTamam) {
      if (digitalRead(BTN_KIRMIZI) == LOW) {
        renkIslemi("KIRMIZI", 0);
        sayacKirmizi++;
        islemTamam = true;
      }
      else if (digitalRead(BTN_MAVI) == LOW) {
        renkIslemi("MAVI   ", 90);
        sayacMavi++;
        islemTamam = true;
      }
      else if (digitalRead(BTN_YESIL) == LOW) {
        renkIslemi("YESIL  ", 180);
        sayacYesil++;
        islemTamam = true;
      }
    }
    
    sayaclariGuncelle();
    
    // --- BURASI DEĞİŞTİ ---
    // Artık sensörün 0 olmasını beklemiyoruz.
    // Varsayıyoruz ki servo vurduğu için cisim düştü.
    
    durumYaz("CISIM ITILIYOR...");
    delay(1000); // Cismin banttan düşmesi için sanal bekleme süresi
    
    // LogicState kullanıyorsan bu sırada elinle 0 yapmalısın.
    // Button kullanıyorsan zaten elini çekmişsindir, sorun olmaz.
    
    durumYaz("TEKRAR BASLIYOR");
    delay(500);
    // Döngü başa döner ve motor otomatik çalışır.
  }
}

// --- YARDIMCI FONKSİYONLAR ---
void ekranSablonuYaz() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("BANT AKIYOR...");
  lcd.setCursor(0, 1); lcd.print("Son Renk: YOK");
  lcd.setCursor(0, 2); lcd.print("K:0   M:0   Y:0");
}

void renkIslemi(String renkAdi, int aci) {
  lcd.setCursor(10, 1);
  lcd.print(renkAdi);
  
  ayiriciServo.write(aci); // İttir
  delay(800);              // Servo hızı için bekle
  ayiriciServo.write(90);  // Geri çekil
  delay(500);
}

void sayaclariGuncelle() {
  lcd.setCursor(2, 2); lcd.print(sayacKirmizi);
  lcd.setCursor(8, 2); lcd.print(sayacMavi);
  lcd.setCursor(14, 2); lcd.print(sayacYesil);
}

void durumYaz(String mesaj) {
  lcd.setCursor(0, 0);
  lcd.print(mesaj);
  for(int i=mesaj.length(); i<20; i++) lcd.print(" ");
}