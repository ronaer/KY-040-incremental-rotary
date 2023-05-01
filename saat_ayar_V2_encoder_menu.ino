/*
 * Dr.TRonik / YouTube / MAYIS 2023 / İzmir / Türkiye 
 * DS3231 RTC modül ve P10 Saat
 * KY 040 encoder ile saati manuel ayarlayabilme
 * Derleme ve karta yükleme öncesi, tüm kütüphaneler arduino ide'sine yüklenmiş olmalıdır...
 * YouTube: https://youtu.be/0qD9_-nSpcQ
 * Bağlantı: https://github.com/ronaer/KY-040-incremental-rotary/blob/master/ds3231_encoder_setup_bb.png
 * 
*/

/********************************************************************
  GLOBALS___GLOBALS___GLOBALS___GLOBALS___GLOBALS___GLOBALS___
 ********************************************************************/
//RealTimeClock
#include "RTClib.h"
RTC_DS3231 rtc;

//Encoder
#include <Encoder.h>  //http://www.pjrc.com/teensy/arduino_libraries/Encoder.zip
Encoder enkoder(2, 3);
// DT-->D2; CLK-->D3; SW-->D0; Vcc-->5V; GND-->GND;

//SPI for DMD comminication
#include "SPI.h"

//DMD2 setup for P10
#include "DMD2.h"
#include <fonts/SystemFont5x7.h>
#include <fonts/angka6x13.h>
SoftDMD dmd(1, 1);

//Variables
int saat, dakika, saniye, p10_Brightness, newHour, newMinute, counter;
long ilkPulse;

//Definitions
#define sw_pin 0             // sw_pin BUTTON PIN NUMBER for enter setup
int sw_counter = 0;          // Butonumuzun kaç defa basıldığı değerini tutan değişken
bool buttonState_sw = HIGH;  // Butonun LOW ya da HIGH olduğunu tutan değişken

char hour_[3];    //2 characters + null
char minute_[3];  //2 characters + null

/********************************************************************
  SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___
 ********************************************************************/
void setup() {
  Serial.begin(9600);
  dmd.begin();
  dmd.clearScreen();

  pinMode(sw_pin, INPUT_PULLUP);
  digitalWrite(sw_pin, HIGH);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Bilgisayar saatine eşitleme
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // Ocak 21 2014 saat 3 0 0 a ayarlama
}

/********************************************************************
  LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__
 ********************************************************************/
void loop() {
  static unsigned long timer = millis();
  set_bright();
  dmd.setBrightness(p10_Brightness);  // set_bright() fonksiyonundan dönen değere göre panel parlaklığı...
  DateTime now = rtc.now();           // loop başlangıcında...
  saat = now.hour();                  // saat değişkenimiz saat modülünün saatine,
  dakika = now.minute();              // dakika değişkenimiz saat modülünün dakikasına,
  saniye = now.second();              // saniye değişkenimiz saat modülünün saniyesine,


  //-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- SET CLOCK
  buttonState_sw = digitalRead(sw_pin);
  if (buttonState_sw == LOW) {
    delay(300);
    sw_counter++;
    if (sw_counter > 2) {
      sw_counter = 0;
    }
  }

  //-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- SwitchCase
  switch (sw_counter) {
    case 0:
      if (millis() - timer > 1000) {
        timer = millis();
        print_clock();
        printSecond();
      }
      break;

    case 1:
      setHour();
      break;

    case 2:
      setMinute();
      break;
  }
}

//-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -


/********************************************************************
  VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs
********************************************************************/
//=========================================== Saati panele yazdırma
void print_clock() {
  //________saniye efekti_________
  dmd.selectFont(SystemFont5x7);
  if (millis() / 1000 % 2 == 0)  // her 1 saniye için
  {
    dmd.drawChar(14, 5, ':');  //iki noktayı göster
  } else {
    dmd.drawChar(13, 5, ' ');  // gösterme
  }

  dmd.selectFont(angka6x13);  //Font seçimi

  sprintf(hour_, "%2d", saat);       // 2 basamak ve değer <10 ise başına 0 koymadan (01:09 ---> 1:09 olarak biçimlendirme)
  sprintf(minute_, "%02d", dakika);  // 2 basamak ve değer <10 ise başına 0 koyarak (1:9 ---> 1:09) biçimlendirme
  dmd.drawString(0, 1, hour_);
  dmd.drawString(19, 1, minute_);
}

//=============================================set hour
void setHour() {
  for (int i = 0; i < 30; i++) { dmd.setPixel(i, 0, 0); }
  for (int i = 31; i < 60; i++) { dmd.setPixel(i, 1, 0); }

  dmd.selectFont(SystemFont5x7);
  dmd.drawChar(14, 5, ':');

  char newHour_[3];
  sprintf(newHour_, "%2d", newHour);
  dmd.selectFont(angka6x13);
  dmd.drawString(0, 1, newHour_);
  dmd.drawString(19, 1, "  ");


  DateTime now = rtc.now();
  newHour = now.hour();

  long sonPulse = enkoder.read();

  if (sonPulse > ilkPulse) {
    ilkPulse = sonPulse;
    newHour++;
    if (newHour > 23) newHour = 0;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHour, now.minute(), 0));
    DateTime now = rtc.now();
  }

  if (sonPulse < ilkPulse) {
    ilkPulse = sonPulse;
    newHour--;
    if (newHour < 0) newHour = 23;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHour, now.minute(), 0));
    DateTime now = rtc.now();
  }
}

//=============================================set minute
void setMinute() {
  for (int i = 0; i < 30; i++) { dmd.setPixel(i, 0, 0); }
  for (int i = 31; i < 60; i++) { dmd.setPixel(i, 1, 0); }

  dmd.selectFont(SystemFont5x7);
  dmd.drawChar(14, 5, ':');

  char newMinute_[3];
  dmd.selectFont(angka6x13);
  sprintf(newMinute_, "%02d", newMinute);
  dmd.drawString(0, 1, "  ");
  dmd.drawString(19, 1, newMinute_);

  DateTime now = rtc.now();
  newMinute = now.minute();

  long sonPulse = enkoder.read();

  if (sonPulse > ilkPulse) {
    ilkPulse = sonPulse;
    newMinute++;
    if (newMinute > 59) newMinute = 0;

    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), newMinute, 0));
    delay(120);
    DateTime now = rtc.now();
  }
  if (sonPulse < ilkPulse) {
    ilkPulse = sonPulse;
    newMinute--;
    if (newMinute < 0) newMinute = 59;

    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), newMinute, 0));
    delay(120);
    DateTime now = rtc.now();
  }
}


//===========================================Saniye efekti
void printSecond() {
  DateTime now = rtc.now();
  saniye = now.second();

  if (saniye <= 29) {
    dmd.setPixel(1 + saniye, 0);
    dmd.setPixel(saniye, 0, 0);
  } else {
    dmd.setPixel(saniye - 29, 1);
    dmd.setPixel(saniye - 30, 1, 0);
  }
  if (saniye == 30) dmd.setPixel(30, 0, 0);
  if (saniye == 0) dmd.setPixel(30, 1, 0);
}


//============================Saate göre parlaklık  ayarlama
int set_bright() {
  if (saat >= 8 && saat < 12) {
    p10_Brightness = 30;
  } else if (saat >= 12 && saat < 19) {
    p10_Brightness = 50;
  } else if (saat >= 19 && saat < 22) {
    p10_Brightness = 10;
  } else if (saat >= 22 && saat < 8) {
    p10_Brightness = 1;
  } else p10_Brightness = 1;
  return p10_Brightness;
}


/*___İletişim:
e-posta: bilgi@ronaer.com
https://www.instagram.com/dr.tronik2023/   
YouTube: Dr.TRonik: www.youtube.com/c/DrTRonik
PCBWay: https://www.pcbway.com/project/member/shareproject/?bmbno=A0E12018-0BBC-4C
*/
