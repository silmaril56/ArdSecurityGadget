///////////////////////////////////////////////////////////
//  CObject.h
//  Implementation of the Class CObject
//  Created on:      2015/08/05 18:28:49
//  Original author: gen
///////////////////////////////////////////////////////////

#if !defined(__COBJECT_H__)
#define __COBJECT_H__

#include "Definitions.h"
//#include "CDataCentre.h"
//#include "CDebugSupport.h"

class CDataCentre;

/**
 * 全クラスの基底クラス。
 * 全クラスに共有の特徴を有する。
 */
class CObject
{
  //=========================================================
  //  列挙型
  //=========================================================

public:
  //=========================================================
  //  メソッド
  //=========================================================
  /**
   * コンストラクタ。
   */
  CObject(){}
  /**
   * デストラクタ。
   */
  virtual ~CObject(){}
  /**
   * クラスIDを取得する。
   * CObjectを継承するクラスでは必ず、本関数を実装し、EClassIDを返す事。
   * @return: EClassID
   */
  virtual EClassID get_class_ID(){ return EVCObjectID; }

protected:
  //=========================================================
  //  属性
  //=========================================================
};
#endif // !defined(__COBJECT_H__)

