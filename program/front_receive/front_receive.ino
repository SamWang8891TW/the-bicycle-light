
#define CE 7  //radio CE pin
#define CSN 8 //radio CSN pin
#define r_mid  A1 //relay_middle
#define l_stats A3//led_stats (flash twice when ready, every radio receive flash once)
#define l_mid  A2//led_mid (indicate)
#define bt_main A0//main light

#include <SPI.h>
#include "RF24.h"

//radio211
RF24 rf24(CE, CSN);
const byte addr[] = "1Node";
const byte pipe = 1;  // 指定通道編號

bool b_mid = false; //bool for mid

/*
Define Radio.
g --> mid on
h --> mid off
*/
char on[32] = "g";
char off[32] = "h";


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
    //pinMode(l_stats, OUTPUT);
    pinMode(l_mid, OUTPUT);
    pinMode(bt_main, INPUT);
    Serial.println("Pin initialized!");
    Serial.println("Starting loop:");
    digitalWrite(l_stats, HIGH);
    delay(1000);
    digitalWrite(l_stats, LOW);
    Serial.println("Starting loop!");
}

void loop() {
  //Serial.println("looped");
  //write bool status to relays and indicate lights
  digitalWrite(r_mid, b_mid);

  //detect button status and react to it
  if(digitalRead(bt_main) == true){
    delay(200);
    if(digitalRead(bt_main) == true){
      b_mid = !b_mid;
      Serial.println("Button pressed, turning signal light off and change mid light status.");
      delay(500);
    }
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
    if(strcmp(msg, on) == 0){
      digitalWrite(l_stats, HIGH);
      Serial.println("Mid on!");
      b_mid = true;
      delay(500);
      digitalWrite(l_stats, LOW);
    }
    else if(strcmp(msg, off) == 0){
      digitalWrite(l_stats, HIGH);
      Serial.println("Mid off!");
      b_mid = false;
      delay(500);
      digitalWrite(l_stats, LOW);
    }
    else{
      Serial.println("Ignore!");
      digitalWrite(l_stats, HIGH);
    }
  }
}
