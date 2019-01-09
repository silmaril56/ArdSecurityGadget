///////////////////////////////////////////////////////////
//  CBlueToothComm.h
//  Implementation of the Class CBlueToothComm
//  Created on:      2018/09/19
//  Original author: gen
///////////////////////////////////////////////////////////

#if !defined(__CBLUETOOTHCOMM_H__)
#define __CBLUETOOTHCOMM_H__

#include "Definitions.h"
#include "CObject.h"
#include "ITask.h"
#include "CSecuritySurveillance.h"
#include "HardWareserial.h"
#include "SoftwareSerial.h"

/**
 * BlueToothにて送受信を行うクラス。
 */
class CBlueToothComm : public CObject, public ITask
{
public:
  //=========================================================
  //  メソッド
  //=========================================================
  /**
  * コンストラクタ。
  */
  CBlueToothComm(HardwareSerial *serial, SoftwareSerial *bt_serial, CSecuritySurveillance *seq_surve){
    _serial = serial;
    _bt_serial = bt_serial;
    _seq_surve  = seq_surve;
  }
  /**
   * デストラクタ。
   */
  virtual ~CBlueToothComm(){}
  /**
   * タスクの起動周期を取得する(最小起動周期で割った値)。
   */
  virtual TInvokePeriod get_invoke_period(){ return c_invoked_period; }
  /**
   * タスクを起動する。
   * ここでは計測角、指示角をdata_strobeで集めて、actuate()する。
   */
  virtual void invoke();
  /**
   * データを送信する
   */
  void transmit();
  /**
   * データを受信する
   */
  void recieve();
  
protected:
  HardwareSerial * get_serial(){ return _serial; }
protected:
  //=========================================================
  //  属性
  //=========================================================

protected:
  /**
   * SPI通信用クラスインスタンス。
   */
  HardwareSerial * _serial;
  /**
   * UART通信用クラスインスタンス。
   */
  SoftwareSerial * _bt_serial;
  //SoftwareSerial _serial(6, 7);
  /**
   * セキュリティー監視クラスインスタンス。
   */
  CSecuritySurveillance * _seq_surve;
  /**
   * タスクの起動周期。
   */
  static const TInvokePeriod c_invoked_period;
};
#endif // !defined(__CBLUETOOTHCOMM_H__)
