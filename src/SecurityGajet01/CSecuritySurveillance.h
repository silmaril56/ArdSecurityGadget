///////////////////////////////////////////////////////////
//  CSecuritySurveillance.h
//  Implementation of the Class CSecuritySurveillance
//  Created on:      2018/11/19
//  Original author: JJ1JQZ
///////////////////////////////////////////////////////////

#if !defined(__CSECURITYSURVEILLANCE_H__)
#define __CSECURITYSURVEILLANCE_H__

#include "Definitions.h"
#include "CObject.h"
#include "ITask.h"
#include "vl53l1x_class.h"
#include "CCircularBuffer.h"
#include "HardwareSerial.h"


/**
 * センサーからの情報を基にセキュリティ情報を生成する
 * また、センサーの設定を行う。
 */
class CSecuritySurveillance : public CObject, public ITask
{
public:
  //=========================================================
  //  メソッド
  //=========================================================
  /**
  * コンストラクタ。
  */
  CSecuritySurveillance(HardwareSerial *serial, VL53L1X * lider1_inst, VL53L1X * lider2_inst){
    _serial = serial;
    lider1  = lider1_inst;
    lider2  = lider2_inst;
    invoke_counter = 0;
    sensor1_error = VL53L1_ERROR_NONE;
    sensor2_error = VL53L1_ERROR_NONE;
    lider1_lp_filter = 0;
    lider2_lp_filter = 0;
    //signal_buffer                = new CCircularBuffer();
  }
  /**
   * デストラクタ。
   */
  virtual ~CSecuritySurveillance(){}
  /**
   * タスクの起動周期を取得する(最小起動周期で割った値)。
   */
  virtual TInvokePeriod get_invoke_period(){ return c_invoked_period; }
  /**
   * タスクを起動する。
   * ここではLiderの値を監視する
   */
  virtual void invoke();
  /**
   * 主要処理前に行う前処理
   */
  void pre_proc();
  /**
   * タスクの主処理終了時処理
   */
  void post_proc();
  /**
   * センサーセットアップ
   */
  bool setup();
  /**
   * センサーによる距離取得
   */
  void sensing();
  /**
   * センサーから取得した情報の解析
   */
  void analyze_sensors();
  /**
   * センサーのエラーコード取得
   */
  VL53L1X_ERROR get_sensor_error_code(EVSensorId sensor_id){
    VL53L1X_ERROR code;

    switch(sensor_id){
      case EVSensorId::EVSensor1:
        code = sensor1_error;
        break;
      case EVSensorId::EVSensor2:
        code = sensor2_error;
        break;
      default:
        code = sensor1_error;
        break;
    }

    return code;
  }
  /**
   * センサーの値取得時間取得
   */
  uint32_t get_sensing_period(){ return sensing_consuming_period; }
  /**
   * センサーの値取得時間取得
   */
  uint32_t get_value(EVSensorValueId sensor_val_id){
    uint32_t value;

    switch(sensor_val_id){
      case EVSensorValueId::EVReady:
        value = ((lider1_u.is_full() == true) && (lider2_u.is_full() == true)) == true ? 1 : 0;
        break;
      case EVSensorValueId::EVSensor1DistMm:
        value = lider1_lp_filter;
        break;
      case EVSensorValueId::EVSensor2DistMm:
        value = lider2_lp_filter;
        break;
      case EVSensorValueId::EVSensor1DistRawMm:
        value = lider1_u.newest();
        break;
      case EVSensorValueId::EVSensor2DistRawMm:
        value = lider2_u.newest();
        break;
      default:
        value = 0;
        break;
    }

    return value;
  }
  
protected:

protected:
  //=========================================================
  //  属性
  //=========================================================
  /**
   * タスクの起動回数カウンター。
   */
  long invoke_counter;
  
  /**
   * Lider1インスタンス
   */
  VL53L1X * lider1;
  /**
   * Lider2インスタンス
   */
  VL53L1X * lider2;
  /**
   * Lider1 sensor error code
   */
  VL53L1X_ERROR sensor1_error;
  /**
   * Lider2 sensor error code
   */
  VL53L1X_ERROR sensor2_error;
  /**
   * センサー応答時間
   */
  uint32_t sensing_consuming_period;
  /**
   * Lider1センサー値配列
   */
  CCircularBuffer lider1_u;
  /**
   * Lider2センサー値配列
   */
  CCircularBuffer lider2_u;
  /**
   * Lider1センサー値(周波数応答の低い物の値)
   */
  uint32_t lider1_lp_filter;
  /**
   * Lider2センサー値(周波数応答の低い物の値)
   */
  uint32_t lider2_lp_filter;
  /**
   * Liderセンサーより送信された距離値バッファ
   */
  //CCircularBuffer * signal_buffer;
  /**
   * デバッグ用出力
   */
  HardwareSerial * _serial;
protected:
  /**
   * タスクの起動周期。
   */
  static const TInvokePeriod c_invoked_period;
};
#endif // !defined(__CSECURITYSURVEILLANCE_H__)
