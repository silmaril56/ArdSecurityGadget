///////////////////////////////////////////////////////////
//  ITask.h
//  Implementation of the Class ITask
//  Created on:      2015/08/05 18:28:51
//  Original author: gen
///////////////////////////////////////////////////////////

#if !defined(__ITASK_H__)
#define __ITASK_H__

#include "Definitions.h"

/**
 * タスクとして実行されるクラスのインターフェース。
 */
class ITask
{

public:
  //=========================================================
  //  メソッド
  //=========================================================
  /**
   * コンストラクタ
   */
  ITask(){}
  /**
   * デストラクタ
   */
  virtual ~ITask(){}

  /**
   * タスクの起動周期を取得する(最小起動周期で割った値)。
   */
  virtual TInvokePeriod get_invoke_period();
  /**
   * タスクを起動する。
   */
  virtual void invoke();

};
#endif // !defined(__ITASK_H__)

