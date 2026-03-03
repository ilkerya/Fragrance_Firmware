void Print_DC_Error(){
    Serial.println(F("Not Set Min %15 /Max %99"));  
}
void Execute_Serial_Commands(){
  uint32_t DutyCycleTemp;
    while (Serial.available()) {
    char incomingChar = Serial.read();  // Read each character from the buffer
    //  static const char LOG_5MSEC[]   PROGMEM = "  5 mS"; //12
    if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
      // Print the message
    // Serial.print("Rx: ");
     // Serial.println(receivedMessage);
      Serial.print("Message Rx   ");
      if (receivedMessage.substring(0,9) == "SpeedHigh") {
        Serial.print(F("SpeedHigh Fan Set To "));
        DutyCycleTemp = (receivedMessage.substring(10,13)).toInt(); 
        if((DutyCycleTemp > 15) && (DutyCycleTemp < 99)){    
          Fan.HighSpeed = (uint8_t)DutyCycleTemp;
          if(Mode == FAN_HIGH)Fan.DutyCycle =Fan.HighSpeed; 
          Serial.print(F("HighSpeed: ")); 
          //  EEPROM.write(EPPROM_ADR_SPEED_HIGH, Fan.HighSpeed); 
          //  EEPROM.commit(); 
             NV_Mem.putUChar("NV_Mem_Fan_High", Fan.HighSpeed);

             Serial.print(NV_Mem.getUChar("NV_Mem_Fan_High", 0)); 
                  
              Serial.println(F("% Duty Cycle"));      
            
        }
        else Print_DC_Error(); 
      }
      if (receivedMessage.substring(0,8) == "SpeedMid") {
        Serial.print(F("SpeedMid Fan Set To "));
        DutyCycleTemp = (receivedMessage.substring(9,12)).toInt(); 
        if((DutyCycleTemp > 15) && (DutyCycleTemp < 99)){    
          Fan.MidSpeed = (uint8_t)DutyCycleTemp;
          if(Mode == FAN_MID)Fan.DutyCycle =Fan.MidSpeed; 
          Serial.print(Fan.MidSpeed);   
          Serial.println(F("% Duty Cycle")); 
         // EEPROM.write(EPPROM_ADR_SPEED_MID, Fan.MidSpeed); 
         // EEPROM.commit(); 
           NV_Mem.putUChar("NV_Mem_Fan_Mid", Fan.MidSpeed);
           
        }     
        else Print_DC_Error(); 

      } //       Serial.print(F("  Compiled:"));
       if (receivedMessage.substring(0,8) == "SpeedLow") {
        Serial.print(F("SpeedLow Fan Set To "));
        DutyCycleTemp = (receivedMessage.substring(9,12)).toInt(); 
        if((DutyCycleTemp > 15) && (DutyCycleTemp < 99)){          
          Fan.LowSpeed = (uint8_t)DutyCycleTemp;
          if(Mode == FAN_LOW)Fan.DutyCycle =Fan.LowSpeed; 
          Serial.print(Fan.LowSpeed);   
          Serial.println(F("% Duty Cycle"));  
          // EEPROM.write(EPPROM_ADR_SPEED_LOW, Fan.LowSpeed);   
          // EEPROM.commit();   
            NV_Mem.putUChar("NV_Mem_Fan_Low", Fan.LowSpeed);
       
        }     
        else Print_DC_Error();        
      }     

      receivedMessage = "";
    } else {
      // Append the character to the message string
      receivedMessage += incomingChar;
    }
  }
}
 void Init_NV_MemData(void){
    NV_Mem.begin("NV_Mem_Mode",false );
    NV_Mem.begin("NV_Mem_Fan_High",false );
    NV_Mem.begin("NV_Mem_Fan_Mid",false );
    NV_Mem.begin("NV_Mem_Fan_Low",false );

    uint8_t Val = NV_Mem.getUChar("NV_Mem_Mode", 0);
  if(!((Val == DEVICE_OFF) || (Val == FAN_HIGH)|| (Val == FAN_MID)|| (Val == FAN_LOW))){  
     Mode = DEVICE_OFF;// write default
      NV_Mem.putUChar("NV_Mem_Mode", Mode);
      Serial.print(F("Mode")) ;   
  }
  else Mode = Val;
  Val = NV_Mem.getUChar("NV_Mem_Fan_High", 0);
  if(!((Val > 15) && (Val < 99))){  
     Fan.HighSpeed = 80; // write default
      NV_Mem.putUChar("NV_Mem_Fan_High", Fan.HighSpeed);
      Serial.print(F("Fan.HighSpeed")) ;    
  }
  else Fan.HighSpeed = Val;
  Val = NV_Mem.getUChar("NV_Mem_Fan_Mid", 0);
  if(!((Val > 15) && (Val < 99))){  
     Fan.MidSpeed = 60; // write default
      NV_Mem.putUChar("NV_Mem_Fan_Mid", Fan.MidSpeed);
      Serial.print(F("Fan.MidSpeed")) ;    
  }
  else Fan.MidSpeed = Val;
  Val = NV_Mem.getUChar("NV_Mem_Fan_Low", 0);
  if(!((Val > 15) && (Val < 99))){  
     Fan.LowSpeed = 40; // write default
      NV_Mem.putUChar("NV_Mem_Fan_Low", Fan.LowSpeed);
      Serial.print(F("Fan.LowSpeed")) ;    
  }
  else Fan.LowSpeed = Val;

  }

 void Read_NV_Memory(void){
      uint8_t Val;

    //  NV_Mem.putUChar("Mode8", 45);
  //NV_Mem.putUChar("NV_Mem_Fan_High", Fan.HighSpeed);
  //State8= NV_Mem.getUChar("NV_Mem_Fan_High", 0);

  Val = NV_Mem.getUChar("NV_Mem_Mode", 0);
  if(!((Val == DEVICE_OFF) || (Val == FAN_HIGH)|| (Val == FAN_MID)|| (Val == FAN_LOW))){  
     Mode = DEVICE_OFF;// write default
      NV_Mem.putUChar("NV_Mem_Mode", Mode);
      Serial.print(F("Mode")) ;   
  }
  else Mode = Val;
  Val = NV_Mem.getUChar("NV_Mem_Fan_High", 0);
  if(!((Val > 15) && (Val < 99))){  
     Fan.HighSpeed = 80; // write default
      NV_Mem.putUChar("NV_Mem_Fan_High", Fan.HighSpeed);
      Serial.print(F("Fan.HighSpeed")) ;    
  }
  else Fan.HighSpeed = Val;
  Val = NV_Mem.getUChar("NV_Mem_Fan_Mid", 0);
  if(!((Val > 15) && (Val < 99))){  
     Fan.MidSpeed = 60; // write default
      NV_Mem.putUChar("NV_Mem_Fan_Mid", Fan.MidSpeed);
      Serial.print(F("Fan.MidSpeed")) ;    
  }
  else Fan.MidSpeed = Val;
  Val = NV_Mem.getUChar("NV_Mem_Fan_Low", 0);
  if(!((Val > 15) && (Val < 99))){  
     Fan.LowSpeed = 40; // write default
      NV_Mem.putUChar("NV_Mem_Fan_Low", Fan.LowSpeed);
      Serial.print(F("Fan.LowSpeed")) ;    
  }
  else Fan.LowSpeed = Val;


/*

  Val = EEPROM.read(EPPROM_ADR_MODE);
  if(!((Val == DEVICE_OFF) || (Val == FAN_HIGH)|| (Val == FAN_MID)|| (Val == FAN_LOW))){  
     Mode = DEVICE_OFF;// write default
      EEPROM.write(EPPROM_ADR_MODE, Mode); // write default
      EEPROM.commit();  
            Serial.print(F("Mode")) ;   
  }
  else Mode = Val;

  Val = EEPROM.read(EPPROM_ADR_SPEED_HIGH);
  if(!((Val > 15) && (Val < 99))){  
     Fan.HighSpeed = 80; // write default
      EEPROM.write(EPPROM_ADR_SPEED_HIGH, Fan.HighSpeed); // write default
      EEPROM.commit();  
      Serial.print(F("Fan.HighSpeed")) ;    
  }
  else Fan.HighSpeed = Val;

  Val = EEPROM.read(EPPROM_ADR_SPEED_MID);
  if(!((Val > 15) && (Val < 99))){  
     Fan.MidSpeed = 60; // write default
      EEPROM.write(EPPROM_ADR_SPEED_MID, Fan.MidSpeed); // write default
      EEPROM.commit();  
      Serial.print(F("Fan.MidSpeed")) ;   
  }
  else Fan.MidSpeed = Val;

   Val = EEPROM.read(EPPROM_ADR_SPEED_LOW);
  if(!((Val > 15) && (Val < 99))){  
     Fan.LowSpeed = 40; // write default
      EEPROM.write(EPPROM_ADR_SPEED_LOW, Fan.LowSpeed); // write default
      EEPROM.commit();  
      Serial.print(F("Fan.LowSpeed")) ;   
  }
  else Fan.LowSpeed = Val; 
  */
}
