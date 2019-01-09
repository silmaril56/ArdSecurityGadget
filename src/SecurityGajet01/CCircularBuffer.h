///////////////////////////////////////////////////////////
//  CCircularBuffer.h
//  Implementation of the Class CCircularBuffer
//  Created on:      2018/09/19
//  Original author: JJ1JQZ
///////////////////////////////////////////////////////////

#if !defined(__CCCIRCULARBUFFER_H__)
#define __CCCIRCULARBUFFER_H__

#include "Definitions.h"
#include "CObject.h"
#include "HardwareSerial.h"

/**
 * FIFOデータ構造
 */
class CCircularBuffer : public CObject
{
public:
  //=========================================================
  //  メソッド
  //=========================================================
  /**
  * コンストラクタ。
  */
  CCircularBuffer(){
    write_p = 0;
    read_p = 0;
    is_filled = false;
    
    //初期化
    for(int i = 0; i < c_CircularBufferSize; ++i){
      data[i] = 0;
    }
  }
  /**
   * デストラクタ。
   */
  virtual ~CCircularBuffer(){}
  
  /**
   * 要素数取得
   */
  int count(){
    int count = read_p - write_p;
    if (count < 0) count += c_CircularBufferSize;
    return count;
  }
  /**
   * 要素追加
   */
  void push(TCBUF_D val){
    write_p = (write_p - 1) & c_Mask;
    if(is_full() == true){is_filled == true;}
    data[write_p] = val;
  }
  /**
   * 要素追加(バッファがfullになったらpop()してからpushする)
   */
  void push_and_pop(TCBUF_D val){
    if(is_full() == true){
      pop();
    }
    push(val);
  }
  /**
   * 要素取得(with delete)
   */
  TCBUF_D pop(){
    TCBUF_D return_value = data[read_p];
    bool is_empty = (read_p == write_p);
    
    if(is_empty != true){
      read_p = (read_p - 1) & c_Mask;
      is_filled = false;
      return return_value;
    }
    else{
      is_filled = false;
      return 0x0000;
    }
  }
  /**
   * 要素取得(without delete)
   */
  TCBUF_D top(){
    TCBUF_D return_value = data[read_p];
    bool is_empty = (read_p == write_p);
    
    if(is_empty != true){
      return return_value;
    }
    else{
      return 0x0000;
    }
  }
  /**
   * 最新要素取得(without delete)
   */
  TCBUF_D newest(){
    TCBUF_D return_value = data[write_p];
    return return_value;
  }
  /**
   * 要素取得(without delete and index access)
   */
  TCBUF_D at(int index){
    if(index < c_CircularBufferSize){
      return data[index];
    }
    else{
      return 0x0000;
    }
  }
  /**
   * 移動平均
   */
  TCBUF_D mean(){
    TCBUF_D sum = 0;
    for(int i = 0; i < c_CircularBufferSize; ++i){
      sum += data[i];
    }
    
    return (sum / c_CircularBufferSize);
  }
  /**
   * 要素数0か否か
   */
  bool is_null(){
    return (read_p == write_p);
  }
  /**
   * バッファが埋まっているか否か
   */
  bool is_full(){
    return (((read_p - 1) & c_Mask) == (write_p & c_Mask) );
  }
protected:

protected:
  //=========================================================
  //  属性
  //=========================================================
  /**
   * 保持するデータの配列
   */
  TCBUF_D data[c_CircularBufferSize];
  
  /**
   * write pointer
   */
  int write_p;
  
  /**
   * read pointer
   */
  int read_p;
  bool is_filled;
protected:
  /**
   * カウンター初期化範囲マスク
   */
  static const int c_Mask = c_CircularBufferSize -1;
};
#endif // !defined(__CCCIRCULARBUFFER_H__)
