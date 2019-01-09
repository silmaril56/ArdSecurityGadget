///////////////////////////////////////////////////////////
//  Definitions.h
//  Defintions for global wide use.
//  Created on:      2018/11/11
//  Original author: JJ1JQZ
///////////////////////////////////////////////////////////


#if !defined(__DEFINITIONS_H__)
#define __DEFINITIONS_H__

#include <Arduino.h>

//=========================================================
//  列挙型
//=========================================================

/**
* クラスID。
*/
enum EClassID
{
  /**
  * CObjectクラスのクラスID
  */
  EVCObjectID = 0,
  /**
  * CTaskManagerクラスのクラスID
  */
  EVCTaskManagerID,
  /**
  * CBlueToothCommクラスのクラスID
  */
  EVCBlueToothCommID,
  /**
  * CSecuritySurveillanceクラスのクラスID
  */
  EVCSecuritySurveillanceID
};

/**
* VL53L1X distance mode
*/
enum EVL53L1XDistanceMode
{
  /**
   * 短距離
   */
  EVL53L1XShort = 0,
  /**
   * 中距離
   */
  EVL53L1XMid = 1,
  /**
   * 長距離
   */
  EVL53L1XLong = 2
};
/**
* VL53L1X distance mode
*/
enum EVSensorId
{
  /**
   * センサー1
   */
  EVSensor1 = 0,
  /**
   * センサー2
   */
  EVSensor2 = 1
};
/**
* VL53L1X distance mode
*/
enum EVSensorValueId
{
  /**
   * センサー2距離値RAW(mm)
   */
  EVReady = 0,
  /**
   * センサー1距離値(mm)
   */
  EVSensor1DistMm = 1,
  /**
   * センサー2距離値(mm)
   */
  EVSensor2DistMm = 2,
  /**
   * センサー1距離値RAW(mm)
   */
  EVSensor1DistRawMm = 3,
  /**
   * センサー2距離値RAW(mm)
   */
  EVSensor2DistRawMm = 4
};
//=========================================================
//  型定義
//=========================================================

/**
 * 基本のTask起動周期に対するTask起動周期を表す型。
 */
typedef uint8_t TInvokePeriod;

//=========================================================
//  定数定義
//=========================================================
//*******************************************
// Defines
//*******************************************
#define MAX_LONG 2147483647

//*******************************************
// Port Number Definition
//*******************************************
//--------------------
// INPUTS
//--------------------

//--------------------
// OUTPUTS
//--------------------
//Audio Port
const int LIDER1SHDN    = 2;
const int LIDER2SHDN    = 3;
const int LIDER1INTR    = 4;
const int LIDER2INTR    = 5;
//LED
const int ON_BOARD_LED  = 13;

//--------------------
// COMM
//--------------------
//UART
const int UART1_RX      = 6;
const int UART1_TX      = 7;


//*******************************************
// Constant Definitions
//*******************************************
/** Lider(VL53L1X) defaults I2C address(7bit+1bit of RW)
 */
const uint8_t  c_LiderI2CAddressDefault = 0x52;//0x52(in 8bit but Gr-sakura I2C library has to specify in 7bit which is 41)
/** Lider1 I2C address(7bit+1bit of RW)
 */
const uint8_t  c_Lider1I2CAddress       = 0x54;//0x54(in 8bit but Gr-sakura I2C library has to specify in 7bit which is 42)

/** Lider2 I2C address(7bit+1bit of RW)
 */
const uint8_t  c_Lider2I2CAddress       = 0x52;

/** Serial output wait time
 * Unit-> ms
 */
const uint32_t  c_SerialWait        = 10;

/** Software Serial boud rate
 */
const uint32_t  c_SSerialBoudRate       = 9600;

/** Hardware Serial boud rate
 */
const uint32_t  c_HSerialBoudRate       = 115200;//9600//115200

/** Max Time counter
 */
const uint32_t  c_MaxTimeCounter        = 1000;
/** Serial outpu wait time
 * Unit-> ms
 */
const uint32_t  c_MaxTaskCount          = 5;

/** Timer Interval
 * Unit-> ms
 */
#define  c_Timer_Interval               ( 25)//set as 25ms
/** Circular Buffer Size
 */
#define  c_CircularBufferSize           ( 16)
//*******************************************
// type Definitions
//*******************************************
typedef uint32_t TCBUF_D;

#endif // !defined(__DEFINITIONS_H__)

