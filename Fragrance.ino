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
//#include <EEPROM.h>
Preferences NV_Mem;
#define RW_MODE false
#define RO_MODE true

#include <Wire.h>
#include <esp_task_wdt.h>
#include  "Defs.h"
#include "driver/rtc_io.h"
#include <driver/uart.h>
  
#include "Variables.h"
#include "DAQ.h"
#include "Sensors.h"
#include "SaveData.h"
#include "Functions.h"

// STR25100

//ESP32 Update Link from preferences
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

void setup() {
    WatchdogTimer_Set();
  Init_IO();
  Serial.begin(115200);
 
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
  rtc_gpio_hold_dis(GPIO_NUM_2);
}
    
  void Rpm_Calculate(){
      Fan.Rpm = 2 * (Fan.Pulse_Low_Latch + Fan.Pulse_High_Latch)+1;
     // if(Fan.Rpm !=0)  Fan.Rpm = 6000000 / Fan.Rpm;  // be careful for divide by 0 errror    
    if(!Fan.Error) Fan.Rpm =  6000000 /Fan.Rpm; 
    else Fan.Rpm = 0;
  }

uint8_t CounterPI;


uint32_t isrCount = 0, isrTime = 0;
void loop() {
   esp_task_wdt_reset(); // 10 seconds
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    //uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
  }
  /*
    if(LOOP_1mSec){
     LOOP_1mSec = OFF;
       // Execute_Serial_Commands();
    }
*/
  if(System.LOOP_20mSec){
     System.LOOP_20mSec = OFF;
    Mode_Select(); 
    Rpm_Calculate();
  }
   if(System.Loop_100mSec){
     System.Loop_100mSec = OFF;
    Execute_Serial_Commands();
  }
  
  if(System.LOOP_1Second){
     System.LOOP_1Second = OFF;

    if(System.Light_SleepTimer){
        System.Light_SleepTimer--;
        if(System.Light_SleepTimer == 0)System.Light_Sleep = ON; 
    } 
    if(System.Deep_SleepTimer){
        System.Deep_SleepTimer--;
        if(System.Deep_SleepTimer == 0)Set_Deep_Sleep();
    } 
     if(Key.Inhibit_Timer)Key.Inhibit_Timer--;

    StoreData();

    digitalWrite(SENSOR_3V_POWER, SENSOR_3V_ENABLE); 
    Read_Temperature();
    Read_TVoc();
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
      if(System.Deep_SleepTimer==1){
        Serial.println(F("Deep Sleep!")); 
        return;
      }

      if(System.Mode == DEVICE_OFF) Serial.print(F("Off ")); 
      if(System.Mode == FAN_HIGH)   Serial.print(F("High ")); 
      if(System.Mode == FAN_MID) Serial.print(F("Mid ")); 
      if(System.Mode == FAN_LOW)    Serial.print(F("Low "));      
     //  Serial.print("Fan: "); 
       Serial.print(Fan.Rpm); Serial.print(F("Rpm-%"));Serial.print(Fan.DutyCycle); Serial.print(F("DC Bat:")) ;    
          // Serial.print(F("  Color:")); 

          Serial.print(Battery.Volt);Serial.print(F("mV Col:")) ;    
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

    Serial.print(F("  Standbye:")); Serial.print(Battery.Standbye);Serial.print(F("  Charge:")); Serial.print(Battery.Charge);

    //Serial.print(Key.Inhibit_Timer);


    //  Serial.print("   Int:");Serial.print(lastIsrAt_Diff);Serial.print("us "); 
   //   Serial.print(" Key: "); 	Serial.print(Key.Key1_Rel); 
    //  Serial.print(" Key: ");
    //  Serial.print( Key.Key1);
    //  Serial.print( Key.Key1_Rel);
    //  Serial.print(" Status: ");
    //  Serial.print( Key.Status);
   //   Serial.print("  KeyTimer:"); Serial.print( Key.TimerPress); 

    //    Serial.print(F("  Ver:"));
  // Serial.print(__DATE__", "__TIME__","__VERSION__); 

      Serial.println(""); 

      if(System.RxSuccess){
        System.RxSuccess = OFF;   
        Serial.println(F("Message Success!")); 
       }
       if(System.RxUnknown){
        System.RxUnknown = OFF;   
        Serial.println(F("Message Failed!")); 
       }     
       

    }  
  }
}



