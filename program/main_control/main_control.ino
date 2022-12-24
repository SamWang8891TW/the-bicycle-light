#define bzsw true   //true to enable buzzer, false to disable



#define CE 7   //radio CE pin
#define CSN 8  //radio CSN pin
#define l_fonrt A0
#define l_back A1
#define l_left A2
#define l_right A3
#define l_stats A4  //led for
#define bt_main 3   //button for main light(cycle(front, back, both))
#define bt_left 4   //button for left flash
#define bt_right 5  //button for right flash
#define bt_dual 6   //button for dual flash
#define vmotor 9    //relay vibrate motor
#define bz 2        //relay bz


#include <SPI.h>
#include "RF24.h"

//radio
RF24 rf24(CE, CSN);  // CE腳, CSN腳
const byte addr[] = "1Node";

/*
Define Radio.
a --> back mid on
b --> back mid off
c --> back left on
d --> back right on
e --> back flash off
f --> back dual blink
g --> front mid on
h --> front mid off
*/
char on[32] = "g";
char off[32] = "h";

char mon[32] = "a";
char moff[32] = "b";
char leon[32] = "c";
char ron[32] = "d";
char foff[32] = "e";
char dual[32] = "f";


/*
0->off
1->front
2->back
3->dual
*/
bool b_mmode = false;
bool b_lmode = false;
bool b_rmode = false;
bool b_dfmode = false;  //dual flash
bool b_dualstate = false;

bool b_main = false;
bool b_left = false;
bool b_right = false;
bool b_dual = false;
bool b_flash = false;  //whether any flashing lights is on

void setup() {
  //start Serial
  Serial.begin(9600);

  // initialize radio
  rf24.begin();
  rf24.setChannel(83);           // 設定頻道編號
  rf24.openWritingPipe(addr);    // 設定通道位址
  rf24.setPALevel(RF24_PA_MIN);  // 設定廣播功率
  rf24.setDataRate(RF24_1MBPS);  // 設定傳輸速率
  rf24.stopListening();          // 停止偵聽；設定成發射模式

  //initialize pinmode
  pinMode(bt_main, INPUT);
  pinMode(bt_left, INPUT);
  pinMode(bt_right, INPUT);
  pinMode(bt_dual, INPUT);
  pinMode(l_fonrt, OUTPUT);
  pinMode(l_back, OUTPUT);
  pinMode(l_left, OUTPUT);
  pinMode(l_right, OUTPUT);
  pinMode(l_stats, OUTPUT);
  pinMode(bz, OUTPUT);
  pinMode(vmotor, OUTPUT);
  beep();
  Serial.println("Starting loop!");
}

void loop() {
  //read button
  if (digitalRead(bt_main) == true) {
    delay(200);
    if (digitalRead(bt_main) == true)
      Serial.println("Main trig");
    b_main = true;
    beep();
    delay(500);
  } else if (digitalRead(bt_left) == true) {
    delay(200);
    if (digitalRead(bt_left) == true)
      Serial.println("Left trig");
    b_left = true;
    beep();
    delay(500);
  } else if (digitalRead(bt_right) == true) {
    delay(200);
    if (digitalRead(bt_right) == true)
      Serial.println("Right trig");
    b_right = true;
    beep();
    delay(500);
  } else if (digitalRead(bt_dual) == true) {
    delay(200);
    Serial.println("Dual trig");
    b_dual = true;
    beep();
    delay(500);
  }

  //react with the btn detect result
  if (b_main == true) {
    b_main = false;
    Serial.print("Main send sig, mode:");
    b_mmode = !b_mmode;

    if(b_mmode){
      rf24.write(&on, sizeof(on));
      rf24.write(&mon, sizeof(mon));
      rf24.write(&on, sizeof(on));
      rf24.write(&mon, sizeof(mon));
      rf24.write(&on, sizeof(on));
      rf24.write(&mon, sizeof(mon));
    }
    else{
      rf24.write(&off, sizeof(off));
      rf24.write(&moff, sizeof(moff));
      rf24.write(&off, sizeof(off));
      rf24.write(&moff, sizeof(moff));
      rf24.write(&off, sizeof(off));
      rf24.write(&moff, sizeof(moff));
    }
  }

  if (b_left == true) {
    Serial.println("l_triggered");
    b_left = false;
    b_lmode = !b_lmode;
    b_flash = b_lmode;
    if (b_flash == false) {
      Serial.println("lon");
      rf24.write(&leon, sizeof(leon));
      rf24.write(&leon, sizeof(leon));
      rf24.write(&leon, sizeof(leon));
      b_left = false;
    } else {
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      Serial.println("loff");
      b_left = false;
      b_dualstate = false;
    }
  }

  if (b_right == true) {
    Serial.println("r_triggered");
    b_right = false;
    b_rmode = !b_rmode;
    b_flash = b_rmode;
    if (b_flash == false) {
      rf24.write(&ron, sizeof(ron));
      rf24.write(&ron, sizeof(ron));
      rf24.write(&ron, sizeof(ron));
      Serial.println("r_on");
      b_right = false;
    } else {
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      Serial.println("r_off");
      b_right = false;
      b_dualstate = false;
    }
  }

  if (b_dual) {
    Serial.println("d_triggered");
    //Serial.println(b_dfmode);
    b_dual = false;
    b_flash = false;
    if(b_dfmode){
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
    }
    if(b_dfmode == false){
      rf24.write(&dual, sizeof(dual));
      rf24.write(&dual, sizeof(dual));
      rf24.write(&dual, sizeof(dual));
    }
    b_dfmode = !b_dfmode;
    delay(500);
  }
}


void beep() {
  if (bzsw == true)
    digitalWrite(bz, HIGH);
  digitalWrite(vmotor, HIGH);
  delay(100);
  digitalWrite(bz, LOW);
  delay(100);
  digitalWrite(vmotor, LOW);
}



