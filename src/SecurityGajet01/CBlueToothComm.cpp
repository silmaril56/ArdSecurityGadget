
///////////////////////////////////////////////////////////
//  CBlueToothComm.cpp
//  Implementation of the Class CBlueToothComm
//    Execute in less than the 1/2 period of the gadget sensing period.
//  Created on:      2018/09/29
//  Original author: gen
///////////////////////////////////////////////////////////

#include "CBlueToothComm.h"

#include "Arduino.h"


const TInvokePeriod CBlueToothComm::c_invoked_period = 20;//20 x 25[ms] = 0.5[sec]



/**
 * タスクを起動する。
 * ここでは送信、そして受信を行う。
 */
void CBlueToothComm::invoke(){
  //[Transfering process]
  transmit();
  
  //[Receiving process]
  recieve();
  
}

/**
 * データを送信する
 */
void CBlueToothComm::transmit(){
  if(_seq_surve->get_value(EVSensorValueId::EVReady) >= 1){
    get_serial()->print( "S1LP:" );
    get_serial()->print( _seq_surve->get_value(EVSensorValueId::EVSensor1DistMm) );
    get_serial()->print( "\tS2LP:" );
    get_serial()->print( _seq_surve->get_value(EVSensorValueId::EVSensor2DistMm) );
    get_serial()->print( "\tS1RW:" );
    get_serial()->print( _seq_surve->get_value(EVSensorValueId::EVSensor1DistRawMm) );
    get_serial()->print( "\tS2RW:" );
    get_serial()->print( _seq_surve->get_value(EVSensorValueId::EVSensor2DistRawMm) );
    get_serial()->print( "\n" );
    
    //transfer data via bluetooth device
    _bt_serial->write( (char *)0xFFFF, 2 );
    _bt_serial->write( EVSensorValueId::EVSensor1DistMm );
    _bt_serial->write( (char *)_seq_surve->get_value(EVSensorValueId::EVSensor1DistMm), 4 );
    _bt_serial->write( EVSensorValueId::EVSensor1DistRawMm );
    _bt_serial->write( (char *)_seq_surve->get_value(EVSensorValueId::EVSensor1DistRawMm), 4 );
    _bt_serial->write( EVSensorValueId::EVSensor2DistMm );
    _bt_serial->write( (char *)_seq_surve->get_value(EVSensorValueId::EVSensor2DistMm), 4 );
    _bt_serial->write( EVSensorValueId::EVSensor2DistRawMm );
    _bt_serial->write( (char *)_seq_surve->get_value(EVSensorValueId::EVSensor2DistRawMm), 4 );
    _bt_serial->write( EVSensorValueId::EVSensorErrorCode );
    _bt_serial->write( (char *)_seq_surve->get_value(EVSensorValueId::EVSensorErrorCode), 4 );
    //_bt_serial->write( EVSensorValueId::EVReady );
    //_bt_serial->write( (char *)0xFFFFFFFF, 4 );
  }
}

/**
 * データを受信する
 */
void CBlueToothComm::recieve(){
  //  char recv_data = 0;
  //  recv_data      = get_serial()->read();
}
