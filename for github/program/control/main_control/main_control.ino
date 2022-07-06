#define CE 7  //radio CE pin
#define CSN 8 //radio CSN pin
#define l_fonrt 1
#define l_back 2
#define l_left 3
#define l_right 4
#define l_stats 5//led for
#define bt_main  6//button for main light(cycle(front, back, both))
#define bt_left 7//button for left flash
#define bt_right 8//button for right flash
#define bt_dual 9//button for dual flash
#define r_vmotor 14//relay vibrate motor

#include <SPI.h>
#include "RF24.h"

//radio
RF24 rf24(7, 8); // CE腳, CSN腳
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
int mode = 0;
bool b_lmode = false;
bool b_rmode = false;
bool b_dfmode = false; //dual flash

bool b_dualstate = false;
bool b_main = false;
bool b_left = false;
bool b_right = false;
bool b_dual = false;
bool b_flash = false; //whether any flashing lights is on

void setup() {
    //start Serial
    Serial.begin(9600);
  
    // initialize radio
    rf24.begin();
    rf24.setChannel(83);       // 設定頻道編號
    rf24.openWritingPipe(addr); // 設定通道位址
    rf24.setPALevel(RF24_PA_MIN);   // 設定廣播功率
    rf24.setDataRate(RF24_250KBPS); // 設定傳輸速率
    rf24.stopListening();       // 停止偵聽；設定成發射模式
    
    //initialize pinmode
    pinMode(bt_main,INPUT);
    pinMode(bt_left,INPUT);
    pinMode(bt_right,INPUT);
    pinMode(bt_dual,INPUT);
    pinMode(l_fonrt,OUTPUT);
    pinMode(l_back,OUTPUT);
    pinMode(l_left,OUTPUT);
    pinMode(l_right,OUTPUT);
    pinMode(l_stats,OUTPUT);
}

void loop() {
    
    //read button
    if(digitalRead(bt_main) == true){
        delay(200);
        if(digitalRead(bt_main) == true)
            b_main == true;
    }
    
    if(digitalRead(bt_left) == true){
        delay(200);
        if(digitalRead(bt_left) == true)
            b_left == true;
    }
    
    if(digitalRead(bt_right) == true){
        delay(200);
        if(digitalRead(bt_right) == true)
            b_right == true;
    }
    
    if(digitalRead(bt_dual) == true){
        delay(200);
        if(digitalRead(bt_dual) == true)
            b_dual == true;
    }
    
    //react with the btn detect result
    if(b_main == true){
        mode ++;
        if(mode == 4)
            mode = 0;
        if(mode == 0){
            rf24.write(&off, sizeof(off));
            rf24.write(&moff, sizeof(moff));
        }
        if(mode == 1){
            rf24.write(&on, sizeof(on));
            rf24.write(&moff, sizeof(moff));
        }
        if(mode == 2){
            rf24.write(&off, sizeof(off));
            rf24.write(&mon, sizeof(mon));
        }
        if(mode == 3){
            rf24.write(&on, sizeof(on));
            rf24.write(&mon, sizeof(mon));
        }
    }
    
    if(b_left == true){
        if(b_flash == false){
            b_lmode = !b_lmode;
            b_flash = b_lmode;
            rf24.write(&leon, sizeof(leon));
            b_left = false;
        }
        else{
            rf24.write(&foff, sizeof(foff));
            b_left = false;
            b_dualstate = false;
        }
    }
        
    if(b_right == true){
        if(b_flash == false){
            b_rmode = !b_rmode;
            b_flash = b_rmode;
            rf24.write(&ron, sizeof(ron));
            b_right = false;
        }
        else{
            rf24.write(&foff, sizeof(foff));
            b_right = false;
            b_dualstate = false;
        }
    }
    
    if(b_dual){
        if(b_flash == false && b_dualstate == false){
            b_flash = true;
            b_dual = !b_dual;
            rf24.write(&dual, sizeof(dual));
        }
        if(b_flash == true && b_dualstate == false){
            rf24.write(&foff, sizeof(foff));
            rf24.write(&dual, sizeof(dual));
        }
        if(b_flash == true && b_dualstate == true){
            rf24.write(&foff, sizeof(foff));
        }
    }
}
