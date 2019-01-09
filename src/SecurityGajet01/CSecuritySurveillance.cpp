
///////////////////////////////////////////////////////////
//  CSecuritySurveillance.cpp
//  Implementation of the Class CSecuritySurveillance
//  Created on:      2018/11/19
//  Original author: JJ1JQZ
///////////////////////////////////////////////////////////

#include "CSecuritySurveillance.h"

#include "Arduino.h"

const TInvokePeriod CSecuritySurveillance::c_invoked_period = 4;//4 x 25[ms] = 100[ms]

/**
 * タスクを起動する。
 * ここではセンサーの値を監視する
 */
void CSecuritySurveillance::invoke(){
  
  //_serial->print( millis() );
  //[pre process]
  pre_proc();
  
  //[sensing process]
  sensing();
  
  //[sensing process]
  analyze_sensors();
  
  //[post process]
  post_proc();
}

/**
 * センサーのセットアップを行う
 */
bool CSecuritySurveillance::setup(){
  
  //センサー1の初期化
  lider2->VL53L1_Off();
  sensor1_error = lider1->InitSensor(0x54);
  if(sensor1_error == VL53L1_ERROR_NONE)
  {
    _serial->println("Sensor1 online!");
  }
  
  //センサー2の初期化
  if(sensor1_error == VL53L1_ERROR_NONE){
    lider2->VL53L1_On();
    delay(10);
    sensor2_error = lider2->InitSensor(0x52);
    if(sensor2_error == VL53L1_ERROR_NONE)
    {
      _serial->println("Sensor2 online!");
    }
  }

  //センサー1のモード設定
  if(sensor1_error >= VL53L1_ERROR_NONE){
    sensor1_error = lider1->VL53L1X_SetDistanceMode(EVL53L1XDistanceMode::EVL53L1XLong);
    if(sensor1_error >= VL53L1_ERROR_NONE){
      _serial->println("Sensor1 mode setting!");
    }
  }

  //センサー2のモード設定
  if(sensor2_error >= VL53L1_ERROR_NONE){
    sensor2_error = lider2->VL53L1X_SetDistanceMode(EVL53L1XDistanceMode::EVL53L1XLong);
    if(sensor2_error >= VL53L1_ERROR_NONE){
      _serial->println("Sensor2 mode setting!");
    }
  }

  return ((sensor1_error >= VL53L1_ERROR_NONE) && (sensor2_error >= VL53L1_ERROR_NONE));
}

/**
 * 主要処理前に行う前処理
 */
void CSecuritySurveillance::pre_proc(){
}

/**
 * センサーによる距離取得
 */
void CSecuritySurveillance::sensing(){
  uint32_t distance1 = 0;
  uint32_t distance2 = 0;

  long startTime = millis();
  
  
  //Lider1 sensing
  //Poll for completion of measurement. Takes 40-50ms.
  lider1->VL53L1X_StartRanging(); //Write configuration bytes to initiate measurement
  lider2->VL53L1X_StartRanging(); //Write configuration bytes to initiate measurement
  
  //take a distance from the sensor
  lider1->GetDistance(&distance1);//Get the result of the measurement from the sensor
  lider2->GetDistance(&distance2);//Get the result of the measurement from the sensor
  
  //push the distance value to the value array
  lider1_u.push_and_pop(distance1);
  lider2_u.push_and_pop(distance2);

  _serial->print("SS1(mm): ");
  _serial->print(distance1);
  _serial->print("\tSS2(mm): ");
  _serial->print(distance2);
  _serial->print("\tSSCNT: ");
  _serial->print(lider1_u.count());
  _serial->print("\n");


  long endTime = millis();
  
  sensing_consuming_period = (endTime - startTime);
}

/**
 * センサーから取得した情報の解析
 */
void CSecuritySurveillance::analyze_sensors(){
  lider1_lp_filter = lider1_u.mean();
  lider2_lp_filter = lider2_u.mean();
}

/**
 * 主要処理後に行う後処理
 */
void CSecuritySurveillance::post_proc(){
}
