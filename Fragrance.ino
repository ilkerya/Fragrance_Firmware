// git add .
// git commit -m "Time Stamp"
// https://github.com/ilkerya/Scent_Diffuser-Firmware.git

/*
 Repeat timer example

 This example shows how to use hardware timer in ESP32. The timer calls onTimer
 function every second. The timer can be stopped with button attached to PIN 0
 (IO0).
 This example code is in the public domain.

 ESP WROOM 32E ->
 https://documentation.espressif.com/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf

 ESP32-D0WD-V3 chip or ESP32-D0WDR2-V3 chip
https://documentation.espressif.com/esp32_datasheet_en.pdf


 */
// Stop button is attached to PIN 0 (IO0)

#include <Wire.h>
//#include <RTClib.h>
//#include <Adafruit_Si7021.h>


#define LED_OUT

//#include <Adafruit_SSD1306.h>

#include  "Defs.h"

#include "driver/rtc_io.h"


#include "Variables.h"
#include "DAQ.h"
#include "Sensors.h"
#include "Functions.h"
//#include  "UserInt.h"
//#include  "Display.h"
//#include  "Menu.h"



//ESP32 Update Link from preferences
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

void setup() {
  Init_IO();
  Serial.begin(115200);
  Wire.begin (SDA, SCL);   // sda= GPIO_18 /scl= GPIO_19
//  Found address: 26 (0x1A)
// Found address: 56 (0x38)
// Found address: 83 (0x53)
 // Scanner ();
   //analogWrite(ledPin, dutyCycle);
 // print_wakeup_reason();

  Init_TempHSensors();     
  Init_Light_Sensor();
  Init_TVoc();

  Interrupt_Set();

}


uint8_t PWM_Counter = 0;
uint8_t CounterPI;


uint32_t isrCount = 0, isrTime = 0;
void loop() {
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    //uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
  }
    // Print it
  
  


  if(LOOP_1Second){
    Led_Control();
     PWM_Counter++;
    if(PWM_Counter > 205)PWM_Counter = 0;
    ledcWrite(FAN_PWM, PWM_Counter); 

     // digitalWrite(SENSOR_3V_POWER, SENSOR_3V_ENABLE); 
      digitalWrite(SENSOR_3V_POWER, SENSOR_3V_ENABLE); 
    Read_Temperature();
    Read_TVoc();
    Read_Light();


        uint32_t  DutyCycle  ;
        DutyCycle  = ((255-PWM_Counter)*100)/255;
        Rpm = 2 * (Pulse_Low_Latch + Pulse_High_Latch);
        Rpm = 6000000 / Rpm;   


      PC_Serial_Mode = OFF;
      if(PC_Serial_Mode)
          DAQ_Send_Data(LOOP_BASED); 
      else{
       // Boost.Led_Pos 
       /*
        switch (Boost.Mode) {
        case DEVICE_OFF:Serial.print(ARR_DEVICE_OFF);
          break;
        case FAN_MID:Serial.print(ARR_FAN_MID);
          break;
        case FAN_HIGH:Serial.print(ARR_FAN_HIGH);
          break;
         case FAN_LOW:Serial.print(ARR_FAN_LOW);
        break;
          case FAN_STANDBYE:Serial.print(ARR_FAN_STANDBYE);
        break;
      }
      */  
       Serial.print("Fan: "); Serial.print(Rpm); Serial.print(" Rpm  %");Serial.print(DutyCycle); Serial.print("DC  ") ;     
      Serial.print(Values.Temperature);Serial.print("°C   Rh%"); Serial.print(Values.Humidity);   
      Serial.print("  TVOC:");Serial.print(Values.TVoc);Serial.print("ppb  ");
      Serial.print(Values.Lux); Serial.print("Lux"); 	
      Serial.print("   Int:");Serial.print(lastIsrAt_Diff);Serial.print("us "); 
   //   Serial.print(" Key: "); 	Serial.print(Key.Key1_Rel); 
      Serial.print(" Key: ");
      Serial.print( Key.Key1);
      Serial.print( Key.Key1_Rel);
      Serial.print(" Status: ");
      Serial.print( Key.Status);Serial.print("  KeyTimer:"); Serial.print( Key.TimerPress); 
      
      Serial.println(""); 

    }
       LOOP_1Second = OFF;
    }
}
void PI_Control(){

    CounterPI++;
  if(CounterPI > 16){
    CounterPI = 0;
    
   if((Boost.Mode != DEVICE_OFF) &  (Boost.Mode != FAN_STANDBYE) )
      Boost.error =  Boost.Actual_Fan_Volt -  Boost.Fan_Target_Volt;
    else Boost.error = 1000;
      //error = Boost.Target_DAC_Volt - Boost.Volt;
      //error = Boost.DAC_Volt - Boost.Volt;
      // DAC higher Volgate Lower
 //  if(Boost.Led_Pos != 0){        
      if(Boost.error > 128 )Boost.DAC ++;
      if(Boost.error < -128 ) Boost.DAC --;
 //   }  
    if(Boost.DAC > 252) Boost.DAC = 252;
    if(Boost.DAC < 32)  Boost.DAC = 32;
   // dacWrite(DAC_OUT, Boost.DAC);//0-255 0-3V3 A1_DAC1_IO25
    } 

}


