#!/usr/bin/env python3
# =========================================================
#   SecuritySurveillance.py
#   Implementation of the Server Side security cuveillance script.
#   Created on:      2019/01/09
#   Original author: silmaril56
# =========================================================

# =========================================================
# --------------------
# import library
# --------------------
import datetime
import smtplib
import ambient
import serial
import signal
import time
from collections import deque
from email.mime.text import MIMEText
from email.utils import formatdate


# =========================================================
# email setting
# =========================================================
# --------------------
# Ambient channel Id
# --------------------
CHANNEL_ID = 
# --------------------
# Ambient Write Key
# --------------------
WRITE_KEY = ''

# =========================================================
# email setting
# =========================================================
# --------------------
# 送信元メールアドレス
# --------------------
FROM_ADDRESS = ''

# --------------------
# 送信元メールアカウントログインパスワード
# --------------------
MY_PASSWORD  = ''

# --------------------
# 送信先アドレス
# --------------------
TO_ADDRESS   = ''

# --------------------
# 送信先BCCアドレス
# --------------------
BCC_ADDRESS   = ''

# --------------------
# タイトルテキスト
# --------------------
SUBJECT      = '★自宅セキュリティ警報★'

# --------------------
# 本文ヘッダテキスト
# --------------------
BODY         = '自宅入り口のセキュリティ装置による侵入者検出警報\n\n'


# =========================================================
# Detaction setting
# =========================================================
# --------------------
# 人物通過判定距離敷居値(通過時:通過検出距離(センサーと反射板の距離[mm] - 数十[mm]))
# --------------------
DISTANCE_THRESHOLD_PASSING = 700
# --------------------
# 人物通過判定距離敷居値(通過後時:通過後に戻る距離(センサーと反射板の距離[mm] - 数十[mm]))
# --------------------
DISTANCE_THRESHOLD_CLEAR   = 600
# --------------------
# センサー値取得周期
# --------------------
SENSOR_DATA_RETREAVE_PERIOD   = 0.25# 250[ms]


# =========================================================
# BT communication setting
# =========================================================
# --------------------
# device boud rate
# --------------------
DEVICE_BAUD_RATE      = 9600
# --------------------
# BT communication time out[s]
# --------------------
COMM_TIMEOUT          = 1000
# --------------------
# device path of the port that gadget will communicate
# --------------------
DEVICE_PATH           = '/dev/rfcomm0'
# --------------------
# the total bytes per one transfer from gadget.
# --------------------
MAX_BYTES_PER_RECEIVE = 27


# =========================================================
# BT receive circuler buffer class
# =========================================================
class CircularBuffer(object):
    # --------------------
    # コンストラクタ
    # --------------------
    def __init__(self, size, init_val):
        """initialization"""
        self.index= 0
        self.size= size
        self._data = bytearray([])
        for i in range(size):
          self._data.append(init_val)

    # --------------------
    # 要素追加
    # --------------------
    def push(self, value):
        """append an element"""
        if len(self._data) == self.size:
            self._data[self.index]= value
        else:
            self._data.append(value)
        self.index= (self.index + 1) % self.size
    
    # --------------------
    # 要素初期化
    # --------------------
    def initialize(self, key, init_val):
        """set element by index"""
        self._data[key] = init_val
    
    # --------------------
    # 要素参照
    # --------------------
    def __getitem__(self, key):
        """get element by index like a regular array"""
        return(self._data[key])

    # --------------------
    # 全要素取得
    # --------------------
    def get_all(self):
        """return a list of all the elements"""
        return(self._data)
    
    # --------------------
    # コメント
    # --------------------
    def __repr__(self):
        """return string representation"""
        return self._data.__repr__() + ' (' + str(len(self._data))+' items)'


# =========================================================
# BT receive Data word class
# =========================================================
class DataWord(object):
    # --------------------
    # コンストラクタ
    # --------------------
    def __init__(self, buffer):
        """initialization"""
        if len(buffer) == 5:
          #self.data_id = int.from_bytes(buffer[0], byteorder='big', signed=False)
          self.data_id = buffer[0]
          if self.data_id == 5:
              #self.error_code1 = int.from_bytes(buffer[2], byteorder='big', signed=False)
              self.error_code1 = buffer[2]
              #self.error_code2 = int.from_bytes(buffer[4], byteorder='big', signed=False)
              self.error_code2 = buffer[4]
          else:
              self.error_code1 = 0
              self.error_code2 = 0
          self.data = int.from_bytes(buffer[1:4], byteorder='big', signed=False)
        else:
          #self.data_id = int.from_bytes(buffer[0], byteorder='big', signed=False)
          self.data_id = buffer[0]
          self.error_code1 = 0
          self.error_code2 = 0
          self.data = 4000
    # --------------------
    # 値設定
    # --------------------
    def set(self, buffer):
        """initialization"""
        if len(buffer) == 5:
          #self.data_id = int.from_bytes(buffer[0], byteorder='big', signed=False)
          self.data_id = buffer[0]
          if self.data_id == 5:
              #self.error_code1 = int.from_bytes(buffer[2], byteorder='big', signed=False)
              self.error_code1 = buffer[2]
              #self.error_code2 = int.from_bytes(buffer[4], byteorder='big', signed=False)
              self.error_code2 = buffer[4]
          else:
              self.error_code1 = 0
              self.error_code2 = 0
          self.data = int.from_bytes(buffer[1:4], byteorder='big', signed=False)
        else:
          #self.data_id = int.from_bytes(buffer[0], byteorder='big', signed=False)
          self.data_id = buffer[0]
          self.error_code1 = 0
          self.error_code2 = 0
          self.data = 4000


# =========================================================
# global variable definition
# =========================================================
# --------------------
# 受信バッファ
# --------------------
rx_buffer = CircularBuffer(MAX_BYTES_PER_RECEIVE * 2, 254)

# --------------------
# 受信データリスト
# --------------------
rx_word = {}

# --------------------
# 侵入検出フラグ
# --------------------
Entrance_detect = False

# --------------------
# Ambient送信クラスインスタンス
# --------------------
am = ambient.Ambient(CHANNEL_ID, WRITE_KEY)



# =========================================================
# create email body text message
# =========================================================
def create_body_text(body_header, 
                     sensor1_distance, 
                     sendor1_lp_distance, 
                     sensor2_distance, 
                     sendor2_lp_distance, 
                     gadget1_error_code):
  body_text = ''
  
  body_text += body_header
  body_text += '日時: ' + datetime.datetime.today().strftime("%Y年%m月%d日 %H時%M分%S秒") + '\n'
  body_text += '■デバイス1\n'
  body_text += '1.センサー1: ' + str(sensor1_distance) + '\n'
  body_text += '2.センサー1(filtered): ' + str(sendor1_lp_distance) + '\n'
  body_text += '3.センサー2: ' + str(sensor2_distance) + '\n'
  body_text += '4.センサー2(filtered): ' + str(sendor2_lp_distance) + '\n'
  body_text += '5.デバイス1 Error Code: ' + str(gadget1_error_code) + '\n'
  body_text += '------------------\n'
  
  return body_text


# =========================================================
# create email message
# =========================================================
def create_message(from_addr, to_addr, bcc_addrs, subject, body):
  msg = MIMEText(body)
  msg['Subject'] = subject
  msg['From'] = from_addr
  msg['To'] = to_addr
  msg['Bcc'] = ''
  msg['Date'] = formatdate()
  
  return msg


# =========================================================
# send email via gmail
# =========================================================
def send_email(from_addr, to_addrs, msg):
  smtpobj = smtplib.SMTP('smtp.gmail.com', 587)
  smtpobj.ehlo()
  smtpobj.starttls()
  smtpobj.ehlo()
  smtpobj.login(FROM_ADDRESS, MY_PASSWORD)
  smtpobj.sendmail(from_addr, to_addrs, msg.as_string())
  smtpobj.close()


# =========================================================
# send sensor data to ambient with retry
# =========================================================
def sendWithRetry(data):
  for retry in range(6):  # 10秒間隔で6回リトライして、ダメならこの回は送信しない
    try:
      ret = am.send(data)
      break
    except requests.exceptions.RequestException as e:
      time.sleep(1)
  
  return ret


# =========================================================
# send sensor data to ambient
# =========================================================
def send2ambient():
  ret = sendWithRetry({'d1': rx_word[1].data, 
                       'd2': rx_word[2].data, 
                       'd3': rx_word[3].data, 
                       'd4': rx_word[5].data, 
                       'd5': rx_word[5].data})


# =========================================================
# retreive data via BT
# =========================================================
def receive_security_gadget_data():
  received_data = ''
  
  ser = serial.Serial(DEVICE_PATH, DEVICE_BAUD_RATE, timeout=COMM_TIMEOUT)
  
  bytes_received = ser.inWaiting()
  if bytes_received > 0:
    received_data = ser.read(bytes_received)
    for value in received_data:
      rx_buffer.push(value)
    
  ser.close()
  
  return rx_buffer
  
# =========================================================
# cutout received data
# =========================================================
def cutout_security_gadget_data():
  received_data = bytearray(b'')
  first_pos = -1
  last_pos = -1
  for i in range(rx_buffer.size):
    if ((rx_buffer[i] == b'\xFF') and (first_pos < 0)):
      first_pos = i
      if ((last_pos > 0) and ((rx_buffer.size - i) > MAX_BYTES_PER_RECEIVE)):
        last_pos = -1
    if (i + 1) < rx_buffer.size:
      if ((rx_buffer[i] == b'\xFE') and (rx_buffer[i + 1] == b'\xFE') and (last_pos < 0)):
        last_pos = i - 1
  
  if last_pos <= 0:
    rx_buffer.size - 1
  
  if (first_pos >= 0) and (last_pos >= 0):
    if last_pos < first_pos:
      # wrap around case
      for i in range((rx_buffer.size - first_pos) - 1):
        received_data.append(rx_buffer[first_pos + i])
        rx_buffer.initialize(first_pos + i, 254)
      
      for i in range(last_pos):
        received_data.append(rx_buffer[i])
        rx_buffer.initialize(i, 254)
    else:
      # straight forward case
      for i in range(last_pos - first_pos):
        received_data.append(rx_buffer[first_pos + i])
        rx_buffer.initialize(first_pos + i, 254)
  
  return received_data


# =========================================================
# translate received data
# =========================================================
def translate_security_gadget_data(received_data):
  if len(received_data) == MAX_BYTES_PER_RECEIVE:
    rx_word[0].set(bytearray([0, 0, 0, 0, 0]))
    rx_word[1].set(received_data[2:6])
    rx_word[2].set(received_data[7:11])
    rx_word[3].set(received_data[12:16])
    rx_word[4].set(received_data[17:21])
    rx_word[5].set(received_data[22:26])
  else:
    rx_word[0].set(bytearray([0, 0, 0, 0, 0]))
    rx_word[1].set(bytearray([0, 0, 0, 0, 0]))
    rx_word[2].set(bytearray([0, 0, 0, 0, 0]))
    rx_word[3].set(bytearray([0, 0, 0, 0, 0]))
    rx_word[4].set(bytearray([0, 0, 0, 0, 0]))
    rx_word[5].set(bytearray([0, 0, 0, 0, 0]))


# =========================================================
# translate received data
# =========================================================
def analyze_security_gadget_data():
  global Entrance_detect
  sensor1_distance = rx_word[1].data
  sendor1_lp_distance = rx_word[2].data
  sensor2_distance = rx_word[3].data
  sendor2_lp_distance = rx_word[4].data
  gadget1_error_code = rx_word[5].data
  sensor1_detact = False
  sensor2_detact = False

  if Entrance_detect == False:
    if sensor1_distance < DISTANCE_THRESHOLD_PASSING:
      sensor1_detact = True
    else:
      sensor1_detact = False
    
    if sensor2_distance < DISTANCE_THRESHOLD_PASSING:
      sensor2_detact = True
    else:
      sensor2_detact = False
    
    if ((sensor1_detact == True) or  (sensor2_detact == True)):
      Entrance_detect = True
  else:
    if sensor1_distance > DISTANCE_THRESHOLD_CLEAR:
      sensor1_detact = False
    else:
      sensor1_detact = True
    
    if sensor2_distance > DISTANCE_THRESHOLD_CLEAR:
      sensor2_detact = False
    else:
      sensor2_detact = True

    if ((sensor1_detact == False) and  (sensor2_detact == False)):
      Entrance_detect = False
  
  return Entrance_detect



# =========================================================
# security surveillance main process function.
# this will be executed periodically.
# =========================================================
def security_surveillance_process(signum, frame):
  global Entrance_detect
  sensor1_distance = 0
  sendor1_lp_distance = 0
  sensor2_distance = 0
  sendor2_lp_distance = 0
  gadget1_error_code = 0
  entrance_result_pre = Entrance_detect
  # retrieve a security gadget data.
  receive_security_gadget_data()
  received_data = cutout_security_gadget_data()
  translate_security_gadget_data(received_data)
  
  # analyze sensor value
  entrance_result = analyze_security_gadget_data()
  
  # take sensor values
  sensor1_distance = rx_word[1].data
  sendor1_lp_distance = rx_word[2].data
  sensor2_distance = rx_word[3].data
  sendor2_lp_distance = rx_word[4].data
  gadget1_error_code = rx_word[5].data

  # send data via ambient
  send2ambient()

  # take a email setting and build a email text.
  if entrance_result != entrance_result_pre:
    body    = create_body_text(BODY, 
                               sensor1_distance, 
                               sendor1_lp_distance, 
                               sensor2_distance, 
                               sendor2_lp_distance, 
                               gadget1_error_code)
    
    # create a email message.
    msg = create_message(FROM_ADDRESS, TO_ADDRESS, BCC_ADDRESS, SUBJECT, body)
    
    # send email.
    send_email(FROM_ADDRESS, TO_ADDRESS, msg)
  entrance_result_pre = Entrance_detect
  

# =========================================================
# main function call
# server will start execute from here.
# =========================================================
signal.signal(signal.SIGALRM, security_surveillance_process)
signal.setitimer(signal.ITIMER_REAL, SENSOR_DATA_RETREAVE_PERIOD, SENSOR_DATA_RETREAVE_PERIOD)

if __name__ == '__main__':
  # initialize
  Entrance_detect = False
  rx_word[0] = DataWord(bytearray(b'\x00\x00\x00\x00\x00'))
  rx_word[1] = DataWord(bytearray(b'\x01\x00\x00\x00\x00'))
  rx_word[2] = DataWord(bytearray(b'\x02\x00\x00\x00\x00'))
  rx_word[3] = DataWord(bytearray(b'\x03\x00\x00\x00\x00'))
  rx_word[4] = DataWord(bytearray(b'\x04\x00\x00\x00\x00'))
  rx_word[5] = DataWord(bytearray(b'\x05\x00\x00\x00\x00'))

  # infinite loop for continuous execution.
  while True:
    time.sleep(1)

  