//
// ABH3CAN HOST サンプルソフト 
//        2021.2.4　Ishikawa
//

#include <ABH3CAN.h>        //ABH3CANライブラリのインクルード
ABH3CAN  par;               //CAN通信構造体の定義


//初期化設定ルーチン
void setup() {
  Serial.begin(115200);
  for (;;) {
    if (can_init(1)) break;           //CAN SHIELDの初期化(引数 0:250kbps 1:500kbps 2:1000kbps)
  }
  can_setadrs(1, 2, &par);            //CANアドレスの設定　ABH3アドレス=1，HOSTアドレス=2
  abh3_can_init(&par);                //ABH3の指令の初期化
}

void get_status(ABH3CAN *par) {
  Serial.println("***********************************************************");
  Serial.println("                    Single Packet                          ");
  Serial.println("***********************************************************");
  Serial.print("VelA:"); Serial.println(cnvCAN2Vel(par->DP0R.io.avel_fb));     //ABH3 A軸速度[min^-1]の取得
  Serial.print("VelB:"); Serial.println(cnvCAN2Vel(par->DP0R.io.bvel_fb));     //ABH3 B軸速度[min^-1]の取得
  Serial.print("VelY:"); Serial.println(cnvCAN2Vel(par->DP0R.io.yvel_fb));     //ABH3 Y軸速度[min^-1]の取得
  Serial.print("VelX:"); Serial.println(cnvCAN2Vel(par->DP0R.io.xvel_fb));     //ABH3 X軸速度[min^-1]の取得
  abh3_can_reqPulse(par);                                                     //ABH3 AB軸積算パルスの取得命令発行
  Serial.print("PulseA:"); Serial.println(par->DP1R.io.apulse);                //ABH3 A軸積算パルスの取得
  Serial.print("PulseB:"); Serial.println(par->DP1R.io.bpulse);                //ABH3 B軸積算パルスの取得

  Serial.println("***********************************************************");
  Serial.println("                    Broad Cast Packet                      ");
  Serial.println("***********************************************************");
  abh3_can_reqBRD(0x28, par);                                                  //ブロードキャストパケット0(0x28)要求　*異常/警告フラグの取得
  Serial.print("Error:"); Serial.println(par->BR0.io.error_flg, HEX);          //ABH3 異常フラグの取得
  Serial.print("Warn:");  Serial.println(par->BR0.io.warning_flg, HEX);        //ABH3 警告フラグの取得
  abh3_can_reqBRD(0x29, par);                                                  //ブロードキャストパケット0(0x29)要求　*制御/ＩＯフラグの取得
  Serial.print("Ctrl:"); Serial.println(par->BR1.io.control_flg, HEX);        //ABH3 制御フラグの取得
  Serial.print("IO  :");  Serial.println(par->BR1.io.io_flg, HEX);             //ABH3 ＩＯフラグの取得
  abh3_can_reqBRD(0x2a, par);                                                  //ブロードキャストパケット2(0x2a)要求　*速度指令/帰還の取得
  Serial.print("COMVelAY:"); Serial.println(cnvCAN2Vel(par->BR2.io.ayvel_com));//ABH3 A/Y軸速度[min^-1]指令の取得
  Serial.print("COMVelBX:"); Serial.println(cnvCAN2Vel(par->BR2.io.bxvel_com));//ABH3 B/X軸速度[min^-1]指令の取得
  Serial.print("FBKVelAY:"); Serial.println(cnvCAN2Vel(par->BR2.io.ayvel_fb)); //ABH3 A/Y軸速度[min^-1]帰還の取得
  Serial.print("FBKVelBX:"); Serial.println(cnvCAN2Vel(par->BR2.io.bxvel_fb)); //ABH3 B/X軸速度[min^-1]帰還の取得
  abh3_can_reqBRD(0x2b, par);                                                    //ブロードキャストパケット3(0x2b)要求　*電流指令/負荷率の取得
  Serial.print("COMTrqAY:"); Serial.println(cnvCAN2Trq(par->BR3.io.aycrt_com));  //ABH3 A/Y軸電流[%]指令の取得
  Serial.print("COMTrqBX:"); Serial.println(cnvCAN2Trq(par->BR3.io.bxcrt_com));  //ABH3 B/X軸電流[%]指令の取得
  Serial.print("LoadA:");    Serial.println(cnvCAN2Load(par->BR3.io.aload));     //ABH3 A/Y軸負荷率[%]帰還の取得
  Serial.print("LoadB:");    Serial.println(cnvCAN2Load(par->BR3.io.bload));     //ABH3 B/X軸負荷率[%]帰還の取得
  abh3_can_reqBRD(0x2c, par);                                                    //ブロードキャストパケット4(0x2c)要求　*パルス積算値の取得
  Serial.print("PulseA:"); Serial.println(par->BR4.io.apulse_integ);             //ABH3 A軸積算パルスの取得
  Serial.print("PulseB:"); Serial.println(par->BR4.io.bpulse_integ);             //ABH3 B軸積算パルスの取得
  abh3_can_reqBRD(0x2d, par);                                                    //ブロードキャストパケット5(0x2d)要求　*アナログ入力/電源電圧の取得
  Serial.print("Analog0:"); Serial.println(cnvCAN2Analog(par->BR5.io.analog_in0));//ABH3 アナログ入力0電圧[V]の取得
  Serial.print("Analog1:"); Serial.println(cnvCAN2Analog(par->BR5.io.analog_in1));//ABH3 アナログ入力1電圧[V]の取得
  Serial.print("MainVolt:"); Serial.println(cnvCAN2Volt(par->BR5.io.main_volt));  //ABH3 主電源電圧[V]の取得
  Serial.print("CtrlVolt:"); Serial.println(cnvCAN2Volt(par->BR5.io.ctrl_volt));  //ABH3 制御電源電圧[V]の取得
  abh3_can_reqBRD(0x2e, par);                                                    //ブロードキャストパケット6(0x2e)要求　*モニタ電圧の取得
  Serial.print("Moni0:"); Serial.println(par->BR5.io.analog_in0);                //ABH3 アナログモニタ0電圧[V]の取得
  Serial.print("Moni1:"); Serial.println(par->BR5.io.analog_in1);                //ABH3 アナログモニタ1電圧[V]の取得

}

void loop() {
  float a = 1000.0;
  float b = -1000.0;

  abh3_can_inBitSet( 0, 1, &par);                                               //ABH3 A軸　サーボON
  abh3_can_inBitSet (1, 1, &par);                                               //ABH3 A軸　スタートON
  abh3_can_inBitSet(12, 1, &par);                                               //ABH3 B軸　サーボON
  abh3_can_inBitSet(13, 1, &par);                                               //ABH3 B軸　スタートON
  abh3_can_cmdAY(cnvVel2CAN(a), &par);                                    //ABH3 A/Y軸速度指令設定
  abh3_can_cmdBX(cnvVel2CAN(b), &par);                                    //ABH3 B/X軸速度指令設定

  for (int i = 0; i < 10; i++) {
    abh3_can_cmd(cnvVel2CAN(a), cnvVel2CAN(b), &par);               //ABH3 A/Y,B/X軸速度指令　同時設定
    get_status(&par);
    a -= 100.0;
    b += 100.0;
    delay(1000);
  }

  abh3_can_inSet(0x00000000, 0x00003003, &par);                                   //ABH3 AB軸サーボ同時OFF、スタート同時OFF　指令
  delay(3000);
}
