///////////////////////////////////////////////////////////
//  CTaskManager.cpp
//  Implementation of the Class CTaskManager
//  Created on:      2015/08/05 18:28:50
//  Original author: gen
///////////////////////////////////////////////////////////

#include "CTaskManager.h"


/**
 * 自身に必要な設定情報を収集、設定する。
 * @return true:設定成功、false: 設定失敗
 */
bool CTaskManager::configure(){

  return true;
}

/**
 * 呼び出された時点で実行可能なタスクを順次起動する。
 */
void CTaskManager::evaluate(){
  for (int i = 0; i < this->m_task_list_size; ++i){

    //もし、カウンタ値がタスク起動周期となっていたら
    if (m_current_period_count_list[i] >= m_task_invoke_period_list[i]){
      //タスク起動
      m_task_list[i]->invoke();
      //カウンタリセット
      m_current_period_count_list[i] = 0;
    }

    //カウンタを進める
    ++m_current_period_count_list[i];
  }
}

