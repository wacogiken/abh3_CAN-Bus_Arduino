/******************************************************************************
   Software License Agreement (BSD License)

    Copyright (c) 2021, Waco Giken Co., Ltd.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

 *   * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/o2r other materials provided
       with the distribution.
 *   * Neither the name of the Waco Giken nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
//
// ABH3CAN HOST サンプルソフト
//        2023.1.25　Ishikawa
//

#include <ABH3CAN.h>        //ABH3CANライブラリのインクルード
ABH3CAN  par;               //CAN通信構造体の定義

#define START_A  4          //AY軸　サーボON & スタートSW
#define START_B  5          //BX軸　サーボON & スタートSW
#define VOL_A    0          //A軸速度指令アナログ入力
#define VOL_B    1          //B軸速度指令アナログ入力

//初期化設定ルーチン
void setup() {
  Serial.begin(115200);
  pinMode(START_A, INPUT_PULLUP);     //AY軸　サーボON & スタートSW　入力設定
  pinMode(START_B, INPUT_PULLUP);     //BX軸　サーボON & スタートSW　入力設定

  for (;;) {
    if (can_init(1)) break;           //CAN SHIELDの初期化(引数 0:250kbps 1:500kbps 2:1000kbps)
  }
  can_setadrs(1, 2, &par);            //CANアドレスの設定　ABH3アドレス=1，HOSTアドレス=2
  abh3_can_init(&par);                //ABH3の指令の初期化
}

//メインルーチン
void loop() {
  float a, b;
  short com_a, com_b;
  float fbk_a, fbk_b;

  if (!digitalRead(START_A)) {              //START_Aスイッチ：0N
    abh3_can_inBitSet( 0, 1, &par);         //ABH3 A軸　サーボON
    abh3_can_inBitSet (1, 1, &par);         //ABH3 A軸　スタートON
  }
  else {                                    //START_Aスイッチ：0FF
    abh3_can_inBitSet( 0, 0, &par);         //ABH3 A軸　サーボOFF
    abh3_can_inBitSet( 1, 0, &par);         //ABH3 A軸　スタートOFF
  }

  if (!digitalRead(START_B)) {              //START_Bスイッチ：0N
    abh3_can_inBitSet( 8, 1, &par);         //ABH3 B軸　サーボON
    abh3_can_inBitSet( 9, 1, &par);         //ABH3 B軸　スタートON
  }
  else {                                    //START_Bスイッチ：0FF
    abh3_can_inBitSet( 8, 0, &par);         //ABH3 B軸　サーボOFF
    abh3_can_inBitSet( 9, 0, &par);         //ABH3 B軸　スタートOFF
  }

  a = analogRead(VOL_A) / 1023.0 * 2000.0;  //アナログ入力の読み取り（A軸速度指令）
  com_a = cnvVel2CAN(a);                    //速度指令をCANデータ形式に変換
  abh3_can_cmdAY(com_a, &par);              //ABH3 A/Y軸速度指令設定

  b = analogRead(VOL_B) / 1023.0 * 2000.0;  //アナログ入力の読み取り（B軸速度指令）
  com_b = cnvVel2CAN(b);                    //速度指令をCANデータ形式に変換
  abh3_can_cmdBX(com_b, &par);              //ABH3 B/X軸速度指令設定


  abh3_can_reqBRD(0x2a, &par);             //ブロードキャスト2(0x2a)
  fbk_a = cnvCAN2Vel(par.BR2.io.ayvel_fb); //ABH3 A/Y軸速度[min^-1]帰還の取得
  fbk_b = cnvCAN2Vel(par.BR2.io.bxvel_fb); //ABH3 B/X軸速度[min^-1]帰還の取得

  Serial.print("FBK_VelAY:"); Serial.println(fbk_a);
  Serial.print("FBK_VelBX:"); Serial.println(fbk_b);

}
