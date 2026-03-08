void Print_DC_Error(void){
   // Serial.println(F("Min %15 /Max %99"));  
}
void Print_Color_Error(void){
  //  Serial.println(F("!!! Min-0 Max-255"));  
}

void StoreData(void){
  if(Key.Task) {
      Key.Task = OFF;
      NV_Mem.putUChar("NV_Mode", System.Mode);
  }
  if(Fan.HighSave){
    Fan.HighSave = OFF;  
    NV_Mem.putUChar("NV_Fan_High", Fan.HighSpeed);  
  } 
  if(Fan.MidSave){
    Fan.MidSave = OFF;  
    NV_Mem.putUChar("NV_Fan_Mid", Fan.MidSpeed);  
  } 
  if(Fan.LowSave){
    Fan.LowSave = OFF;  
    NV_Mem.putUChar("NV_Fan_Low", Fan.LowSpeed); 
  } 
  if(Led.HighSave){
    Led.HighSave = OFF;  
    NV_Mem.putUChar("NV_Col_High", Led.ColorHigh);  
  } 
  if(Led.MidSave){
    Led.MidSave = OFF;  
    NV_Mem.putUChar("NV_Col_Mid", Led.ColorMid); 
  } 
  if(Led.LowSave){
    Led.LowSave = OFF;  
    NV_Mem.putUChar("NV_Col_Low", Led.ColorLow); 
  }   
}

void Execute_Serial_Commands(void){
    while (Serial.available()) {
    char incomingChar = Serial.read();  // Read each character from the buffer
    //  static const char LOG_5MSEC[]   PROGMEM = "  5 mS"; //12
    if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
      System.RxUnknown = ON;  
      if (receivedMessage.substring(0,4) == "FanH") { // SpeedHigh (0,9)
       uint8_t Temp = (uint8_t)(receivedMessage.substring(5,8)).toInt();  // (10,13)
        if((Temp > 15) && (Temp < 99)){    
          Fan.HighSpeed = Temp;
           Fan.HighSave = ON;                 
          if(System.Mode == FAN_HIGH)Fan.DutyCycle =Fan.HighSpeed;  
          System.RxSuccess = ON;                   
        }
        else Print_DC_Error(); 
      }
      if (receivedMessage.substring(0,4) == "FanM") { // (0,8) == "SpeedMid") {
       uint8_t Temp = (uint8_t)(receivedMessage.substring(5,8)).toInt();  // (9,12))
        if((Temp > 15) && (Temp < 99)){    
          Fan.MidSpeed = Temp;
          Fan.MidSave = ON;         
          if(System.Mode == FAN_MID)Fan.DutyCycle =Fan.MidSpeed;  
          System.RxSuccess = ON;   
        }     
        else Print_DC_Error(); 
      } 
       if (receivedMessage.substring(0,4) == "FanL") {
       uint8_t Temp = (uint8_t)(receivedMessage.substring(5,8)).toInt(); 
        if((Temp > 15) && (Temp< 99)){
          Fan.LowSpeed = Temp;   
          Fan.LowSave = ON;                 
          if(System.Mode == FAN_LOW)Fan.DutyCycle =Fan.LowSpeed; 
          System.RxSuccess = ON;   

        }     
        else Print_DC_Error();        
      }
       if (receivedMessage.substring(0,4) == "ColH") {  //(0,9) == "ColorHigh")  (10,14))
        uint8_t Temp = (uint8_t)(receivedMessage.substring(5,9)).toInt();  
          Led.ColorHigh = Temp;
          Led.HighSave = ON;
          if(System.Mode == FAN_HIGH)Led.Color = Led.ColorHigh; 
          System.RxSuccess = ON;   
      } 
        if (receivedMessage.substring(0,4) == "ColM") {  // 
         uint8_t Temp = (uint8_t)(receivedMessage.substring(5,9)).toInt();  
        Led.ColorMid = Temp;
        Led.MidSave = ON;         
        if(System.Mode == FAN_MID)Led.Color = Led.ColorMid; 
        System.RxSuccess = ON;   
      }      
       if (receivedMessage.substring(0,4) == "ColL") {  // SpeedMid ColorLow
        uint8_t Temp = (uint8_t)(receivedMessage.substring(5,9)).toInt(); 
        Led.ColorLow = Temp;
        Led.LowSave = ON;
        if(System.Mode == FAN_LOW)Led.Color = Led.ColorLow; 
        System.RxSuccess = ON;   
      } 
      if (receivedMessage.substring(0,5) == "Reset") {  // SpeedMid ColorLow
        System.RxSuccess = ON;   
        ESP.restart(); 
      }      
       if (receivedMessage.substring(0,6) == "DSleep") {  // SpeedMid ColorLow
       // digitalWrite(BOOST_CONV_POWER, OFF);
       // Sleep_Inhibit_Timer = 5;   
        System.RxSuccess = OFF;
        System.RxUnknown = OFF;    
     //   Set_Deep_Sleep();
        System.Deep_SleepTimer  = 3;   
      }     
       if (receivedMessage.substring(0,6) == "LSleep") {  // SpeedMid ColorLow
       // digitalWrite(BOOST_CONV_POWER, OFF);
       // Sleep_Inhibit_Timer = 5;   
        System.RxSuccess = OFF;
        System.RxUnknown = OFF;    
        System.Light_SleepTimer  = 3;    
      }       

      if(System.RxSuccess)System.RxUnknown = OFF;  
      receivedMessage = "";
    } else {
      receivedMessage += incomingChar;
    }
  } // end of while
} // end of serial check function

void Init_NV_MemData(void){
    NV_Mem.begin("NV_MEMORY",RO_MODE );   // RW_MODE  false // RO_MODE true
    bool tpInit = NV_Mem.isKey("nvsInit");  
  if (tpInit == false) {
      // If tpInit is 'false', the key "nvsInit" does not yet exist therefore this
      //  must be our first-time run. We need to set up our Preferences namespace keys. So...
      NV_Mem.end();                             // close the namespace in RO mode and...
      NV_Mem.begin("NV_MEMORY", RW_MODE);        //  reopen it in RW mode.
      // The .begin() method created the "STCPrefs" namespace and since this is our
      //  first-time run we will create
      //  our keys and store the initial "factory default" values.
      NV_Mem.putUChar("NV_Mode", DEVICE_OFF);
      NV_Mem.putUChar("NV_Fan_High", 80);
      NV_Mem.putUChar("NV_Fan_Mid", 60);
      NV_Mem.putUChar("NV_Fan_Low", 40);
      NV_Mem.putUChar("NV_Col_High", 200);
      NV_Mem.putUChar("NV_Col_Mid", 120);
      NV_Mem.putUChar("NV_Col_Low", 20);
      NV_Mem.putBool("nvsInit", true);          // Create the "already initialized"
                                                  //  key and store a value.
      // The "factory defaults" are created and stored so...
      NV_Mem.end();                             // Close the namespace in RW mode and...
   //   NV_Mem.begin("NV_MEMORY", RO_MODE);        //  reopen it in RO mode so the setup code
                                                  //  outside this first-time run 'if' block
                                                  //  can retrieve the run-time values
                                                  //  from the "STCPrefs" namespace.
   }
   NV_Mem.end(); 
   NV_Mem.begin("NV_MEMORY", RW_MODE);        //  reopen it in RW mode.

    System.Mode = NV_Mem.getUChar("NV_Mode");
    Fan.HighSpeed = NV_Mem.getUChar("NV_Fan_High");
    Fan.MidSpeed = NV_Mem.getUChar("NV_Fan_Mid");
    Fan.LowSpeed = NV_Mem.getUChar("NV_Fan_Low");

    Led.ColorHigh = NV_Mem.getUChar("NV_Col_High");
    Led.ColorMid = NV_Mem.getUChar("NV_Col_Mid");
    Led.ColorLow = NV_Mem.getUChar("NV_Col_Low");
   // All done. Last run state (or the factory default) is now restored.
  // NV_Mem.end(); 
}

