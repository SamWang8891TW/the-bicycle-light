#define enable_serial_debug true  //是否開啟序列除錯

#define CE 7        //2.4G 無線模組的 CE 腳位
#define CSN 8       //2.4G 無線模組的 CSN 腳位
#define r_mid A1    //大燈(中間)繼電器腳位
#define l_mid A2    //大燈指示led燈腳位
#define bt_main A0  //大燈手動開啟的按鍵腳位


#include <SPI.h>
#include "RF24.h" //RF24.h by nRF24 "https://github.com/nRF24/RF24"



bool b_mid = false;  //大燈按鈕是否觸發的布林函數



//2.4G 無限模組
RF24 rf24(CE, CSN);           //定義2.4G無線模組的腳位
const byte addr[] = "1Node";  //定義2.4G無限模組的位置
const byte pipe = 1;          //指定通道編號


/*
定義接收訊號
a --> on
b --> off
*/
char on[32] = "a";
char off[32] = "b";




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



  //設定腳位狀態
  pinMode(r_mid, OUTPUT);
  pinMode(l_mid, OUTPUT);
  pinMode(bt_main, INPUT);

  //讓所有燈都亮過一遍檢查錯誤
  Serial.println("Testing lights...");
  digitalWrite(r_mid, HIGH);
  digitalWrite(l_mid, HIGH);
  delay(2000);
  digitalWrite(r_mid, LOW);
  digitalWrite(l_mid, LOW);

  Serial.println("Radio initialized! Receiving!");
  Serial.println("CH83, 1MBPS, pipe1, addr = 1Node");
  Serial.println("Starting loop:");
}

void loop() {
  //**********************
  //****PART1 寫入繼電器****
  //**********************


  //將大燈的布林函數狀態寫入繼電器及指示燈
  digitalWrite(r_mid, b_mid);
  digitalWrite(l_mid, b_mid);




  //**********************
  //*****PART2 偵測信號*****
  //**********************


  //偵測按鈕是否按下並針對動作作動
  if (digitalRead(bt_main) == true) {
    delay(200);
    if (digitalRead(bt_main) == true) {
      b_mid = !b_mid;
      Serial.println("Button pressed, turning signal light off and change mid light status.");

    }
  }

  //2.4G無線模組接收訊號
  if (rf24.available(&pipe)) {
    char msg[32] = "";
    rf24.read(&msg, sizeof(msg));  //將獨到的訊息存入msg

    Serial.println(msg);

    if (strcmp(msg, on) == 0) {  //判斷接收到的訊息是否為開大燈
      Serial.println("Mid on!");
      b_mid = true;


    } else if (strcmp(msg, off) == 0) { //判斷接收到的訊息是否為開大燈
      Serial.println("Mid off!");
      b_mid = false;


    } else {
      Serial.println("Ignore!");
    }
  }
}
