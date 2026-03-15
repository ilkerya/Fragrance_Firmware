void Key_Functions_Digital(void) {
  Key.Key1 = digitalRead(KEY); //release 1
  if (!Key.Key1_Rel && Key.Key1) {  // default
    Key.TimerPress = 0;
     return;
  }
  if (!Key.Key1_Rel && !Key.Key1) {  // key1 pressedd   Key.Key1_Rel = 0 normally
    Key.Key1_Rel = 1;//    0 && 0   rel && press
    Key.TimerPress ++;
    return;
  }
  if (Key.Key1_Rel && !Key.Key1) {  // still pressed
    Key.TimerPress ++;
    if(Key.TimerPress > 350)ESP.restart(); //20ms*350 = 7000mS 7 sec
  }
  if (Key.Key1_Rel && Key.Key1) {  // key released job done
    Key.Key1_Rel = 0;
    if((Key.Inhibit_Timer == 0) && (!Key.Inhibit)){
      System.Mode++;
      if(System.Mode <= RUN_TEST_LIMIT){
        if(System.Mode > RUN_HIGH)System.Mode = RUN_OFF;
        Reset_Run_Modes();
      }
      else{
        if(System.Mode > TEST_HIGH)System.Mode = TEST_OFF;    
      }
      Key.Task = ON;
    }
    if(Key.Inhibit)Key.Inhibit = OFF;
   }
}
#define SECONDS_10 10
 void Reset_Run_Modes(void) {
  System.Index = 0;
  System.RunTimer = 0;
  System.Index_UpdateTimer = SECONDS_10;
}

#define TIMESCALE 600 // 60 minutes x 100msec = 60x10 = 600
bool Run_Mode_Timer(uint8_t*p) {
  uint8_t i;
  bool FanStatus = OFF;
   System.RunTimer++;
  if(System.Index> (p[4])){
    Reset_Run_Modes();
    System.Mode = RUN_OFF;
    return OFF;
  }
  if(System.Index >= 4) i = System.Index % 4; // 4 cycles
  else  i = System.Index ;
  if((System.Index & 0X01) == 0X00){ // even number
     FanStatus = ON;
     digitalWrite(BOOST_CONV_POWER, ON);   
      if(System.RunTimer > (p[i]*TIMESCALE)){
        System.RunTimer = 0;
        System.Index++;
        System.Index_UpdateTimer = SECONDS_10;
      }
  }
  else { // odd
      Fan.DutyCycle = PWM_MINIMUM;   
      digitalWrite(BOOST_CONV_POWER, OFF);// 
      FanStatus = OFF;    
      if(System.RunTimer > (p[i]*TIMESCALE)){
        System.RunTimer = 0;
        System.Index++;
        System.Index_UpdateTimer = SECONDS_10;
      } 
  }
  return FanStatus;
 }

 void Mode_Select(void) {
  if(System.Light_Sleep || System.RTC_Sleep){
      Fan.DutyCycle = PWM_MINIMUM;   
      ledcWrite(FAN_PWM, 255-((Fan.DutyCycle*255)/100) ); 
      digitalWrite(BOOST_CONV_POWER, OFF);// 
      Led.Color = 0; //Black
      ledcWrite(LED_RED, 0);  // write red component to channel 1, etc.
      ledcWrite(LED_GREEN, 0);
      ledcWrite(LED_BLUE, 0);
      Key.Inhibit = ON;
      if(System.Light_Sleep){
        System.Light_Sleep = OFF;  
        Set_Light_Sleep();
      }
      if(System.RTC_Sleep){
        System.RTC_Sleep = OFF;   
        Set_RTC_Sleep();
      }    
         
    return;
  }
  switch(System.Mode){
    case TEST_OFF :
    case RUN_OFF :
      Fan.DutyCycle = PWM_MINIMUM;   
      digitalWrite(BOOST_CONV_POWER, OFF);// 
      Led.Color = 0; //Black
      ledcWrite(LED_RED, 0);  // write red component to channel 1, etc.
      ledcWrite(LED_GREEN, 0);
      ledcWrite(LED_BLUE, 0);
      break;
    case TEST_HIGH :     
      Fan.DutyCycle =Fan.HighSpeed; 
      digitalWrite(BOOST_CONV_POWER, ON);
      Led.Color = Led.ColorHigh;
      break;
    case TEST_MID : 
      Fan.DutyCycle = Fan.MidSpeed; 
      digitalWrite(BOOST_CONV_POWER, ON);
      Led.Color = Led.ColorMid;
      break;
    case TEST_LOW : 
      Fan.DutyCycle =Fan.LowSpeed;  
       digitalWrite(BOOST_CONV_POWER, ON);
      Led.Color = Led.ColorLow;
      break;
    case RUN_HIGH :
      if(Run_Mode_Timer(&System.Time_High[0])) Fan.DutyCycle =Fan.HighSpeed; 
      Led.Color = Led.ColorHigh;
      break;
    case RUN_MID :
       if(Run_Mode_Timer(&System.Time_Mid[0]))Fan.DutyCycle =Fan.MidSpeed; 
      Led.Color = Led.ColorMid;
      break;
    case RUN_LOW :
      if(Run_Mode_Timer(&System.Time_Low[0]))Fan.DutyCycle =Fan.LowSpeed; 
      Led.Color = Led.ColorLow;
      break;
    default:
    break;
    }
   if((System.Mode != TEST_OFF) || (System.Mode != RUN_OFF)){
      digitalWrite(BOOST_CONV_POWER, ON);
     SetColor(Led.Color,Led.Bright); // Color // brightness
     ledcWrite(FAN_PWM, 255-((Fan.DutyCycle*255)/100) ); 
  }
}
void hueToRGB(uint8_t hue, uint8_t brightness) {
  uint16_t scaledHue = (hue * 6);
  uint8_t segment = scaledHue / 256;                     // segment 0 to 5 around the
                                                         // color wheel
  uint16_t segmentOffset = scaledHue - (segment * 256);  // position within the segment

  uint8_t complement = 0;
  uint16_t prev = (brightness * (255 - segmentOffset)) / 256;
  uint16_t next = (brightness * segmentOffset) / 256;

  if (Led.invert) {
    brightness = 255 - brightness;
    complement = 255;
    prev = 255 - prev;
    next = 255 - next;
  }

  switch (segment) {
    case 0:  // red
      Led.R = brightness;
      Led.G = next;
      Led.B = complement;
      break;
    case 1:  // yellow
      Led.R = prev;
      Led.G = brightness;
      Led.B = complement;
      break;
    case 2:  // green
      Led.R = complement;
     Led.G = brightness;
      Led.B = next;
      break;
    case 3:  // cyan
      Led.R = complement;
      Led.G = prev;
      Led.B = brightness;
      break;
    case 4:  // blue
      Led.R = next;
      Led.G = complement;
      Led.B = brightness;
      break;
    case 5:  // magenta
    default:
      Led.R = brightness;
      Led.G = complement;
      Led.B = prev;
      break;
  }
}
void  SetColor(uint8_t Col,uint8_t Brg){
    hueToRGB(Col, Brg);  // call function to convert hue to RGB
    // write the RGB values to the pins
    ledcWrite(LED_RED, Led.R);  // write red component to channel 1, etc.
    ledcWrite(LED_GREEN, Led.G);
    ledcWrite(LED_BLUE, Led.B);
}
/*
void Scanner ()
{
  Serial.println ();
  Serial.println (F("I2C scanner. Scanning ..."));
  byte count = 0;

  //Wire.begin();
    Wire.begin (SDA, SCL);   // sda= GPIO_18 /scl= GPIO_19
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response)
    {
      Serial.print (F("Found address: "));
      Serial.print (i, DEC);
      Serial.print (F(" (0x"));
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print (F("Found "));
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (F(" device(s)."));
}
*/

