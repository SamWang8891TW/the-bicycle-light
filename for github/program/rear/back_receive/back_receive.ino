/*
Define Radio.
a --> mid on
b --> mid off
c --> left on
d --> right on
e --> flash off
f --> dual blink
*/

#define flashing_cycle  800//in ms
#define CE 7  //radio CE pin
#define CSN 8 //radio CSN pin
#define r_mid  1 //relay_middle
#define r_left  2  //relay_left
#define r_right  3  //relay_right
#define l_stats  4//led_stats (flash twice when ready, every radio receive flash once)
#define l_mid  5//led_mid (indicate)
#define l_left  6//led_left (indicate)
#define l_right 9//led_right (indicate)
#define bt_main 14//main light(when pressed, disable all 方向燈)

#include <SPI.h>
#include "RF24.h"


//radio
RF24 rf24(CE, CSN);
const byte addr[] = "1Node";
const byte pipe = 1;  // 指定通道編號


bool b_mid = false; //bool for mid
bool b_left = false;  //bool for left
bool b_right = false;  //bool for right

/*
Define Radio.
a --> mid on
b --> mid off
c --> left on
d --> right on
e --> flash off
f --> dual blink
*/
char mon[32] = "a";
char moff[32] = "b";
char leon[32] = "c";
char ron[32] = "d";
char foff[32] = "e";
char dual[32] = "f";


unsigned long previousMillis = 0;  


void setup() {
    //start Serial
    Serial.begin(9600);
  
    // initialize radio
    rf24.begin();
    rf24.setChannel(83);  // 設定頻道編號
    rf24.setPALevel(RF24_PA_MIN);
    rf24.setDataRate(RF24_1MBPS);
    rf24.openReadingPipe(pipe, addr);  // 開啟通道和位址
    rf24.startListening();  // 開始監聽無線廣播
    Serial.print("Radio initialized! Receiving!\n");
    Serial.print("CH83, 1MBPS, pipe1, addr=1Node\n");

    //pin initialize
    
    pinMode(r_mid, OUTPUT);
    pinMode(r_left, OUTPUT);
    pinMode(r_right, OUTPUT);
    pinMode(l_stats, OUTPUT);
    pinMode(l_mid, OUTPUT);
    pinMode(l_left, OUTPUT);
    pinMode(l_right, OUTPUT);
    pinMode(bt_main, INPUT);
    Serial.println("Pin initialized!");
    Serial.println("Starting loop:");
    digitalWrite(l_stats, HIGH);
    delay(1000);
    digitalWrite(l_stats, LOW);
    
}

void loop() {
    //Serial.println("looped");
    //write bool status to relays and indicate lights
    digitalWrite(r_mid, b_mid);
    digitalWrite(r_mid, l_mid);

    while(b_left || b_right){
      unsigned long currectMillis = millis();
      if(currectMillis - previousMillis >= flashing_cycle){
        previousMillis = currectMillis;
        if(b_right == false)
          digitalWrite(r_right, LOW);
        else
          digitalWrite(r_right, HIGH);
        if(b_left == false)
          digitalWrite(r_left, LOW);
        else
          digitalWrite(r_left, HIGH);   
      }
    }

        
    //detect button status and react to it
    
    if(digitalRead(bt_main) == true){
        b_mid = !b_mid;
        b_left = false;
        b_right = false;
        Serial.println("Button pressed, turning signal light off and change mid light status.");
    }
    
    
    
    //radio receive
    if (rf24.available(&pipe)){
        //Serial.println("entered");
        char msg[32] = "";
        //Serial.println("oneee");
        rf24.read(&msg, sizeof(msg));
        //Serial.println("twoooo");
        Serial.println(msg); // 顯示訊息內容
        
        //Serial.println("Starting if");
        if(strcmp(msg, mon) == 0){
            digitalWrite(l_stats, HIGH);
            Serial.println("Mid on!");
            b_mid = true;
            delay(500);
            digitalWrite(l_stats, LOW);
        }
        else if(strcmp(msg, moff) == 0){
            digitalWrite(l_stats, HIGH);
            Serial.println("Mid off!");
            b_mid = false;
            delay(500);
            digitalWrite(l_stats, LOW);
        }
        else if(strcmp(msg, leon) == 0){
            previousMillis = millis() - 800; //let the light trigger immediately
            b_right = false; //turn off the other
            digitalWrite(l_stats, HIGH);
            Serial.println("Left on!");
            b_left = true;
            delay(500);
            digitalWrite(l_stats, LOW);
        }
        else if(strcmp(msg, ron) == 0){
            previousMillis = millis() - 800; //let the light trigger immediately
            b_left = false; //turn off the other
            digitalWrite(l_stats, HIGH);
            Serial.println("Right on!");
            b_right = true;
            delay(500);
            digitalWrite(l_stats, LOW);
        }
        else if(strcmp(msg, foff) == 0){
            digitalWrite(l_stats, HIGH);
            Serial.println("Flash off!");
            digitalWrite(r_right, LOW);
            digitalWrite(r_right, LOW);
            b_right = false;
            b_left = false;
            delay(500);
            digitalWrite(l_stats, LOW);
        }
        else if(strcmp(msg, dual) == 0){
            previousMillis = millis() - 800; //let the light trigger immediately
            Serial.println("Dual Flashing!");
            b_left = true;
            b_right = true;
            delay(500);
            digitalWrite(l_stats, LOW);
        }
        else{
            Serial.println("ERROR!");
            digitalWrite(l_stats, HIGH);
        }
    }
}
