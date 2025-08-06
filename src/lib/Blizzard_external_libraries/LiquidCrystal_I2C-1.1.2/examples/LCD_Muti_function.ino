#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4); //LiquidCrystal_I2C lcd(0x20,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display or 20 chars and 4 line display
//video https://www.youtube.com/watch?v=PqJQYnVNa3s&list=TLPQMTEwNzIwMjTA5zac0JO0WA&index=2
//#include <LiquidCrystal.h>
//LiquidCrystal lcd(3,4,5,6,7,8);
#define up 3
#define down 2
#define enter 4
#define Clear 5
#define FAN 9
#define batt 8
#define AC A3
#define Temp A2
#define load1 6
#define load2 7

byte batteryl1[]={0x0E,0X1F,0X11,0X11,0X11,0X11,0X11,0X1F};
byte batteryl2[]={0x0E,0X1F,0X11,0X11,0X11,0X1F,0X1F,0X1F};
byte batteryM[]={0x0E,0X1F,0X11,0X11,0X1F,0X1F,0X1F,0X1F};
byte batteryF[]={0x0E,0X1F,0X1F,0X1F,0X1F,0X1F,0X1F,0X1F};
byte fan[]={0x00,0X11,0X0A,0X04,0X0A,0X11,0X00,0X00};
byte pointer[]={0x00,0x00,0x04,0x02,0x1D,0X02,0X04,0X00};

int downsel=0;

void setup() {
 lcd.init();  // initialize the lcd 
 lcd.backlight();
 //lcd.begin(20,4);
 pinMode(up,INPUT_PULLUP);
 pinMode(down,INPUT_PULLUP);
 pinMode(enter,INPUT_PULLUP);
 pinMode(Clear,INPUT_PULLUP);
 pinMode(FAN,OUTPUT);
 pinMode(batt,INPUT);
 pinMode(AC,INPUT);
 pinMode(load1,OUTPUT);
 pinMode(load2,OUTPUT);
 pinMode(Temp,INPUT);
// pinMode(led,OUTPUT);
 
  lcd.setCursor(0,0);
  lcd.print("WELCOME TO ");
  lcd.setCursor(5,1);
  lcd.print("TONY-TECH PRO");
   lcd.setCursor(3,2);
  lcd.print("Electrical and ");
   lcd.setCursor(0,3);
  lcd.print("Electronics Services");
  delay(1000);
  lcd.clear();
 
 lcd.createChar(0, batteryl1); // create a new character
 lcd.createChar(1, batteryl2); // create a new character
 lcd.createChar(2, batteryM); // create a new character
 lcd.createChar(3,  batteryF); // create a new character
 lcd.createChar(4, fan); // create a new character
 lcd.createChar(5,  pointer); // create a new character
 
 //lcd.setCursor(0, 0);
// lcd.write(byte(0)); // when calling lcd.write() '0' must be cast as a byte
// lcd.setCursor(0,1);  
// lcd.write(1);
// lcd.setCursor(0,2);  
// lcd.write(2);
// lcd.setCursor(0,3);  
// lcd.write(3);
// lcd.setCursor(1,0);  
//lcd.write(4);
 // lcd.setCursor(1,1);  
// lcd.write(5);
}


void loop() {
UPandDOWN();

  }
  
   void UPandDOWN(){
   lcd.clear(); 
   switch(downsel){
   case 0: lcd.setCursor(0,0); 
   lcd.write(5);
   break;
    case 1: lcd.setCursor(0,1); 
   lcd.write(5);
   break;
    case 2: lcd.setCursor(0,2); 
   lcd.write(5);
   break;
    case 3: lcd.setCursor(0,3); 
   lcd.write(5);
   break;
    }  
   lcd.setCursor(1,0);
   lcd.print("BATTERY"); 
   lcd.setCursor(1,1);
   lcd.print("TEMPERATURE");  
   lcd.setCursor(1,2);
   lcd.print("AC VOLTAGE"); 
   lcd.setCursor(1,3);
   lcd.print("LOAD");
  if(digitalRead(down) == LOW){ 
    downsel++;
    delay(200);
    }
  if(digitalRead(up)== LOW){
   downsel--;
    delay(100);
    }
  if(digitalRead(enter)== LOW){
    while(1){
     ENTER();
 if(digitalRead(Clear)== LOW){
    lcd.clear();
    }
    }
    }
  
    delay(100);
 }
 void ENTER(){
  switch(downsel){
 case 0: 
  lcd.clear();
  lcd.setCursor(5,0); 
  lcd.print("BATTERY  Status"); 
  lcd.setCursor(0,1);
  lcd.print("Voltage:");
    lcd.setCursor(11,1);
    lcd.print("V");
  lcd.setCursor(0,2);
  lcd.print("Current:");
    lcd.setCursor(11,2);
    lcd.print("A");
  lcd.setCursor(0,3);
  lcd.print("Strangth:"); 
     lcd.setCursor(11,3);
     lcd.write(3);
     delay(150); 
    break;
  case 1:
  lcd.clear();
  lcd.setCursor(1,0); 
  lcd.print("TEMPERATURE  Status"); 
  lcd.setCursor(0,1);
  lcd.print("TEMP1:");
    lcd.setCursor(11,1);
    lcd.print("C");
  lcd.setCursor(0,2);
  lcd.print("TEMP2:");
    lcd.setCursor(11,2);
    lcd.print("C");
  lcd.setCursor(0,3);
  lcd.print("TEMP3:"); 
     lcd.setCursor(11,3);
     lcd.print("C");   
     delay(150);
    break;
 case 2: 
  lcd.clear();
  lcd.setCursor(2,0); 
  lcd.print("AC VOLTAGE  Status"); 
  lcd.setCursor(0,1);
  lcd.print("Voltage:");
    lcd.setCursor(11,1);
    lcd.print("V");
  lcd.setCursor(0,2);
  lcd.print("Current:");
    lcd.setCursor(11,2);
    lcd.print("A");
  lcd.setCursor(0,3);
  lcd.print("Wattage:"); 
     lcd.setCursor(11,3);
    lcd.print("W");    
    delay(150);
    break;
 case 3: 
  lcd.clear();
  lcd.setCursor(5,0); 
  lcd.print("LOAD  Status"); 
  lcd.setCursor(0,1);
  lcd.print("LOAD1:");
    lcd.setCursor(11,1);
    lcd.print("NONE");
  lcd.setCursor(0,2);
  lcd.print("LOAD2:");
    lcd.setCursor(11,2);
    lcd.print("NONE");
  lcd.setCursor(0,3);
  lcd.print("LOAD3:"); 
     lcd.setCursor(11,3);
    lcd.print("NONE");   
    delay(150);
    break;   
    }
  
  }
