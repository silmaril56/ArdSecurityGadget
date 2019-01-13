///////////////////////////////////////////////////////////
//  CTaskManager.h
//  Implementation of the Class CTaskManager
//  Created on:      2015/08/05 18:28:50
//  Original author: JJ1JQZ
///////////////////////////////////////////////////////////

#if !defined(__CTASKMANAGER_H__)
#define __CTASKMANAGER_H__

#include "Definitions.h"
#include "ITask.h"
#include "CObject.h"

/**
 * タスク管理を行うクラス。
 */
class CTaskManager : public CObject
{

public:
  //=========================================================
  //  メソッド
  //=========================================================
  /**
   * コンストラクタ。
   */
  CTaskManager(){
    m_task_list_size = 0;
  }
  /**
   * デストラクタ。
   */
  virtual ~CTaskManager(){}

  /**
   * 起動タスクを追加する。
   * 配列の並びで優先度が決まるので優先度の高いタスクを先に登録する事。
   */
  void add_task(ITask * task, TInvokePeriod period, TInvokePeriod init_cnt){
    //タスク配列にタスクインターフェースを追加し、起動周期を配列に追加、現在のタイマーカウント0にする。
    m_task_list[m_task_list_size] = task;
    m_task_invoke_period_list[m_task_list_size] = period;
    m_current_period_count_list[m_task_list_size] = init_cnt;

    //タスクリストサイズを+1
    ++m_task_list_size;
  }
  /**
   * 自身に必要な設定情報を収集、設定する。
   * @return true:設定成功、false: 設定失敗
   */
  bool configure();
  /**
   * クラスIDを取得する。
   */
  EClassID get_class_ID(){ return EVCTaskManagerID; }
  /**
   * 呼び出された時点で実行可能なタスクを順次起動する。
   * 配列の並びで優先度が決まる。
   */
  virtual void evaluate();

protected:
  //=========================================================
  //  属性
  //=========================================================
  /**
   * 各タスクの現在の起動周期/基準起動周期のカウント数のリスト。
   */
  TInvokePeriod m_current_period_count_list[5];
  /**
   * 起動するタスクに対する起動周期を基準周期で割った値のリスト。
   * 並び順はm_task_listと同じとする。
   */
  TInvokePeriod m_task_invoke_period_list[5];
  /**
   * 起動するタスクのリスト。
   */
  ITask* m_task_list[5];
  /**
   * タスクリストのサイズ。
   */
  unsigned char m_task_list_size;

};
#endif // !defined(__CTASKMANAGER_H__)

