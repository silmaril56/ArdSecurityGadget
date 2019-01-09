#include <Wire.h>
#include <MsTimer2.h>
#include <Time.h>
#include "Definitions.h"
#include "CObject.h"
#include "CTaskManager.h"
#include "CBlueToothComm.h"
#include "vl53l1x_class.h"
#include "CSecuritySurveillance.h"

//////////////////////////////////////////////////////////////////
//  Gardening Support Equipment Control Software
// --------------------------------------------------------------
// Created Date: 11th, November, 2018
// Update Date:  11th, November, 2018
// Version: 0.1
// author: JJ1JQZ
// --------------------------------------------------------------
// Target HW: Arduino Duemilanove with some custom HW compornents.
// Library: DateTime Library
// Program Environment: Arduino IDE
// --------------------------------------------------------------
// Change History
// --------------------------------------------------------------
//               <Ver. 0.1 ( 11th, November, 2018 )>
//   1. New Creation with timer interruption model and current time & date setting
// --------------------------------------------
//////////////////////////////////////////////////////////////////

//*******************************************
// Grobal Value Definitions
//*******************************************
/** Current Moisture Value
 */
volatile boolean TaskInvokeEvent;

/** Task Counter
 */
int              TaskCounter;

/** Start time[ms] of this loop process
 */
long             StartTime;

/** End time[ms] of this loop process
 */
long             EndTime;

/** Processing period[ms] of this loop process
 */
long             ProcessTime;

/** Task manager class instance
 */
CTaskManager  TaskManager;


/** software serial over UART class
 */
SoftwareSerial BTSerial(UART1_RX, UART1_TX);

/** I2C Lider device1
 */
VL53L1X I2CLider1(&Wire, LIDER1SHDN, LIDER1INTR);

/** I2C Lider device2
 */
VL53L1X I2CLider2(&Wire, LIDER2SHDN, LIDER2INTR);

/** Security Surveillance class
 */
CSecuritySurveillance SecuritySurveillance(&Serial, &I2CLider1, &I2CLider2);

/** bluetooth communication class
 */
CBlueToothComm BlueToothComm( &Serial, &BTSerial, &SecuritySurveillance);
//*******************************************
// Function Definitions
//*******************************************

//******************************
// Utility functions
//******************************
/**
 * convert character to int
 */
//#define c2i ( c ) c - '0'
inline int c2i(char c){ return (c - '0'); }


//******************************
// Set up and loop
//******************************
/**
 * Initialization Functions
 */
void setup() {
  
  //---------------------
  //open serial port
  //---------------------
 // RN4020 power on wait
  //MsTimer2::stop();
  delay( 1000 );  // 1[s]

  // ボーレートを指定して通信開始
  BTSerial.begin( c_SSerialBoudRate );
  Serial.begin  ( c_HSerialBoudRate );//9600//115200
  //Wire1.setFrequency(400000);
  Wire.begin();
  
  //---------------------
  //digital IO setup
  //---------------------
  pinMode(LIDER1SHDN, OUTPUT);//OUTPUT_OPENDRAIN
  pinMode(LIDER2SHDN, OUTPUT);//OUTPUT_OPENDRAIN
  //digitalWrite(LIDER1SHDN, LOW);//HIGH
  //digitalWrite(LIDER2SHDN, LOW);//LOW enable
  delay(100);
  //---------------------
  
  //---------------------
  //sensor set up
  //---------------------
  Serial.print( "sensor SETUP begin:" );
  Serial.print( "\n" );
  bool result = SecuritySurveillance.setup();
  if(result != true){
    Serial.print( "sensor SETUP failed:\n" );
  }
  Serial.print( "sensor SETUP:" );
  Serial.print( result );
  Serial.print( "\n" );
  //---------------------
  
  //---------------------
  //task registration
  //---------------------
  TaskManager.add_task(&SecuritySurveillance, SecuritySurveillance.get_invoke_period(), SecuritySurveillance.get_invoke_period());
  TaskManager.add_task(&BlueToothComm, BlueToothComm.get_invoke_period(), BlueToothComm.get_invoke_period());
  //---------------------
  
  //---------------------
  //time cont value initialization
  //---------------------
  StartTime                  = 0;
  EndTime                    = 0;
  //---------------------
  
  //---------------------
  //Initialize and entry the timer 1 interrupt and task invokation information
  //---------------------
  TaskInvokeEvent            = false;
  TaskCounter                = 1;//count for 1 to 10
  delay( 1000 );  // 1[s]
  MsTimer2::set( c_Timer_Interval, isr_timer1 );
  MsTimer2::start();
  //---------------------
  Serial.print( "SETUP DONE:\n" );
}

/**
 * Main loop functions
 * to select and invoke tasks when Task Invoke Event is set
 */
void loop() {
  // [Initialization Process]
  boolean TIE_preserve = TaskInvokeEvent;
  TaskInvokeEvent = false;
  
  StartTime       = millis();
  
  // -----
  //[Invoke Main Task Process]
  if( TIE_preserve == true ){
    
    //invoke tasks
    TaskManager.evaluate();
    
    //Task counter increment or reset
    if(TaskCounter >= c_MaxTimeCounter){
      TaskCounter = 1;
      //Timer1.restart();
    }
    else{
      TaskCounter++;
    }
  }
  // -----
  
  // [Task process time calculation]
  EndTime         = millis();
  ProcessTime     = EndTime - StartTime;
  

}

//******************************
// Interrupt Service Routines
//******************************

/**
 * Timer1 Interrupt Service Routine
 * Sets the Task Invoke Event Flag
 */
void isr_timer1()
{
  TaskInvokeEvent = true;
}


//******************************
// Tasks
//******************************

