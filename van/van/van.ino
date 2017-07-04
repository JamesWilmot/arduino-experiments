/*
  LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

// include the library code:
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <math.h>

//#include <DHT.h>
//#include <DHT_U.h>

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

int cursorX = 0;

//#define DHTPIN 52
//#define DHTTYPE DHT22

//DHT dht(DHTPIN, DHTTYPE);

byte rowPins[ROWS] = { 31, 33, 35, 37 };
byte colPins[COLS] = { 22, 24, 26, 28 };

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 34, 36, 38, 40);

long lastRender = 0;
const long interval = 2000;

void setup() {
  // set up the LCD's number of columns and rows:
  //delay(1000);
  lcd.begin(16, 2);
  delay(100);
  // Print a message to the LCD.

  //dht.begin();

  String line0 = "11:30PM";
  //
  lcd.clear();
  delay(100);
  lcd.print("Daylight spent:");

  lcd.setCursor(0, 1);
  //lcd.print("|");
  //cd.print((char)165);
  //lcd.print("|##########|100%");

  //lcd.print((char)223);
  //lcd.print("C");
  Serial.begin(9600);

  // experiments in time and space
  long jDay = convertToJulianDay(5, 7, 2017);
  Serial.print("Current Julian day: ");
  Serial.println(jDay);
  
  float julianDay = calcJulianDay((float)2458107);
  Serial.print("Truncated Julian day: ");
  Serial.println(julianDay);

  float longWest = 149.13;
  float latNorth = -35.2809;

  float meanSolarNoon = calcMeanSolarNoon(julianDay, longWest);
  Serial.print("Mean solar noon: ");
  Serial.println(meanSolarNoon);

  float solarMeanAnomaly = calcSolarMeanAnomaly(meanSolarNoon);
  Serial.print("Solar Mean Anomaly: ");
  Serial.println(solarMeanAnomaly);

  float eqtnCenter = calcEquationOfTheCenter(solarMeanAnomaly);
  Serial.print("Equation of the center: ");
  Serial.println(eqtnCenter);

  float eclipticLong = calcEclipticLongitude(solarMeanAnomaly, eqtnCenter);
  Serial.print("Ecliptic Longitude: ");
  Serial.println(eclipticLong);

  float jTransit = calcSolarTransit(meanSolarNoon, solarMeanAnomaly, eclipticLong);
  Serial.print("Solar transit: ");
  Serial.println(jTransit);

  float sunDeclination = calcDeclinationSun(eclipticLong);
  Serial.print("Sun declination: ");
  Serial.println(sunDeclination);

  float hourAngle = calcHourAngle(sunDeclination, latNorth);
  Serial.print("Hour angle: ");
  Serial.println(hourAngle);

  float sunrise = calcSunrise(jTransit, hourAngle);
  Serial.print("Sunrise: ");
  Serial.println(sunrise);

  float sunset = calcSunset(jTransit, hourAngle);
  Serial.print("Sunset: ");
  Serial.println(sunset);
}

int counter = 0;
boolean flicker = false;

void loop() {
  // render every 1 second
  unsigned long currentTime = millis();


  if (currentTime - lastRender > interval) {
    daylightView(55, flicker);

    lastRender = currentTime;

    flicker = flicker == true ? false : true;
    //Serial.println(counter);
  }

  /*char key = kpd.getKey();
    if(key) {
    switch (key) {
      case  'C':
        lcd.setCursor(--cursorX, 1);
        lcd.print(" ");
        break;
      default:
        lcd.setCursor(cursorX++, 1);
        lcd.print(key);
    }
    }*/
}

void daylightView(int percent, boolean doBlink) {
  lcd.setCursor(0, 1);

  /* lcd.print(percent); */
  lcd.print("|");
  int counter = 0;
  for (counter = 0; counter < 100; counter = counter + 10) {
    if (counter < percent && counter <= percent - 10) {
      lcd.print("#");
    } else if (counter < percent && counter > percent - 10 && doBlink) {
      lcd.print("#");
    } else {
      lcd.print(" ");
    }
  }
  lcd.print("|");

  if ( (int)(percent / 100) ) {
    lcd.print("100");
  } else  {
    lcd.print(" ");
    if ( !(int)(percent / 10) ) {
      lcd.print(" ");
    }
    lcd.print(percent);
  };
  lcd.print("%");
}

// https://en.wikipedia.org/wiki/Sunrise_equation#Calculate_sunrise_and_sunset
float calcJulianDay(float Jdate) {
  float n = 0.0;

  return (float)(Jdate - 2451545.0 + 0.0008);
}

float calcMeanSolarNoon(float JDay, float longWest) {
  return (float)(JDay - (float)(longWest / 360));
}

float calcSolarMeanAnomaly(float Jstar) {
  float M = 0.0;

  M = fmod((357.5291 + 0.98560028 * Jstar), 360);

  return M;
}

float calcEquationOfTheCenter(float M) {
  float C = 0.0;
  /* 1.9148 \sin(M) + 0.0200 \sin(2  M) + 0.0003 \sin(3  M)*/
  C = 1.9148 * sin(degToRads(M)) + 0.0200 * sin (2 * degToRads(M)) + 0.0003 * sin(3 * degToRads(M));

  return C;
}

float calcEclipticLongitude(float M, float C) {
  float lambda = 0.0;

  lambda = fmod((M + C + 180 + 102.9372) , 360);

  return lambda;
}

float calcSolarTransit(float Jstar, float M, float lambda) {
  float jTransit = 0.0;

  //jTransit = 2451545.5 + Jstar + 0.0053 * sin(degToRads(M)) - 0.0069 * sin(degToRads(2 * lambda));
  jTransit = 0.5 + Jstar + 0.0053 * sin(degToRads(M)) - 0.0069 * sin(degToRads(2 * lambda));
  return jTransit;
}

float calcDeclinationSun(float lambda) {
  float sinDeclination = sin(degToRads(lambda)) * sin(degToRads(23.44));
  float declination = radsToDegrees(asin(sinDeclination));

  return declination;
}

float calcHourAngle(float declinationSun, float northLatitude) {
  float cosHourAngle = 0.0;
  float hourAngle = 0.0;

  cosHourAngle = sin(degToRads(-0.83) - sin(degToRads(northLatitude)) * sin(degToRads(declinationSun)))
                 / (cos(degToRads(northLatitude)) * cos(degToRads(declinationSun)));

  hourAngle = radsToDegrees(acos(cosHourAngle));
  return hourAngle;
}

float calcSunrise(float jTransit, float hourAngle) {
  float jRise = jTransit - hourAngle / 360;
  return jRise;
}

float calcSunset(float jTransit, float hourAngle) {
  float jSet = jTransit + hourAngle / 360;
  return jSet;
}

float degToRads(float x) {
  return x * PI / 180;
}

float radsToDegrees(float rads) {
  return rads * 180 / PI;
}

long convertToJulianDay(int day, int month, int year) {
  long JDN;

  int a = floor((14 - month)/12);
  int y = year + 4800 - a;
  int m = month + 12*a - 3;

  JDN = day;
  JDN += floor( (float)(153*m + 2)/5 );
  JDN += (long)365 * y;
  JDN += floor( (float)(y/4) );
  JDN -= floor( (float)(y/100) );
  JDN += floor( (float)(y/400));
  JDN -= 32045;
  
  return JDN;
}

float jDateTimezoneFix(float jDate, float longitude) {
  float fixedJDate = 0.0;

  fixedJDate = jDate + longitude / 360;

  return fixedJDate;
}

