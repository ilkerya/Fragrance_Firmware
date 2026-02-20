#include <array>
uint16_t Led_Que = 0;
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;
volatile uint32_t lastIsrAt_Prev = 0;
volatile uint32_t lastIsrAt_Diff= 0;


uint32_t Pulse_High=0;
uint32_t Pulse_Low=0;

uint32_t Pulse_High_Latch=0;
uint32_t Pulse_Low_Latch=0;
uint32_t Rotation;
uint32_t Rpm;

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

   if(digitalRead(FAN_FEEDBACK)){
    if(Pulse_Low > 0){
        Pulse_Low_Latch = Pulse_Low; 
        Pulse_Low = 0;
    }
    Pulse_High++;
   }
   else {
    if(Pulse_High > 0){
        Pulse_High_Latch = Pulse_High; 
        Pulse_High = 0;
    }
    Pulse_Low++;
   }


//  Key_Functions_Digital();

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

  pinMode(LED_BLUE, OUTPUT);  
   digitalWrite(LED_BLUE, OFF); 

   pinMode(LED_GREEN, OUTPUT);  
   digitalWrite(LED_GREEN, OFF); 

   pinMode(LED_RED, OUTPUT);  
   digitalWrite(LED_RED, OFF); 

  // analogSetWidth(11);               // 11Bit resolution

  //analogSetAttenuation(ADC_0db);
}
void Led_Control(void){

 //   digitalWrite(FAN_PWM, !digitalRead(FAN_PWM)); 

    switch(Led_Que){
      case 0:    digitalWrite(LED_GREEN, OFF); 
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
}
void SW_Read(void){
//  SW1_In = digitalRead(SWITCH_1); 
 // SW2_In = digitalRead(SWITCH_2); 
}

void DAC_Write(void){
  
   // https://www.electronicwings.com/esp32/dac-digital-to-analog-converter-esp32
   // dacWrite(BOOST_DAC_1, DAC_Val_1);//0-255 0-3V3 A1_DAC1_IO25 
  // dacWrite(BOOST_CONTROL, DAC_Val_2);//0-255 0-3V3 A1_DAC1_IO25 
 uint32_t Temp;
  //dacWrite(DAC_OUT, Boost.DAC);//0-255 0-3V3 A1_DAC1_IO25 
  /*
  if(Boost.State){
   // digitalWrite(BOOST_EN, ON); 
    digitalWrite(FAN_POWER, ON); 
      }  
  else{
   //  digitalWrite(BOOST_EN, OFF); 
     digitalWrite(FAN_POWER, OFF);  
     }    
     */
  /*
  Boost.DAC_Volt32 = Boost.DAC * 3300;
  Boost.DAC_Volt32 /= 256;
  Boost.DAC_Volt = (uint16_t)Boost.DAC_Volt32;
    // 1.24 * (1+ 110/15) + (1.24 + Vin)*110/44
#define VOLT_OFFSET 270
   Boost.Fan_Target_Volt_32  =(10333 + (124 - Boost.DAC_Volt/10)*25);//1000  mV olarak
   Boost.Fan_Target_Volt  = (uint16_t)Boost.Fan_Target_Volt_32 + VOLT_OFFSET;
*/
  Temp = Boost.DAC * 3300;
  Temp /= 256;
  Boost.DAC_Volt = (uint16_t)Temp;

  Temp = Boost.Target_DAC * 3300;
  Temp /= 256;
  Boost.Target_DAC_Volt = (uint16_t)Temp;

    // 1.24 * (1+ 110/15) + (1.24 + Vin)*110/44
#define VOLT_OFFSET 0
   Temp  =(10333 + (124 - Boost.Target_DAC_Volt/10)*25);//1000  mV olarak
   Boost.Fan_Target_Volt  = (uint16_t)Temp + VOLT_OFFSET;
  }


  #define ADC_OFFSET 2
void Boost_Read(void){
  /*      Boost.Adc = analogRead(BOOST_ADC); 
      if((ADC_OFFSET + Boost.Adc) < 4094 )Boost.Adc += ADC_OFFSET; 
      else Boost.Adc = 4094;
      
       uint32_t Temp;
     
  //    Boost.Volt_32 = Boost.Adc * 3300;   // 1920 adc => 1550 mv 1660mv 110 mV luk eksik var
  //    Boost.Volt_32 *= 57;  // 47K 10K
  //    Boost.Volt_32 /= 10; 
  //    Boost.Volt_32 /= 4096;
 //    Boost.Volt = (uint16_t)Boost.Volt_32;
      
      Temp = Boost.Adc * 3300;   // 1920 adc => 1550 mv 1660mv 110 mV luk eksik var
      Temp *= 57;  // 47K // 10K
      Temp /= 10; 
      Temp /= 4096;
      Boost.Actual_Fan_Volt = (uint16_t)Temp;
      */
}

void quicksort(uint16_t *p, uint16_t Size){
	uint16_t i, t;// unsigned int temp2;
  uint16_t temp;
	for(t = Size - 1;	t !=  0; t--){
		for(i = 0; i != t; i++){
			if(p[i] > p[i + 1]){
				temp = p[i + 1];
				p[i+ 1] = p[i];
				p[i] = temp;
			}
		}
	}
}

void Battery_Volt(void){
  /*
 // uint16_t Battery_Volt; 
  Battery.Adc = analogRead(BATTERY_ADC);
  uint32_t Temp;

 // uint32_t temp = 3300;
 // Battery.Volt_32 = Battery.Adc * 3300;
 // Battery.Volt_32 /= 4096;
 // Battery.Volt_32 /= 2048;   // 100K/100K voltage Divider
  //Battery.Volt = (uint16_t)Battery.Volt_32;

  Temp = Battery.Adc * 3300;
 // Battery.Volt_32 /= 4096;
  Temp /= 2048;   // 100K/100K voltage Divider
  Battery.Volt = (uint16_t)Temp;
  Battery.USB = analogRead(USB_DETECT_ADC);
//for-(int i = 0; i < 50 ;i++)
  Battery.Array[Battery.Index] = Battery.Volt;
  Battery.Index++;
  if(Battery.Index > ARRAY_SIZE-1){
    Battery.Index = 0;
   quicksort(&Battery.Array[0], ARRAY_SIZE);
    Battery.Median = 0;
    Battery.Min = 5000;
    Battery.Max = 0;
     for(uint8_t i = 0; i < ARRAY_SIZE; i++){
        if(Battery.Array[i] < Battery.Min) Battery.Min = Battery.Array[i];
        if(Battery.Array[i] > Battery.Max) Battery.Max = Battery.Array[i];
        Battery.Median += Battery.Array[i];    
    } 
    Battery.Median /= ARRAY_SIZE;
    Battery.Diff = Battery.Max - Battery.Min;
    if(Battery.Diff > 50) Battery.State = OFF; //off battery
    else Battery.State = ON;

    Battery.Power = ONLY_BATTERY;
    if(Battery.USB > USB_ON_LEVEL){//  USB ON
      if(Battery.State == ON)Battery.Power = BATTERY_USB ;
      else Battery.Power = ONLY_USB;
    } 
*/
/*
    for(uint8_t i = 0; i < MAX_NO; i++){
      Serial.print(i) ;Serial.print(':') ;
      Serial.print(Battery.Array[i]) ;Serial.print(' '); 
    }
    Serial.println() ;
    Serial.print("Max: ");Serial.print(Battery.Max) ;
    Serial.print("   Median: ");Serial.print(Battery.Median) ;
    Serial.print("   Min: ");Serial.print(Battery.Min) ; 
    Serial.print("   Diff: ");Serial.println(Battery.Diff) ;      
*/
   // int n = sizeof(Battery.Array) / sizeof(Battery.Array[0]);  
      // Sorting arr using inbuilt quicksort method
  //  qsort(Battery.Array, n, sizeof(int), compare);

/*
uint8_t pIndex = 0;
uint8_t pivot  = Battery.Array[49];

for(i = start; i < end - 1; i++)
{
    if (Battery.Array[i] < pivot)
    {
        swap Battery.Array[i] and Battery.Array[pIndex]
        increment pIndex by 1.
    }

    Finally, swap (Battery.Array[end], Battery.Array[pIndex]).
    return pIndex.
}
  
  }
   */
}
#define WAKEUP_GPIO_KEY2              GPIO_NUM_39     // Only RTC IO are allowed - ESP32 Pin example
RTC_DATA_ATTR int bootCount = 0;

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

void Key_1_Function() {
  Key.Task = ON;
  //Set_Position();
}
void Key_Functions_Digital(void) {
  
  Key.Key1 = digitalRead(KEY); //release 1 
    if (!Key.Key1_Rel && Key.Key1) {  // default
    Key.Status = 6;
    Key.TimerPress = 0;
     return;
  }

  if (!Key.Key1_Rel && !Key.Key1) {  // key1 pressedd   Key.Key1_Rel = 0 normally 
    Key.Key1_Rel = 1;//    0 && 0   rel && press
    Key.Status = 7;
    Key.TimerPress ++;
    return;
  }
  if (Key.Key1_Rel && !Key.Key1) {  // still pressed
     Key.Status = 8;
    Key.TimerPress ++;
  }

  if (Key.Key1_Rel && Key.Key1) {  // key released job done
    Key.Key1_Rel = 0;
    Key.Status = 9;
    Key_1_Function();
 
  }
}



