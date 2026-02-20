

void DisplayWakeUp(void) {
  if ((Display.OLED_Timer == 0) && (Display.OLED_Init == OFF)) {
    Display.OLED_Init = ON;
  }  //else if(OLED_Timer)OLED_Timer += 10; // add additional time every time any key released
}

void DispExtTimeout(void) {
  if (Display.OLED_Timer <= KEYDISP_TIMER) Display.OLED_Timer = KEYDISP_TIMER;
}
void UpdateInfoQue(uint8_t UpDown) {
#define MAXLINE 9
#define MINLINE 1
  if (UpDown == DOWNROLL) {  // down menu
    DispRollIndex[3] = DispRollIndex[2];
    DispRollIndex[2] = DispRollIndex[1];
    DispRollIndex[1] = DispRollIndex[0];
    DispRollIndex[0]++;
    if (DispRollIndex[0] > MAXLINE) DispRollIndex[0] = MINLINE;
  }
  if (UpDown == UPROLL) {
    DispRollIndex[0] = DispRollIndex[1];
    DispRollIndex[1] = DispRollIndex[2];
    DispRollIndex[2] = DispRollIndex[3];
    DispRollIndex[3]--;
    if (DispRollIndex[3] < MINLINE) DispRollIndex[3] = MAXLINE;
  }
}
void UpdateDispRoll(uint8_t UpDown) {
  if (Display.SensorRollTimer) {
    Display.SensorRollTimer--;
    return;
  }
  UpdateInfoQue(UpDown);
}
void KeySensonsorRoll(uint8_t UpDown) {
  Display.SensorRollTimer = 30;  // 2sec x 30 = 60 sec
  UpdateInfoQue(UpDown);
}
void DispEnable(bool Enable, uint8_t Timer) {
  if (Enable == ON) {
    Display.SleepEnable = ON;  //go sleep
    Display.OLED_Timer = Timer;
  } else Display.SleepEnable = OFF;  // no sleep
}
void KeyTimeOutStart(void) {
}
void Position_Mid(void) {
  Boost.Mode = FAN_MID; // 
  Boost.Mode_Prev = FAN_STANDBYE;
 //Boost.Target_DAC = 230; // 6.3 Volt
 //Boost.Target_DAC = 60; // 11.5 Volt
 //  Boost.Target_DAC = 95; // 10.8 Volt
 // Boost.Target_DAC = 100; // 10.5 Volt
  Boost.Target_DAC = 105; //  10 Volt  


  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Blue, OUTPUT);
/*
  digitalWrite(LED_Green, OFF);
  digitalWrite(LED_Red, ON);
  digitalWrite(LED_Blue, OFF);
*/
  digitalWrite(LED_Green, ON);
  digitalWrite(LED_Red, ON);
  digitalWrite(LED_Blue, OFF);


  digitalWrite(FAN_POWER, ON);
  Boost.Mode_Counter = MODE_COUNT;

  // LED_Green = 1;LED_Red = 0;LED_Blue = 0;
}
void Position_High(void) { // FAN_HIGH
  Boost.Mode = FAN_HIGH;//2
  Boost.Mode_Prev = FAN_STANDBYE;

 // Boost.Target_DAC = 43; // 12 Volt
    Boost.Target_DAC = 38; // 12,2 Volt


  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Blue, OUTPUT);
/*
  digitalWrite(LED_Green, ON);
  digitalWrite(LED_Red, OFF);
  digitalWrite(LED_Blue, OFF);
*/
  digitalWrite(LED_Green, ON);
  digitalWrite(LED_Red, OFF);
  digitalWrite(LED_Blue, ON);

  digitalWrite(FAN_POWER, ON);

  Boost.Mode_Counter = MODE_COUNT;
  // LED_Green = 0;LED_Red = 1;LED_Blue = 0;
}
void Position_Low(void) {
  Boost.Mode = FAN_LOW;  // 3
  Boost.Mode_Prev = FAN_STANDBYE;

     // Boost.Target_DAC = 190; // 7.6 Volt
  //  Boost.Target_DAC = 200; // 7.2 Volt
   //    Boost.Target_DAC = 205; //  Volt
  //   Boost.Target_DAC = 210; // 6.9 Volt   
   Boost.Target_DAC = 198; // 6.9 Volt   




  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Blue, OUTPUT);
  /*
  digitalWrite(LED_Green, OFF);
  digitalWrite(LED_Red, OFF);
  digitalWrite(LED_Blue, ON);
*/
    digitalWrite(LED_Green, OFF);
  digitalWrite(LED_Red, ON);
  digitalWrite(LED_Blue, ON);
  //LED_Green = 0;LED_Red = 0;LED_Blue = 1;
  digitalWrite(FAN_POWER, ON);
  Boost.Mode_Counter = MODE_COUNT;
}
void Fan_Standbye(void) {
  Boost.Mode = 64;
  digitalWrite(FAN_POWER, OFF);
  Boost.Target_DAC = 240;
  /*
  digitalWrite(LED_Green, OFF);
  digitalWrite(LED_Red, OFF);
  digitalWrite(LED_Blue, OFF);
  */
  //digitalWrite(LED_Green, ON);
  //digitalWrite(LED_Red, ON);
  //digitalWrite(LED_Blue, ON);
  Boost.Mode_Counter = MODE_COUNT;
}
void Device_OFF(void) {
  Boost.Mode = 0;
  digitalWrite(FAN_POWER, OFF);
  Boost.Target_DAC = 240;
  /*
  digitalWrite(LED_Green, OFF);
  digitalWrite(LED_Red, OFF);
  digitalWrite(LED_Blue, OFF);
  */
  digitalWrite(LED_Green, ON);
  digitalWrite(LED_Red, ON);
  digitalWrite(LED_Blue, ON);
  Boost.Mode_Counter = MODE_COUNT;
  Set_Sleep();
}

void Set_Position(void) {
  if(Boost.Mode == FAN_STANDBYE){
    Boost.Mode  = Boost.Mode_Prev;
  }
  Boost.Mode++;
  if (Boost.Mode > 3) Boost.Mode = 0;
  Boost.Mode_Prev = 0;

  switch (Boost.Mode) {
    case DEVICE_OFF:
      Device_OFF();  // 230
      break;

    case FAN_MID:
      Position_Mid();  // 
      break;

    case FAN_HIGH:
      Position_High();  // 
      break;

    case FAN_LOW:
      Position_Low();  // 
      break;

    default:
      Device_OFF();
      break;
  }
}

void Key_4_Function() {
  Set_Position();
}
void Key_2_Function() {
  Set_Position();
}
void Key_3_Function() {
  //Set_Position();
}
void Key_1_Function() {
  //Set_Position();
}

void Key_Functions_Digital(void) {
  bool Key1, Key2, Key3, Key4;
  Key1 = digitalRead(KEY_1);
  Key2 = digitalRead(KEY_2);
  Key3 = digitalRead(KEY_3);
  Key4 = digitalRead(KEY_4);
  /*
  Serial.print("Key1: ");Serial.print(Key1);
  Serial.print("   Key2: ");Serial.print(Key2);
  Serial.print("   Key3: ");Serial.print(Key3);
  Serial.print("   Key4: ");Serial.println(Key4);
  */
  /*
  if (!Key.Key1_Rel && !Key.Key2_Rel && !Key.Key3_Rel && !Key.Key4_Rel) {
    Key.Released = ON;
  }
  */
  if (!Key.Key1_Rel && !Key.Key2_Rel && !Key.Key3_Rel && !Key.Key4_Rel) {
    Key.Released = ON;
  }

  if (Key.Released && !Key1) {  // key1 press
    Key.Key1_Rel = ON;
    return;
  }
  if (Key.Key1_Rel && Key1) {  // key1 release
    Key.Key1_Rel = OFF;
    DisplayWakeUp();
    Key_1_Function();
    KeyTimeOutStart();
    return;
  }

  if (Key.Released && !Key2) {
    Key.Key2_Rel = ON;
    return;
  }
  if (Key.Key2_Rel && Key2) {
    Key.Key2_Rel = OFF;
    DisplayWakeUp();
    Key_2_Function();
    KeyTimeOutStart();
    return;
  }

  if (Key.Released && !Key3) {
    Key.Key3_Rel = ON;
    return;
  }
  if (Key.Key3_Rel && Key3) {
    Key.Key3_Rel = OFF;
    DisplayWakeUp();
    Key_3_Function();
    KeyTimeOutStart();
    return;
  }

  if (Key.Released && !Key4) {
    Key.Key4_Rel = ON;
    return;
  }
  if (Key.Key4_Rel && Key4) {
    Key.Key4_Rel = OFF;
    DisplayWakeUp();
    Key_4_Function();
    KeyTimeOutStart();
    return;
  }
}
