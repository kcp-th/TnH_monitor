# TnH_monitor
Temperature and Humidity monitor 

 * Climate Conditioner 
 * The idea was originated by Prof Teerawate at Srinakarinwirote University
 * To use microcontroller to monitor temperature and humidity 
 * and then control evaporator (to reduce temperature) and fogger (to increase humidity) to 
 * the appropriate level for the mushroom. 
 * This idea may be applied to other kind plant by adding another type of sensor and equipment
 * for example:
   - measure CO2 and control air flow in close environment
   - measure ambient light and turn on/off eletrical light or shade in green house

 * This is the first prototype: single microcontroller in stand alone environment
 * HW  
  Microcontroller :          Arduino Nano (compatible) 
  Temp and Humidity sensor : DHT22 (AM2302)
  Output :                   2 channels relay 220V 10A 
  User interface:            16x2 LCD with 3 button switch
 
แนวความคิดเริ่มต้นจากอาจารย์ธีรเวทย์ จากมหาวิทยาลัยศรีนครินทรวิโรฒ ที่จะนำไมโครคอนโทรลเลอร์มาใช้ในการตรวจวัดอุณหภูมิและความชื้นจากน้นทำการควบคุมอุปกรณ์ทำความเย็น (evaporator )เพื่อลดอุณหภูมิและเครื่องสร้างไอน้ำเพื่อเพิ่มความชื้นในระดับที่เหมาะสมให้กับโรงเพาะเห็ด
แนวความคิดนี้สามารถนำไปใช้กับพืชชนิดอื่นๆได้โดยการเพิ่มอุปกรณ์ตรวจวัดและอุปกรณ์ควบคุมเช่น
- ตรวจวัดคาร์บอนไดออกไซด์ (ในโรงเรือนระบบปิด) และทำการควบคุมพัดลมถ่ายเทอากาศ
- ตรวจวัดความเข้มของแสงและสั่งปิดเปิดไฟแสงสว่างหรือปิดเปิดสแลนลดแสง

ชุดต้นแบบในขั้นแรกเป็นแบบที่ใช้ไมโครคอนโทรลเลอร์ขนาดเล็กเพียงตัวเดียวและไม่มีการเชื่อมต่อกับ network ภายนอก

Author : kcp-th
