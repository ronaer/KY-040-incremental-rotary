/* http://forum.arduinoturkiye.com/index.php?p=/discussion/6395/artimsal-enkoder-ile-uzunluk-oelcuemue/p1
   https://www.youtube.com/watch?time_continue=2&v=bXT9G5DQub4
   Bünyamin GÖZÜPEK hocamız tarafından geliştirilmiş olup,
   Dr.TRonik tarafından  uyarlanmıştır...
*/

#include <LiquidCrystal_I2C.h> //https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library  I2C+LCD ekran için
#include <Encoder.h> //http://www.pjrc.com/teensy/arduino_libraries/Encoder.zip Encoder için

//Encoder Tanımı
Encoder enkoder(2, 3); // Kesme Pinleri Uno kart için 2 ve 3 nolu pinlerdir! 

//LCD Tanımı
LiquidCrystal_I2C lcd(0x3F, 16, 2); // LCD çalışmaz ise I2C adresini değiştiriniz.
                                     
//Değişkenler
float cap = 65; // Enkoder Üzerindeki Şaft veya Tekerlek Çapı(mm)
int pulse = 80; // Enkoder Pulse Adedi 

float uzunluk = 0; // Uzunluk Olcumu
long ilkPulse; // Enkoder Pozisyonu

void setup()
{

  lcd.begin(); // LCD Başlatılıyor

  //Serial.begin(9600); // SeriPort gerekirse

  lcd.backlight();
  lcd.print("KY40 ENCODER iLE"); // LCD Giriş Mesajı
  lcd.setCursor(0, 1);
  lcd.print(" UZUNLUK OLCUMU"); // LCD Giriş Mesajı
}

void loop()
{
  long sonPulse = enkoder.read();

  if (sonPulse != ilkPulse)
  {
    ilkPulse = sonPulse;
    float cevre = cap * PI; // Çevre Hesabı
    float hesap = cevre / pulse; // Hesaplanan çevreyi Pulse adedine bölünüyor

    uzunluk = (sonPulse * hesap)/10; // Elde edilen pulse ile hesaplanan 1 pulse uzunluğunu çarpılıyor. /10 ->cm

    Serial.print(sonPulse); // Pulse Sayısı
    Serial.print(" -- ");
    Serial.print(uzunluk); // Milimetre cinsinden uzunluk ölçüsü
    Serial.println(" mm");

    lcd.clear(); // LCD Ekran Siliniyor
    lcd.setCursor(0, 0); // Cursor Ayarlanıyor
    lcd.print("Pulse"); // LCD Ekrana Yazdırma

    lcd.setCursor(0, 1);
    lcd.print(sonPulse);

    lcd.setCursor(7, 0);
    lcd.print("Uzunluk");

    lcd.setCursor(7, 1);
    lcd.print(uzunluk) + lcd.print("cm");
  }
}
//YouTube Dr.TRonik...
