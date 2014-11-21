
 
#include <Servo.h>
#include <LiquidCrystal.h>
 
Servo myservo;
LiquidCrystal lcd(7,6,5,4,3,2);

int SERVO_MIN = 600;
int SERVO_MAX = 1600;

struct servo_freq {
  String lib;
  unsigned long value;
};

int FREQUENCIES_SIZE = 17;
servo_freq frequencies[17] = {
                               { "48h",  172800000},
                               { "24h",  86400000},
                               { "12h",  43200000},
                               { " 8h",  28800000},
                               { " 6h",  21600000},
                               { " 4h",  14400000},
                               { " 2h",  7200000},
                               { " 1h",  3600000},
                               { "40m",  2400000},
                               { "20m",  1200000},
                               { "10m",  600000},
                               { " 5m",  300000},
                               { " 2m",  120000},
                               { " 1m",  60000},
                               { "30s",  30000},
                               { "10s",  10000},
                               { " 5s",  5000}
                            };

int currentFreq = 4;
unsigned long timeSinceLastServo = 0;
unsigned long currentTimeInMillis = 0;
unsigned long lastServoSequenceTimeInMillis = 0; 

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte up[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

byte down[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};

int SENSOR_PIN = A0;
int LCD_BUTTON_1 = 0;
int LCD_BUTTON_2 = 206;
int LCD_BUTTON_3 = 424;
int LCD_BUTTON_4 = 622;
int LCD_BUTTON_5 = 823;

int lastInput = 1023;
 
void setup() 
{ 
  lcd.createChar(0,heart);
  lcd.createChar(1,up);
  lcd.createChar(2,down);
  lcd.begin(16, 2);
  displayTitleLine();

  myservo.attach(9);  // attaches the servo on pin 5 to the servo object 
  Serial.begin(19200); // some servos doesn't work without Serial
  myservo.write(SERVO_MIN);                     
  delay(1000);
}
 
void loop() 
{   
  currentTimeInMillis = millis();
  timeSinceLastServo = currentTimeInMillis - lastServoSequenceTimeInMillis;
  
    displayCurrentFrequency();
    displayRemainingTime();
  
  if(timeSinceLastServo > frequencies[currentFreq].value) { 

    servoSequence();
    lastServoSequenceTimeInMillis = millis();
  }
  else{
    
    
    processInput();
    
    
  }
  
  
  
}

void servoSequence(){
    /* SERVO SEQUENCE */
    for(int val = SERVO_MIN ; val <= SERVO_MAX; val +=1) { 
      myservo.write(val);       
      delay(1);                            
    }
    delay(500);
    for(int val = SERVO_MAX ; val > SERVO_MIN; val -=1) { 
      myservo.write(val);            
      delay(1);                            
    }
    /* END OF SERVO SEQUENCE */ 
}

void displayCurrentFrequency(){
  lcd.setCursor(0,1);
  lcd.print("Fr=");
  lcd.print(frequencies[currentFreq].lib); 
}

void displayRemainingTime(){
  /* DISPLAY REMAINING TIME */
  long remainingTimeInMillis = frequencies[currentFreq].value - timeSinceLastServo;
  lcd.setCursor(8, 1);
  if(remainingTimeInMillis > 0) {
    long remainingTimeInSeconds = remainingTimeInMillis / 1000;
    long remainingTimeInMinutes = remainingTimeInSeconds / 60;
    int remainingTimeInHours = remainingTimeInMinutes / 60;
    long remainingHoursInMinutes = remainingTimeInHours * 60;
    int remainingMinutes = remainingTimeInMinutes % remainingHoursInMinutes;
    long remainingMinutesInSeconds = remainingTimeInMinutes * 60;
    int remainingSeconds = remainingTimeInSeconds % remainingMinutesInSeconds;
    lcd.print(pad(remainingTimeInHours,2));
    lcd.print(":");
    lcd.print(pad(remainingMinutes,2));
    lcd.print(":");
    lcd.print(pad(remainingSeconds,2));
  }
  else{
    lcd.print("00:00:00");
  }
  /* END OF DISPLAY REMAINING TIME */
}

String pad( int number, byte width ) {
  String n = String(number);
  String s = "";
  for (byte i=0; i<width - n.length(); i++){ s += "0"; }
  s += number;
  return s;
}

void processInput(){
 
  int input = analogRead(SENSOR_PIN);
  
  if((input < (lastInput - 10)) || (input > (lastInput + 10))){
    /* NEW INPUT VALUE */ 
    
    lcd.setCursor(0,0);
    if((input > (LCD_BUTTON_1 - 10)) && (input < (LCD_BUTTON_1 + 10))){
          //lcd.print("BUTTON 1 PRESSED");
    }
    else if((input > (LCD_BUTTON_2 - 10)) && (input < (LCD_BUTTON_2 + 10))){
          //lcd.print("BUTTON 2 PRESSED");
          selectLowerFrequency();
    }
    else if((input > (LCD_BUTTON_3 - 10)) && (input < (LCD_BUTTON_3 + 10))){
          //lcd.print("BUTTON 3 PRESSED"); 
          selectHigherFrequency();     
    }    
    else if((input > (LCD_BUTTON_4 - 10)) && (input < (LCD_BUTTON_4 + 10))){
          //lcd.print("BUTTON 4 PRESSED");      
    }
    else if((input > (LCD_BUTTON_5 - 10)) && (input < (LCD_BUTTON_5 + 10))){
          //lcd.print("BUTTON 5 PRESSED");   
          servoSequence();   
    }    
    else {
      displayTitleLine();
      lcd.setCursor(6,1);
      lcd.write(" ");
    }
    lastInput = input;
  }
   
}

void displayTitleLine(){
  lcd.setCursor(0,0);
  lcd.print("DISTROCROQ' V1 ");
  lcd.write(byte(0));
}

void selectLowerFrequency(){
  if(currentFreq > 0) currentFreq--;
  lcd.setCursor(6,1);
  lcd.write(byte(1));
}

void selectHigherFrequency(){
  if(currentFreq < (FREQUENCIES_SIZE - 1)) currentFreq++;
  lcd.setCursor(6,1);
  lcd.write(byte(2));
}
