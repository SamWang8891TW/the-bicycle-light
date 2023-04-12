#define bzsw true                 //是否啟用蜂鳴器及震動
#define enable_serial_debug true  //是否啟序列除錯


#define CE 7        //2.4G 無線模組的 CE 腳位
#define CSN 8       //2.4G 無線模組的 CSN 腳位
#define l_mid A0    //大燈指示led燈腳位
#define l_left A2   //左轉方向燈指示led燈腳位
#define l_right A3  //右轉方向燈指示led燈腳位
#define l_stats A4  //狀態指示led燈腳位
#define bt_mid 5    //大燈(中間)按鈕腳位
#define bt_left 4   //左轉方向燈按鈕腳位
#define bt_right 6  //右轉方向燈按鈕腳位
#define bt_dual 3   //警示燈按鈕腳位
#define vmotor 9    //震動馬達繼電器腳位
#define bz 2        //蜂鳴器繼電器腳位


#include <SPI.h>
#include "RF24.h"  //RF24.h by nRF24 "https://github.com/nRF24/RF24"


bool b_mid = false;      //大燈按鈕是否觸發的布林函數
bool b_left = false;     //左轉燈按鈕是否觸發的布林函數
bool b_right = false;    //右轉燈按鈕是否觸發的布林函數
bool b_dual = false;     //警示燈是否觸發的布林函數
bool b_flash = false;    //決定左右轉方向燈動作的布林函數
bool b_mstats = false;   //是否正在使用大燈燈的布林函數
bool b_lstats = false;   //是否正在使用左轉燈的布林函數
bool b_rstats = false;   //是否正在使用右轉燈的布林函數
bool b_dfstats = false;  //是否正在使用警示燈的布林函數



//2.4G 無限模組
RF24 rf24(CE, CSN);           //定義2.4G無線模組的腳位
const byte addr[] = "1Node";  //定義2.4G無限模組的位置


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
  Serial.begin(9600);

  rf24.begin();                  //初始化2.4G無線模組
  rf24.setChannel(83);           //設定頻道編號
  rf24.openWritingPipe(addr);    //設定通道位址
  rf24.setPALevel(RF24_PA_MIN);  //設定功率
  rf24.setDataRate(RF24_1MBPS);  //設定傳輸速率
  rf24.stopListening();          //停止偵聽-->設定成發射模式

  //設定腳位狀態
  pinMode(bt_mid, INPUT);
  pinMode(bt_left, INPUT);
  pinMode(bt_right, INPUT);
  pinMode(bt_dual, INPUT);
  pinMode(l_mid, OUTPUT);
  pinMode(l_left, OUTPUT);
  pinMode(l_right, OUTPUT);
  pinMode(l_stats, OUTPUT);
  pinMode(bz, OUTPUT);
  pinMode(vmotor, OUTPUT);

  //讓所有燈都亮過一遍檢查錯誤
  Serial.println("Testing lights...");
  digitalWrite(l_mid, HIGH);
  delay(1000);
  digitalWrite(l_mid, LOW);
  delay(1000);
  digitalWrite(l_left, HIGH);
  delay(1000);
  digitalWrite(l_left, LOW);
  delay(1000);
  digitalWrite(l_right, HIGH);
  delay(1000);
  digitalWrite(l_right, LOW);
  delay(1000);
  digitalWrite(l_stats, HIGH);
  delay(1000);
  digitalWrite(l_stats, LOW);


  beep();
  Serial.println("Radio initialized! Sending!");
  Serial.println("CH83, 1MBPS, addr = 1Node");
  Serial.println("Starting loop!");
}

void loop() {
  //**********************
  //****PART1 偵測按鈕*****
  //**********************
  if (digitalRead(bt_mid) == true) { //判斷開關大燈的按鈕是否按下
    delay(200);
    if (digitalRead(bt_mid) == true)
      Serial.println("Main trig");
    b_mid = true;
    beep();

  } else if (digitalRead(bt_left) == true) { //判斷開關左轉方向燈的按鈕是否按下
    delay(200);
    if (digitalRead(bt_left) == true)
      Serial.println("Left trig");
    b_left = true;
    beep();

  } else if (digitalRead(bt_right) == true) { //判斷開關右轉方向燈的按鈕是否按下
    delay(200);
    if (digitalRead(bt_right) == true)
      Serial.println("Right trig");
    b_right = true;
    beep();

  } else if (digitalRead(bt_dual) == true) { //判斷開關警示燈的按鈕是否按下
    delay(200);
    Serial.println("Dual trig");
    b_dual = true;
    beep();

  }


  //**********************
  //****PART2 發送訊號*****
  //**********************
  if (b_mid == true) { //傳送開關大燈訊息
    b_mid = false;
    Serial.print("Main send sig");
    b_mstats = !b_mstats;

    if (b_mstats) {
      Serial.println("mid on");
      rf24.write(&on, sizeof(on));
      rf24.write(&on, sizeof(on));
      rf24.write(&on, sizeof(on));
      digitalWrite(l_mid, HIGH);
    } else {
      Serial.println("mid off");
      rf24.write(&off, sizeof(off));
      rf24.write(&off, sizeof(off));
      rf24.write(&off, sizeof(off));
      digitalWrite(l_mid, LOW);
    }

  } else if (b_left == true) { //傳送開關左轉方向燈訊息
    Serial.println("l_triggered");
    b_left = false;
    b_dfstats = false;

    if (b_rstats) {
      b_rstats = false;
      b_lstats = true;
      b_flash = true;
    } else if (b_lstats) {
      b_lstats = false;
      b_flash = false;
    } else {
      b_lstats = true;
      b_flash = true;
    }

    if (b_flash) {
      Serial.println("lon");
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&lon, sizeof(lon));
      rf24.write(&lon, sizeof(lon));
      rf24.write(&lon, sizeof(lon));
      digitalWrite(l_left, HIGH);
    } else {
      Serial.println("flash off");
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      digitalWrite(l_left, LOW);
      digitalWrite(l_right, LOW);
    }

  } else if (b_right == true) { //傳送開關右轉方向燈訊息
    Serial.println("r_triggered");

    b_right = false;
    b_dfstats = false;

    if (b_lstats) {
      b_lstats = false;
      b_rstats = true;
      b_flash = true;
    } else if (b_rstats) {
      b_rstats = false;
      b_flash = false;
    } else {
      b_rstats = true;
      b_flash = true;
    }


    if (b_flash) {
      Serial.println("ron");
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&ron, sizeof(ron));
      rf24.write(&ron, sizeof(ron));
      rf24.write(&ron, sizeof(ron));
      digitalWrite(l_right, HIGH);
    } else {
      Serial.println("flash off");
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      digitalWrite(l_left, LOW);
      digitalWrite(l_right, LOW);
    }

  } else if (b_dual) { //傳送開關警示燈訊息
    Serial.println("d_triggered");
    b_dual = false;
    b_lstats = false;
    b_rstats = false;
    if (b_dfstats) {
      Serial.println("flash off");
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      digitalWrite(l_left, LOW);
      digitalWrite(l_right, LOW);
    } else {
      Serial.println("dual on");
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&foff, sizeof(foff));
      rf24.write(&dual, sizeof(dual));
      rf24.write(&dual, sizeof(dual));
      rf24.write(&dual, sizeof(dual));
      digitalWrite(l_left, HIGH);
      digitalWrite(l_right, HIGH);
    }
    b_dfstats = !b_dfstats;

  }
}


void beep() { //用蜂鳴器及震動馬達及狀態燈告知使用者
  digitalWrite(l_stats, HIGH);
  if (bzsw == true)
    digitalWrite(bz, HIGH);
  digitalWrite(vmotor, HIGH);
  delay(100);
  digitalWrite(bz, LOW);
  delay(100);
  digitalWrite(vmotor, LOW);
  digitalWrite(l_stats, LOW);
}
