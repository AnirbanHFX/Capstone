#include <LiquidCrystal.h>
#define gas_Pin 7
#define nsample 10
#include <TinyGPS.h>

TinyGPS gps;
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int xcal, ycal, zcal;

void checkGas() {
    int gas_value = digitalRead(gas_Pin); 
    if(gas_value==1) {
        digitalWrite(6,HIGH);
        digitalWrite(5,HIGH);
        lcd.setCursor (0, 1);
        lcd.print("ALCOHOL DETECTED");
        lcd.display();
        while(gas_value == 1) { 
            gas_value = digitalRead(gas_Pin);
        }
        digitalWrite(6,LOW);
        digitalWrite(5,LOW);
        lcd.clear();
    }
}

void doGPSmagic(float *flat, float *flon) {
    bool newData = false;
    unsigned long chars;
    unsigned short sentences, failed;
    for (unsigned long start = millis(); millis() - start < 1000;) {
        while (Serial.available()) {
            char c = Serial.read();
            if (gps.encode(c)) 
                newData = true;  
        }
    }
    if (newData) {     //If newData is true
        unsigned long age;
        gps.f_get_position(flat, flon, &age);   
    }
}

void calibrateAccelerometer() {
    lcd.setCursor(0, 0);
    lcd.print("Calibrating");
    lcd.display();
    lcd.setCursor(0, 1);
    xcal = 0;
    ycal = 0;
    zcal = 0;
    for(int i=0; i<nsample; i++) {
        lcd.print(".");
        lcd.display();
        xcal += analogRead(A0);
        ycal += analogRead(A1);
        zcal += analogRead(A2);
        delay(200);
    }
    xcal /= nsample;
    ycal /= nsample;
    zcal /= nsample;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Calibrated!");
}

void checkCrash() {
    uint32_t xval, yval, zval, A;
    xval = abs(analogRead(A0)-xcal);
    yval = abs(analogRead(A1)-ycal);
    zval = abs(analogRead(A2)-zcal);
    A = (int) sqrt(xval*xval + yval*yval + zval*zval);
    if (A >= 299) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Accident");
        lcd.setCursor(0, 1);
        lcd.print("Detected!");
        lcd.display();
        digitalWrite(6,HIGH);
        digitalWrite(5,HIGH);
        sendsms();
        while(1) {}
    }
}

void sendsms() {
    float flat, flon;
    doGPSmagic(&flat, &flon);
    Serial1.print("AT+CMGF=1\r"); 
    delay(100);
    Serial1.println("AT + CMGS = \"+918240305500\""); 
    delay(100);
    Serial1.println("Accident!!Save Our Soul"); 
    Serial1.print("Latitude = ");
    Serial1.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial1.print(" Longitude = ");
    Serial1.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    delay(100);
    Serial1.println((char)26); 
    delay(100);
    Serial1.println();
    delay(100);
}

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    delay(2000);
    lcd.begin(16, 2);  
    lcd.clear();
    pinMode(6,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(A0,INPUT);
    pinMode(A1,INPUT);
    pinMode(A2,INPUT);
    pinMode(4,INPUT);
    calibrateAccelerometer();
}
void checkAccident()
{
  float flat,flon;
  doGPSmagic(&flat, &flon);
  if(flat==30.240455&& flon==-97.8177100)
    {digitalWrite(5,HIGH);
    lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Accident");
        lcd.setCursor(0, 1);
        lcd.print("Prone zone!");
        lcd.display();};
}
void panic()
{
  if(digitalRead(4)==HIGH)
  {
    float flat, flon;
    doGPSmagic(&flat, &flon);
     lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Panic");
        lcd.setCursor(0, 1);
        lcd.print("Detected!");
        lcd.display();
        digitalWrite(6,HIGH);
        digitalWrite(5,HIGH);
    Serial1.print("AT+CMGF=1\r"); 
    delay(100);
    Serial1.println("AT + CMGS = \"+918240305500\""); 
    delay(100);
    Serial1.println("Perverts following me!!!"); 
    Serial1.print("Latitude = ");
    Serial1.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial1.print(" Longitude = ");
    Serial1.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    delay(100);
    Serial1.println((char)26); 
    delay(100);
    Serial1.println();
    delay(100);
  }
  
}
void loop()
{
    checkGas();
    checkCrash();
    checkAccident();
    panic();
    digitalWrite(5,LOW);
}
