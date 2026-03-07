#include <array>
uint16_t Led_Que = 0;
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;
volatile uint32_t lastIsrAt_Prev = 0;
volatile uint32_t lastIsrAt_Diff= 0;

#define WDT_TIMEOUT 10000 // 10000mS = 10 second... ->
//#define CONFIG_FREERTOS_NUMBER_OF_CORES 1
void WatchdogTimer_Set(){
  esp_task_wdt_config_t twdt_config =
    {
        .timeout_ms = WDT_TIMEOUT,
        .idle_core_mask = 0,    // Bitmask of cores
        .trigger_panic = true,
    };
  esp_task_wdt_deinit(); //wdt is enabled by default, so we need to 'deinit' it first
  esp_task_wdt_init(&twdt_config); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
}

void ARDUINO_ISR_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter = isrCounter + 1;
 // lastIsrAt = millis();
  lastIsrAt = micros();
  lastIsrAt_Diff = lastIsrAt - lastIsrAt_Prev;
  lastIsrAt_Prev = lastIsrAt;
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
   //1 msec


  Loop_1SecCounter++;
  if(Loop_1SecCounter >= 100000){
    Loop_1SecCounter = 0;
    LOOP_1Second = ON;
  }
  Loop_20mSecCounter++;
  if(Loop_20mSecCounter >= 2000){
    Loop_20mSecCounter = 0;
    LOOP_20mSec = ON;
   Key_Functions_Digital();
  }
#define TACHO_ERROR 30000 // low tahn 100 rpm
   if(digitalRead(FAN_FEEDBACK)){
    if(Fan.Pulse_Low > 0){
        Fan.Pulse_Low_Latch = Fan.Pulse_Low;
        Fan.Pulse_Low = 0;
        Fan.Error = OFF;
    }
    Fan.Pulse_High++;
    if(Fan.Pulse_High > TACHO_ERROR){ //error
      Fan.Error = ON;
      //Fan.Pulse_High_Latch = TACHO_ERROR;
     // Fan.Pulse_Low_Latch = TACHO_ERROR;
    }
   }
   else {
    if(Fan.Pulse_High > 0){
        Fan.Pulse_High_Latch = Fan.Pulse_High;
        Fan.Pulse_High = 0;
        Fan.Error = OFF;
    }
    Fan.Pulse_Low++;
    if(Fan.Pulse_Low > TACHO_ERROR){ //error
      Fan.Error = ON;
     // Fan.Pulse_High_Latch = TACHO_ERROR;
     // Fan.Pulse_Low_Latch = TACHO_ERROR;
    }
   }

}
void Interrupt_Set(void){
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();
  // Set timer frequency to 1Mhz resolution
  timer = timerBegin(1000000);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer);
  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
 // timerAlarm(timer, TIME_USEC, true, 0);// 100000 usec => 1000 msec => 1 sec
  #define TIME_1MSEC 1000 // 1 msec resolution
  #define TIME_100USEC 100 // 1 msec resolution
  #define TIME_10USEC 10 // 1 msec resolution
  #define TIME_1USEC 1 // 1 msec resolution

  timerAlarm(timer, TIME_10USEC, true, 0);// 1000 usec => 1 msec
 // pinMode(BTN_STOP_ALARM, INPUT);
}
/*
void SetDutyCycle(uint8_t DC){
  //if(Mode == TEST_FRAG)
  Fan.DutyCycle = DC;
            EEPROM.write(EPPROM_ADR_SPEED_HIGH, Fan.HighSpeed); 
            EEPROM.commit();  

  Key.Task = ON;
}
*/





 
void Key_Function(){
 //   Mode++;
 //   if(Mode > FAN_HIGH)Mode = DEVICE_OFF;
  
  }

void Mode_Select() {
  //  if(!Key.Task) return;
  //  Key.Task = OFF;

      if(Mode == DEVICE_OFF) {
        Fan.DutyCycle = 1;   
        digitalWrite(BOOST_CONV_POWER, OFF);
        Led.Color = 0; //Black
        ledcWrite(LED_RED, 0);  // write red component to channel 1, etc.
        ledcWrite(LED_GREEN, 0);
        ledcWrite(LED_BLUE, 0);
      }  
      if(Mode == FAN_HIGH)   {
        Fan.DutyCycle =Fan.HighSpeed; 
        digitalWrite(BOOST_CONV_POWER, ON);
        Led.Color = Led.ColorHigh;
      }
      if(Mode == FAN_MID) {
        Fan.DutyCycle = Fan.MidSpeed; 
        digitalWrite(BOOST_CONV_POWER, ON);
        Led.Color = Led.ColorMid;
      }
      if(Mode == FAN_LOW)    {
        Fan.DutyCycle =Fan.LowSpeed;  
        digitalWrite(BOOST_CONV_POWER, ON);
        Led.Color = Led.ColorLow;
      }
   //     EEPROM.write(EPPROM_ADR_MODE, Mode); // write default
    //    EEPROM.commit();
       //   NV_Mem.putUChar("NV_Mem_Mode", Mode);

       // Led_Control();
 }


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
     Mode++;
    if(Mode > 3)Mode = 0;
       Key.Task = ON;
      // NV_Mem.putUChar("NV_Mem_Mode", Mode);
    //   Key.EEPROM_Task = ON;
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

void  Init_IO(void){

  pinMode(BAT_CHARGE, INPUT);
  pinMode(BAT_STANDBYE, INPUT);
  pinMode(FAN_FEEDBACK, INPUT);
  pinMode(KEY, INPUT);
  pinMode (KEY, INPUT_PULLUP);

  pinMode(FAN_PWM, OUTPUT);
  digitalWrite(FAN_PWM, OFF);

   ledcAttach(FAN_PWM, 25000, 8);

  pinMode(BOOST_CONV_POWER, OUTPUT);
  digitalWrite(BOOST_CONV_POWER, ON);

  pinMode(LED_CANDLE, OUTPUT);
  digitalWrite(LED_CANDLE, ON);

  pinMode(SENSOR_3V_POWER, OUTPUT);
       digitalWrite(SENSOR_3V_POWER, SENSOR_3V_DISABLE);
  //digitalWrite(SENSOR_3V_POWER, SENSOR_3V_ENABLE);
/*
  pinMode(LED_BLUE, OUTPUT);
   digitalWrite(LED_BLUE, OFF);

   pinMode(LED_GREEN, OUTPUT);
   digitalWrite(LED_GREEN, OFF);

   pinMode(LED_RED, OUTPUT);
   digitalWrite(LED_RED, OFF);
*/
  ledcAttach(LED_RED, 12000, 8);  // 12 kHz PWM, 8-bit resolution
  ledcAttach(LED_GREEN, 12000, 8);
  ledcAttach(LED_BLUE, 12000, 8);

  // analogSetWidth(11);               // 11Bit resolution

  //analogSetAttenuation(ADC_0db);
}
void Led_Control(void){
  /*
  if(Mode == DEVICE_OFF) {
    digitalWrite(LED_GREEN, OFF);
    digitalWrite(LED_BLUE, OFF);
    digitalWrite(LED_RED, OFF);
  }
  if(Mode == FAN_HIGH)   {
    digitalWrite(LED_GREEN, ON);
    digitalWrite(LED_BLUE, ON);
    digitalWrite(LED_RED, ON);
  }
  if(Mode == FAN_MID) {
    digitalWrite(LED_GREEN, OFF);
    digitalWrite(LED_BLUE, ON);
    digitalWrite(LED_RED, OFF);
  }
  if(Mode == FAN_LOW)    {
    digitalWrite(LED_GREEN, ON);
    digitalWrite(LED_BLUE, OFF);
    digitalWrite(LED_RED, OFF);
  }
*/
 //   digitalWrite(FAN_PWM, !digitalRead(FAN_PWM));
/*
    switch(Led_Que){
      case 0:  digitalWrite(LED_GREEN, OFF);
              digitalWrite(LED_BLUE, OFF);
              digitalWrite(LED_RED, ON);
        break;
      case 1:   digitalWrite(LED_GREEN, OFF);
              digitalWrite(LED_BLUE, ON);
               digitalWrite(LED_RED, OFF);
        break;
      case 2:   digitalWrite(LED_GREEN, ON);
              digitalWrite(LED_BLUE, OFF);
              digitalWrite(LED_RED, OFF);
        break;
    }
    Led_Que++;
    if(Led_Que > 2){ //1500 msec
        Led_Que = 0;
    }
    */
}
 

//RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
#define WAKEUP_GPIO_KEY2              GPIO_NUM_39     // Only RTC IO are allowed - ESP32 Pin example
void Set_Sleep(void){
  esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO_KEY2, 0);  //1 = High, 0 = Low
  /*
  rtc_gpio_pullup_dis(WAKEUP_GPIO_KEY2);
  rtc_gpio_pulldown_en(WAKEUP_GPIO_KEY2);
*/
  rtc_gpio_pullup_en(WAKEUP_GPIO_KEY2);
  rtc_gpio_pulldown_dis(WAKEUP_GPIO_KEY2);

/*
#if USE_EXT0_WAKEUP
  esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO, 0);  //1 = High, 0 = Low
  // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
  // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
  // No need to keep that power domain explicitly, unlike EXT1.
  rtc_gpio_pullup_dis(WAKEUP_GPIO);
  rtc_gpio_pulldown_en(WAKEUP_GPIO);

#else  // EXT1 WAKEUP
  //If you were to use ext1, you would use it like
  esp_sleep_enable_ext1_wakeup_io(BUTTON_PIN_BITMASK(WAKEUP_GPIO), ESP_EXT1_WAKEUP_ANY_HIGH);

  //  If there are no external pull-up/downs, tie wakeup pins to inactive level with internal pull-up/downs via RTC IO
   //      during deepsleep. However, RTC IO relies on the RTC_PERIPH power domain. Keeping this power domain on will
   //      increase some power consumption. However, if we turn off the RTC_PERIPH domain or if certain chips lack the RTC_PERIPH
  //       domain, we will use the HOLD feature to maintain the pull-up and pull-down on the pins during sleep.

  rtc_gpio_pulldown_en(WAKEUP_GPIO);  // GPIO33 is tie to GND in order to wake up in HIGH
  rtc_gpio_pullup_dis(WAKEUP_GPIO);   // Disable PULL_UP in order to allow it to wakeup on HIGH
#endif
*/
  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");

}
void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  //Wire.begin();
    Wire.begin (SDA, SCL);   // sda= GPIO_18 /scl= GPIO_19
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}





