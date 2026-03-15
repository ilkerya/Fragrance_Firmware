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
#include <Preferences.h>
Preferences NV_Mem;
#define RW_MODE false
#define RO_MODE true

#include <Wire.h>
//#include <WiFi.h>
#include <esp_task_wdt.h>
#include  "Defs.h"
#include "driver/rtc_io.h"
#include <ESP32Time.h>
      ESP32Time rtc;
#include <driver/uart.h>

#include "Variables.h"
#include "DAQ.h"
#include "Sensors.h"
#include "MemSave.h"
#include "Functions.h"
#include "Menu.h"
// STR25100

//ESP32 Update Link from preferences
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

void setup() {
  WatchdogTimer_Set();
  Init_IO();
  Serial.begin(115200);
  rtc.setTime(30, 0, 20, 15, 3, 2026);  // 17th Jan 2021 15:24:30
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 //  WiFi.mode(WIFI_MODE_STA); 
  print_wakeup_reason();

  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  Serial.println(reason); // Prints integer value

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
  Init_NV_MemData();     
  Interrupt_Set();
  Key.Inhibit_Timer = 3;  
  rtc_gpio_hold_dis(GPIO_NUM_2);// FAN 12V ON/OFF Control

 // Serial.print("ESP32 MAC Address: ");
 // Serial.println(WiFi.macAddress());
 // WiFi.mode(WIFI_OFF); // save power
}
    
void Rpm_Calculate(){
      Fan.Rpm = 2 * (Fan.Pulse_Low_Latch + Fan.Pulse_High_Latch)+1;
     // if(Fan.Rpm !=0)  Fan.Rpm = 6000000 / Fan.Rpm;  // be careful for divide by 0 errror    
    if(!Fan.Error) Fan.Rpm =  6000000 /Fan.Rpm; 
    else Fan.Rpm = 0;
}

//uint32_t isrCount = 0, isrTime = 0;
uint8_t RunTimer;
void loop() {
   esp_task_wdt_reset(); // 10 seconds
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    //uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
  //  isrCount = isrCounter;
   // isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
  }

  if(System.LOOP_20mSec){
     System.LOOP_20mSec = OFF;
     Key_Functions_Digital();

  }
  if(System.Loop_100mSec){
    System.Loop_100mSec = OFF;
    Mode_Select(); 
    Rpm_Calculate();
  }
   if(System.Loop_500mSec){
     System.Loop_500mSec = OFF;
     Execute_Serial_Commands();
  }
  if(System.LOOP_5Second){
     System.LOOP_5Second = OFF;

      StoreData();
      Read_TVoc();
  }
 if(System.LOOP_1Second){
     System.LOOP_1Second = OFF;

    if(Key.Inhibit_Timer)Key.Inhibit_Timer--;
    if(System.Light_SleepTimer){
        System.Light_SleepTimer--;
        if(System.Light_SleepTimer == 0)System.Light_Sleep = ON; 
    } 
    if(System.RTC_SleepTimer){
        System.RTC_SleepTimer--;
        if(System.RTC_SleepTimer == 0)System.RTC_Sleep = ON; 
    }    
    if(System.Deep_SleepTimer){
        System.Deep_SleepTimer--;
        if(System.Deep_SleepTimer == 0)Set_Deep_Sleep();
    } 
    if(System.Index_UpdateTimer){
        System.Index_UpdateTimer--;
        if(System.Index_UpdateTimer == 0)System.Index_Update = ON; 
    }    

    digitalWrite(SENSOR_3V_POWER, SENSOR_3V_ENABLE); 
    Read_Temperature();
   
    Read_Light();

    Battery_Volt();

    System.PC_Serial_Mode = OFF;
    if(System.PC_Serial_Mode)
        DAQ_Send_Data(LOOP_BASED); 
    else{
      if((System.Light_SleepTimer==1) || (System.Light_Sleep) ){
        Serial.println(F("Light Sleep!")); 
        return;
      }
      if((System.RTC_SleepTimer==1) || (System.RTC_Sleep) ){
        Serial.println(F("RTC Sleep!")); 
        return;
      }      
      if(System.Deep_SleepTimer==1){
        Serial.println(F("Deep Sleep!")); 
        return;
      }




    if(System.RxSuccess){
      System.RxSuccess = OFF;   
      Serial.println(F("Message Success!")); 
    }
    if(System.RxUnknown){
      System.RxUnknown = OFF;   
      Serial.println(F("Message Failed!")); 
    }   
    if(System.Version){        
        System.Version = OFF;  
        Serial.print(F("Compile Date & Time ")); 
        Serial.println(__DATE__ ", " __TIME__); 
      }

      if(System.Mode <= RUN_TEST_LIMIT) {
          System.MonitorTimer++;
          if(System.MonitorTimer <  5) return;
      }
      System.MonitorTimer = 0;

      if(System.Mode == TEST_OFF) Serial.print(F("TEST_Off ")); 
      if(System.Mode == TEST_HIGH)   Serial.print(F("TEST_High ")); 
      if(System.Mode == TEST_MID) Serial.print(F("TEST_Mid ")); 
      if(System.Mode == TEST_LOW)    Serial.print(F("TEST_Low "));      

      if(System.Mode == RUN_OFF) Serial.print(F("OFF ")); 
      if(System.Mode == RUN_HIGH)   Serial.print(F("HIGH ")); 
      if(System.Mode == RUN_MID) Serial.print(F("MID ")); 
      if(System.Mode == RUN_LOW)    Serial.print(F("LOW "));   


      if(System.Mode <= RUN_TEST_LIMIT) {
        //Serial.print((System.Cycle+1));Serial.print(".");
        Serial.print((System.Index+1));Serial.print(".");Serial.print(System.RunTimer/10); Serial.print(F("  "));        
      }
      Serial.print(Fan.Rpm); Serial.print(F("Rpm-%"));Serial.print(Fan.DutyCycle); Serial.print(F("DC Bat:")) ;    
          // Serial.print(F("  Color:")); 
    //  Serial.print(Battery.Volt/1000);Serial.print('.');
    //  uint16_t temp  = Battery.Volt%1000;
    // float temp = (float)Battery.Volt/1000;
      Battery.F_Val = (float)Battery.Volt;
      Battery.F_Val /= 1000;
      Serial.print(Battery.F_Val,2);  

     Serial.print(F("V Col:")) ;    
     Serial.print(Led.Color); 
     Serial.print(F("  ")); 
     Serial.print(Values.Temperature,1);Serial.print(F("°C %")); Serial.print(Values.Humidity,0);   
     // Serial.print(" TVOC: ");
     Serial.print(F("rh "));
     if(Values.TVoc_Error== ON)Serial.print(F("  "));
     else Serial.print(Values.TVoc);
     Serial.print(F("ppb "));
     Serial.print(Values.Lux,1); Serial.print(F("Lux %FSet:")); 	
     Serial.print(Fan.LowSpeed);Serial.print(F("/")); Serial.print(Fan.MidSpeed);Serial.print(F("/")); Serial.print(Fan.HighSpeed);
     Serial.print(F(" CSet:")); 
     Serial.print(Led.ColorLow);Serial.print(F("/")); Serial.print(Led.ColorMid);Serial.print(F("/")); Serial.print(Led.ColorHigh);
     Serial.print(F("  Std:")); Serial.print(Battery.Standbye);Serial.print(F("  Chg:")); Serial.print(Battery.Charge);
    // Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S")); 
    Serial.print(rtc.getTime(" %H:%M:%S %d.%B.%Y")); 
    //    Serial.print(F("  Ver:"));
     // Serial.print(__DATE__", "__TIME__","__VERSION__); 
    Serial.println(""); 


    }  
  }
}



