#include <LiquidCrystal.h>
#define gas_Pin 7
#include <TinyGPS.h>

TinyGPS gps;
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int gas_value;
void setup() {
    Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);  
  pinMode(6,OUTPUT);
  pinMode(5,OUTPUT);
}

void checkGas() {
    gas_value = digitalRead(gas_Pin); 
    if(gas_value==1) {
        digitalWrite(6,HIGH);
        digitalWrite(5,HIGH);
        lcd.setCursor (0, 1);
        lcd.print("ALCOHOL DETECTED");
        lcd.display();
        while(gas_value == 1) { 
            gas_value = digitalRead(gas_Pin);
        }
        lcd.clear();
    }
    else {
        digitalWrite(6,LOW);
    }
}

void loop()
{
    bool newData = false;
    unsigned long chars;
    unsigned short sentences, failed;
    digitalWrite(5,LOW);
    // For one second we parse GPS data and report some key values
    for (unsigned long start = millis(); millis() - start < 1000;) {
        while (Serial.available()) {
            char c = Serial.read();
            //Serial.print(c);
            if (gps.encode(c)) 
                newData = true;  
        }
    }
    
    if (newData) {     //If newData is true
        float flat, flon;
        unsigned long age;
        gps.f_get_position(&flat, &flon, &age);   
        Serial.print("Latitude = ");
        Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        if(flat==30.240455&& flon==-97.8177100)
        digitalWrite(5,HIGH);
        Serial.print(" Longitude = ");
        Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    }
    
    Serial.println(failed);

    checkGas();

}
