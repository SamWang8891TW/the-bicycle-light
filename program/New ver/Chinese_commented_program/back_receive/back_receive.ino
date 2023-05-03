#define flashing_cycle 800        //定義方向燈及警示燈閃爍間隔（微秒）
#define enable_serial_debug true  //是否啟序列除錯


#define CE 7        //2.4G 無線模組的 CE 腳位
#define CSN 8       //2.4G 無線模組的 CSN 腳位
#define r_mid A3    //大燈(中間)繼電器腳位 A0
#define r_left A2  //左邊方向燈繼電器腳位 A2
#define r_right A1  //右邊方向燈繼電器腳位 A1
#define l_mid 5     //大燈指示led燈腳位
#define l_left 6    //左轉指示led燈腳位
#define l_right 9   //右轉指示led燈腳位
#define l_state 1000
#define bt_main 14  //大燈手動開啟的按鍵腳位


#include <SPI.h>
#include "RF24.h"  //RF24.h by nRF24 "https://github.com/nRF24/RF24"
#include <math.h>



bool b_mid = false;                     //大燈按鈕是否觸發的布林函數
bool b_left = false;                    //左轉燈按鈕是否觸發的布林函數
bool b_flash = false;                   //左右燈是否觸發閃爍程式的布林函數
bool b_right = false;                   //右轉燈按鈕是否觸發的布林函數
bool b_leftstats = false;               //是否正在使用左轉燈的布林函數
bool b_rightstats = false;              //是否正在使用左轉燈的布林函數
bool b_midorgstate = false;             //大燈原本狀態
unsigned long flashpreviousMillis = 0;  //用以儲存上一次閃爍的時間
unsigned long midpreviousMillis, offpreviousMillis, dualpreviousMillis, leftpreviousMillis, rightpreviousMillis = 0;



//2.4G 無限模組
RF24 rf24(CE, CSN);           //定義2.4G無線模組的腳位
const byte addr[] = "1Node";  //定義2.4G無限模組的位置
const byte pipe = 1;          //指定通道編號


/*
定義接收訊號
a --> 大燈開啟
b --> 大燈關閉
c --> 左轉燈開啟
d --> 右轉燈開啟
e --> 方向燈及警示燈關閉
f --> 警示燈開啟
*/
char on[32] = "a";
char off[32] = "b";
char lon[32] = "c";
char ron[32] = "d";
char foff[32] = "e";
char dual[32] = "f";




void setup() {
  //啟用序列監控
  if (enable_serial_debug)
    Serial.begin(9600);

  rf24.begin();                      //初始化2.4G無線模組
  rf24.setChannel(83);               //設定頻道編號
  rf24.setPALevel(RF24_PA_MIN);      //設定功率
  rf24.setDataRate(RF24_1MBPS);      //設定傳輸速率
  rf24.openReadingPipe(pipe, addr);  //開啟通道和位址
  rf24.startListening();             //開始監聽無線廣播
  Serial.print("Radio initialized! Receiving!\n");
  Serial.print("CH83, 1MBPS, pipe1, addr=1Node\n");

  //設定腳位狀態
  pinMode(r_mid, OUTPUT);
  pinMode(r_left, OUTPUT);
  pinMode(r_right, OUTPUT);
  pinMode(l_state, OUTPUT);
  pinMode(l_mid, OUTPUT);
  pinMode(l_left, OUTPUT);
  pinMode(l_right, OUTPUT);
  pinMode(bt_main, INPUT);

  //讓所有燈都亮過一遍檢查錯誤
  Serial.println("Testing lights...");
  digitalWrite(r_left, HIGH);
  digitalWrite(l_left, HIGH);
  delay(1000);
  digitalWrite(r_left, LOW);
  digitalWrite(l_left, LOW);
  delay(1000);
  digitalWrite(r_mid, HIGH);
  digitalWrite(l_mid, HIGH);
  delay(1000);
  digitalWrite(r_mid, LOW);
  digitalWrite(l_mid, LOW);
  delay(1000);
  digitalWrite(r_right, HIGH);
  digitalWrite(l_right, HIGH);
  delay(1000);
  digitalWrite(r_right, LOW);
  digitalWrite(l_right, LOW);

  Serial.print("Radio initialized! Receiving!");
  Serial.print("CH83, 1MBPS, pipe1, addr = 1Node");
  Serial.println("Starting loop:");
}

void loop() {
  //**********************
  //****PART1 寫入繼電器****
  //**********************


  //將大燈的布林函數狀態寫入繼電器及指示燈
  digitalWrite(r_mid, b_mid);
  digitalWrite(l_mid, b_mid);

  while (b_flash) {  //讓方向燈及警示燈閃爍
    unsigned long currentMillis = millis();
    if (currentMillis - flashpreviousMillis >= flashing_cycle) {
      flashpreviousMillis = currentMillis;

      if (b_right == true && b_rightstats) {
        b_right = false;
        digitalWrite(r_right, LOW);
        digitalWrite(l_right, LOW);
      } else if (b_right == false && b_rightstats) {
        b_right = true;
        //Serial.println("right_tick");
        digitalWrite(r_right, HIGH);
        digitalWrite(l_right, HIGH);
      }

      if (b_left == true && b_leftstats) {
        b_left = false;
        digitalWrite(r_left, LOW);
        digitalWrite(l_left, LOW);
      } else if (b_left == false && b_leftstats) {
        b_left = true;
        //Serial.println("left_tick");
        digitalWrite(r_left, HIGH);
        digitalWrite(l_left, HIGH);
      }
    }
    break;
  }




  //**********************
  //*****PART2 偵測信號*****
  //**********************


  //偵測按鈕是否按下並針對動作作動
  if (digitalRead(bt_main) == true) {
    b_mid = !b_mid;
    b_left = false;
    b_leftstats = false;
    b_rightstats = false;
    b_right = false;
    b_flash = false;
    b_midorgstate = false;
    Serial.println("Button pressed, turning signal light off and change mid light stats.");
  }

  //2.4G無線模組接收訊號
  if (rf24.available(&pipe)) {
    char msg[32] = "";
    rf24.read(&msg, sizeof(msg));  //將獨到的訊息存入msg

    Serial.println(msg);

    if (strcmp(msg, on) == 0) {  //判斷接收到的訊息是否為開大燈
      Serial.println("Mid on!");
      if (b_flash) {
        unsigned long currentMillis = millis();
        if (currentMillis - midpreviousMillis >= 2000) {
          //Serial.println(midpreviousMillis);
          //Serial.println(currentMillis);
          b_midorgstate = !b_midorgstate;
          Serial.print("org state to:");
          Serial.println(b_midorgstate);
          //Serial.println(currentMillis);
          midpreviousMillis = currentMillis;
        }
      } else
        b_mid = true;


    } else if (strcmp(msg, off) == 0) {  //判斷接收到的訊息是否為關大燈
      Serial.println("Mid off!");
      if (b_flash)
        b_midorgstate = false;
      else
        b_mid = false;

    } else if (strcmp(msg, lon) == 0) {  //判斷接收到的訊息是否為開啟左邊方向燈
      Serial.println("Left on!");

      flashpreviousMillis = millis() - 100000;  //讓閃爍程式馬上觸發

      unsigned long currentMillis = millis();
      if (currentMillis - rightpreviousMillis > 2000) {
        if (b_mid == false && b_rightstats == false) {  //如果大燈未開啟及且另一邊的方向燈未啟用，則先開啟大燈再開啟方向燈以便後車辨認方向燈方向
          b_midorgstate = false;                        //將原本大燈的狀態儲存，以便之後關閉方向燈之後還原大燈狀態
          b_mid = true;
        } else if (b_mid && b_rightstats == false) {
          b_midorgstate = true;
        }
        rightpreviousMillis = currentMillis;
      }
      b_right = false;  //關閉另一邊的方向燈
      b_rightstats = false;
      b_flash = true;
      b_left = true;
      b_leftstats = true;


    } else if (strcmp(msg, ron) == 0) {  //判斷接收到的訊息是否為開啟右邊方向燈
      Serial.println("Right on!");
      flashpreviousMillis = millis() - 100000;  //讓閃爍程式馬上觸發

      unsigned long currentMillis = millis();
      if (currentMillis - leftpreviousMillis > 2000) {
        if (b_mid == false && b_leftstats == false) {  //如果大燈未開啟及另一邊的方向燈未啟用，則先開啟大燈再開啟方向燈以便後車辨認方向燈方向
          b_midorgstate = false;                       //將原本大燈的狀態儲存，以便之後關閉方向燈之後還原大燈狀態
          b_mid = true;
        } else if (b_mid && b_leftstats == false) {
          b_midorgstate = false;
        }
        leftpreviousMillis = currentMillis;
      }
      b_left = false;  //關閉另一邊的方向燈
      b_leftstats = false;
      b_flash = true;
      b_right = false;
      b_rightstats = true;


    } else if (strcmp(msg, foff) == 0) {  //判斷接收到的訊息是否關閉方向燈或是警示燈
      Serial.println("Flash off!");
      digitalWrite(r_right, LOW);
      digitalWrite(l_right, LOW);
      digitalWrite(r_left, LOW);
      digitalWrite(l_left, LOW);

      unsigned long currentMillis = millis();
      if (currentMillis - offpreviousMillis > 2000) {
        if (b_midorgstate) {
          Serial.println("org state 1 --> b_mid to true");
          b_mid = true;
        } else {
          Serial.println("run");
          b_mid = false;
        }
        offpreviousMillis = currentMillis;
      }
      b_midorgstate = false;
      b_flash = false;
      b_right = false;
      b_left = false;
      b_rightstats = false;
      b_leftstats = false;


    } else if (strcmp(msg, dual) == 0) {  //判斷接收到的訊息是開啟警示燈
      Serial.println("Dual Flash!");

      flashpreviousMillis = millis() - 100000;  //讓閃爍程式馬上觸發

      unsigned long currentMillis = millis();
      if (currentMillis - dualpreviousMillis > 2000) {
        if (b_mid == false && b_left == false && b_right == false) {  //如果大燈未開啟及方向燈未啟用，則先開啟大燈再開啟方向燈以便後車辨認方向燈方向
          b_midorgstate = false;                                      //將原本大燈的狀態儲存，以便之後關閉方向燈之後還原大燈狀態
          b_mid = true;
        } else if (b_mid && b_left == false && b_right == false) {
          b_midorgstate = true;
        }
        dualpreviousMillis = currentMillis;
      }
      b_left = true;
      b_right = true;
      b_flash = true;
      b_rightstats = true;
      b_leftstats = true;


    } else {
      Serial.println("Ignore!");
    }
  }
}
