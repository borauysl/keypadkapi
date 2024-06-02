#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte SatirSayisi = 4; 
const byte SutunSayisi = 4;

char keys[SatirSayisi][SutunSayisi] = { 
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte SatirPinleri[SatirSayisi] = {9, 8, 7, 6}; 
byte SutunPinleri[SutunSayisi] = {5, 4, 3, 2};

Keypad keypad1 = Keypad(makeKeymap(keys), SatirPinleri, SutunPinleri, SatirSayisi, SutunSayisi);

const String correctPassword = "2016";
String enteredPassword = "";
int wrongAttempts = 0;
bool isPasswordMode = false;
unsigned long lockoutStart = 0;
const unsigned long lockoutDuration = 20000; // 20 seconds
bool isLocked = false;

void setup() {
  Serial.begin(9600); 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Lutfen sifrenizi");
  lcd.setCursor(0, 1);
  lcd.print("** ile baslatin");
}

void loop() {
  if (isLocked) {
    if (millis() - lockoutStart < lockoutDuration) {
      unsigned long remainingTime = (lockoutDuration - (millis() - lockoutStart)) / 1000;
      lcd.setCursor(0, 0);
      lcd.print("Kilitli: ");
      lcd.print(remainingTime);
      lcd.print(" sn");
      lcd.setCursor(0, 1);
      lcd.print("                    ");
      return;
    } else {
      isLocked = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lutfen sifrenizi");
      lcd.setCursor(0, 1);
      lcd.print("** ile baslatin");
      enteredPassword = "";
    }
  }

  char key = keypad1.getKey(); 

  if (key != NO_KEY) {
    if (!isPasswordMode) {
      if (key == '*') {
        if (enteredPassword.length() == 0) {
          enteredPassword += key;
        } else if (enteredPassword.length() == 1 && enteredPassword[0] == '*') {
          enteredPassword = ""; // "**" girildikten sonra sıfırlanır
          isPasswordMode = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Sifreniz:");
          lcd.setCursor(0, 1);
          lcd.print("            "); // Alt satırı temizler
        }
      } else {
        enteredPassword = ""; // "*" dışındaki girişleri temizle
      }
    } else {
      if (key == '#') {
        if (enteredPassword.length() > 0) {
          enteredPassword.remove(enteredPassword.length() - 1);
          lcd.setCursor(0, 1);
          lcd.print(enteredPassword + " ");
        }
      } else if (key == '*' || (key >= '0' && key <= '9')) {
        enteredPassword += key;
        lcd.setCursor(0, 1);
        lcd.print(enteredPassword + " ");

        if (enteredPassword.length() == 4) {
          if (enteredPassword == correctPassword) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Hosgeldiniz");
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Lutfen sifrenizi");
            lcd.setCursor(0, 1);
            lcd.print("** ile baslatin");
            wrongAttempts = 0;
            isPasswordMode = false;
          } else {
            wrongAttempts++;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Yanlis sifre");
            delay(2000);

            if (wrongAttempts >= 3) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Sistem kitlendi");
              lockoutStart = millis();
              isLocked = true;
              wrongAttempts = 0;
            } else {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Sifre girin:");
              lcd.setCursor(0, 1);
              lcd.print("            "); // Alt satırı temizler
              isPasswordMode = true; // Şifre giriş modunu açık tut
            }
          }
          enteredPassword = "";
        }
      }
    }
  }
}
