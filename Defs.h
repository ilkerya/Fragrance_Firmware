//  https://www.ditig.com/256-colors-cheat-sheet

/*
First Time
git init
git add README.md
git add Branch main

GitHub Bash Commands
Adding new file
https://help.github.com/articles/adding-a-file-to-a-repository-using-the-command-line/#platform-windows

git add .

git commit -m "Add existing file"
// For Common git
git push https://github.com/ilkerya/Fragrance_Firmware main

 
In case in the first push gives error use below command
git remote add origin remote repository URL
git push origin master
git push --force https://github.com/ilkerya/Fragrance_Firmware main --force

To create a branch:
$ git branch <branch_name> 
To switch to that branch:
$ git checkout <same_branch_name> 
To do the above operations in one line, it will create and switch branch:
$ git checkout -b <new_branch> 
For example, say you want to create a branch named `learning_git`
$ git branch learning_git 
$ git checkout learning_git 
or
$ git checkout -b learning_git 
I2C scanner. Scanning ...
Found address: 26 (0x1A)
Found address: 56 (0x38) //aht20
Found address: 83 (0x53)
Found 3 device(s).
*/
#define uint8_t byte
#define uint16_t unsigned int
#define uint32_t unsigned long

#define int8_t signed char
#define int16_t signed int
#define int32_t signed long

#define EEPROM_SIZE 16
#define  EPPROM_ADR_MODE 8
#define  EPPROM_ADR_SPEED_HIGH 1
#define  EPPROM_ADR_SPEED_MID 2
#define  EPPROM_ADR_SPEED_LOW 3

#define ON 1
#define OFF 0

#define MODE_COUNT 3000 //  1000->100sec  600->60sec    5 minutes 5*60= 300sec->3000 
//#define MODE_COUNT 200 //  20sec  

//#define FAN_STANDBYE 64
#define DEVICE_OFF 0
#define FAN_LOW 1
#define FAN_MID 2
#define FAN_HIGH 3

#define ONLY_BATTERY 32
#define ONLY_USB     16
#define BATTERY_USB  64
#define USB_ON_LEVEL  1024  // adc level    5V  47K / 10 K GND


#define ESP32_FRAGRANCE
//#define ESP32_ADAFRUIT

#ifdef ESP32_ADAFRUIT
#define LED_PIN 13  // GPIO13_A12 TOUCH4
//#define BOOST_EN 12 // 
//#define BOOST_EN 32 
#define BATTERY_ADC 35 //I35/A1_7 A13_I35
#define BOOST_DAC_1 25
#define FAN_POWER 12 // IO12_A11 OE  TOUCH5

#define BOOST_ADC 36 // A5_IO4 Fan Voltage
//#define BOOST_CONTROL 26 // DAC Output
#define DAC_OUT 26 // DAC Output

#define I2C_1_PWR 14  // TOUCH6 
#define I2C_2_PWR 15 // TOUCH3 
//#define SWITCH_1 34
//#define SWITCH_2 39

#define USB_DETECT_ADC 33 // ADC 9 IO33 JP3->7  KICAD-> IO22 

  #define KEY_1 34 //34 RTC_GPIO4 
  #define KEY_2 39// RTC_GPIO3 
  #define KEY_3 18// no
  #define KEY_4 19// no

/*
JP3
IO13_A12 LED_PIN
IO12_A11
IO27_A10 OLED_DATA
IO33_A9
IO15_A8 I2C_2_PWR
IO32_A7 BOOST_EN
IO14_A6 I2C_1_PWR
SCL     I2C
SDA     I2C

JP1
A0_DAC2_IO26 BOOST_DAC_2
A1_DAC1_IO25 BOOST_DAC_1 
A2_I34 SWITCH_1
A3_I39 SWITCH_2
A4_IO36 OLED_CLK
A5_IO4 4 BOOST_ADC
SCK IO5
MOSI IO19
MISO IO18
IO16  OLED_DC
IO17  OLED_RESET
IO21  OLED_CS
*/

#endif

#ifdef ESP32_FRAGRANCE
/*
#define LED_PIN 13  // GPIO13_A12 TOUCH4
#define BATTERY_ADC 35 //I35/A1_7 A13_I35
#define FAN_POWER 12 // IO12_A11 OE  TOUCH5
#define I2C_1_PWR 14  // TOUCH6 
#define I2C_2_PWR 15 // TOUCH3 
#define USB_DETECT_ADC 33 // ADC 9 IO33 JP3->7  KICAD-> IO22 
*/
///#define I2C_1_PWR 14  // TOUCH6 



  #define BAT_CHARGE 22 //
  #define BAT_STANDBYE 21 //
  //#define BAT_VOLT 15 //34 RTC_GPIO4 eski hali yeri suan bos
   #define FAN_PWM  15 //SENSOR_VN  39 du
  //pinMode(BAT_VOLT, INPUT);  
  //#define USB_DETECT 35 // 39//SENSOR_VN ile yer degisti  
 //   pinMode(USB_DETECT, INPUT);  
  #define FAN_FEEDBACK  36//SENSOR_VP 
  #define BOOST_CONV_POWER 2 //34 RTC_GPIO4 

  #define KEY 4
  #define LED_CANDLE 34//IO23
  
  #define SENSOR_3V_POWER 23//IO23
      #define SENSOR_3V_ENABLE 0// FALSE Logic
      #define SENSOR_3V_DISABLE 1// 
  
 // #define LED_BLUE 5 //34 RTC_GPIO4 
 // #define LED_GREEN 17// RTC_GPIO3 
 // #define LED_RED 16// no

  uint8_t LED_BLUE = 5; //34 RTC_GPIO4 
  uint8_t LED_GREEN = 17;// RTC_GPIO3 
  uint8_t LED_RED = 16;// no

  #define SDA  18
  #define SCL  19

#endif

  #define MIN_PWM  18
  // 255/5=52

/*
  #define TASK_1MSEC     1000
  #define TASK_5MSEC     5000   
  #define TASK_10MSEC   10000
  #define TASK_20MSEC   20000
  #define TASK_50MSEC   50000
  #define TASK_100MSEC 100000
  #define TASK_250MSEC 250000
  #define TASK_500MSEC 500000
  #define TASK_1SEC   1000000
  
  #define TASK_2SEC   2000000
  #define TASK_5SEC   5000000
  #define TASK_10SEC 10000000
  #define TASK_20SEC 200
  #define TASK_60SEC 202 
  */
// 001BC5067010E312
/*
typedef  byte               uint8;
typedef  signed char        int8;
typedef  unsigned char      uint8;
//typedef  signed short       int16;
//typedef  unsigned short     uint16;
typedef  signed int         int16;
typedef  unsigned int       uint16;
typedef  signed long        int32;
typedef  unsigned long      uint32;
typedef  signed long long   int64;
typedef  unsigned long long uint64
C:\Program Files (x86)\Arduino\libraries
*/


  #define ANALOG_RES_12BIT
  //#define ANALOG_RES_10BIT



 // #define DEBUG_SIMULATOR_MODE // For DEbugging As A Simulator
// Select Hardware Type
//#define FIRST_PROTOTYPE  // with LEM current Transdcucer





#define ON 1 //
#define OFF 0 //

void Led_Control(void);

void Fan_Standbye(void);
void Device_OFF(void);
void Position_Mid(void);
void Position_High(void);
void Position_Low(void);
void PI_Control(void);
// function prototypes
void Key_Functions_Digital(void);
void Common_Loop(); 
void ResetCasePrint();
void IO_Settings();
void MicroInit(void);
void Display_ReInit_Start(uint8_t Timer);
void Display_ReInit_End(void);

void SD_Card_Info(void);
void SD_Card_Init(void);
void SD_Card_Data_Preparation(void);
void SD_Card_Header_Preparation(void);

void RTC_Init();
void SensorInit_Si072(uint8_t);
void SensorAlt_Init();
void SensorLight_Init();
void SensorACccel_GyroInit();
void Sensor_LidarInit(void);
void Sensors_PeripInit();

void CurrentVolt_Read(void);
void AdcRead(void);
void WindSensorRead(void);
void SensorRead_Si072(unsigned char);
void SensorAlt_Read(void);
void SensorLight_Read(void);
void SensorAcccel_GyroRead(void);
void Sensor_LidarRead(void);
void SDS_DustSensor(void);
void UpdateSensorInfo(void);

void UpdateInfoLine();
void UpdateDisplayMenu();
void UpdateSD_LogTime();
void UpdateFileSize();
void ConvertFileSize(uint32_t);// Line3  
void UpdateProperLine(uint8_t Index, uint8_t Line);

void  RTC_TimeClock(void);

void EscMenuKey(void);
void EnterMenuKey(void);
void DownMenuKey(void);
void UpMenuKey(void);
void SetSampling(uint16_t Time);
void DispEnable(bool Enable, uint8_t Timer);
void DispEnable_4SD_Prblm(bool Enable, uint8_t Timer);

void  DispExtTimeout(void);
void   DisplayMenu(void);
void KeyTimeOutCheck(void);
void SD_CardLogTask(void);
void SD_Log_File(void);
void SD_Info_Only(void);
void DisplayFullSensors(void);
void DisplayTestDevices(void);
void SerialPortRx(void);
void UpdateDispRoll(uint8_t);
void Log_Data_Write_SD(void);

void Parse_FileString(void);
void Relay_loop(void) ;
float GetValue(uint8_t Relay);
String LimitCopyDisplayStr(String str, uint8_t MaxNumber);
void EnergyMeterIC_Operation(void);
void I2_ACK_Reset(void);

void SetResetLog(bool Enable);
void NVRam_Write_LogStatus(bool Mode);
uint8_t NVRam_Read(uint8_t Address);
void NVRam_Write(uint8_t Address, uint8_t Sample); // EE_SAMPLE
void NVRam_Read_Standbye(void);
void NVRam_Write_Standbye(bool Mode);
void NVRam_Read_MainsFreq(void);
void NVRam_Write_MainsFreq(bool Mode);
void NVRam_Write_MaxFileSize(uint8_t Size);
uint8_t NVRam_Read_MaxFileSize(void);

void NVRam_Read_SerNo(void);
void NVRam_Write_SerNo(char* p);
void NVRam_Read_QueNo(void);
void NVRam_Write_QueNo(char* p);
char NVRam_Check_Content(char Content, bool Number);

void UpdateLogFileId(void);
char* CopyFlashToRam(const char* );

void Due_Memory();
void Print_ARM_SPI_Regs(void);

void UpdateLogFileNo(void);
void UpdateLogFileId(void);
void Startup_NV_Ram(void);

void EEProm_Update_FileNo(void);
void EEProm_Update_DevId(void);
void EEProm_Update_Debug(bool);

bool Log_Escape(void);

/*
C:\Users\ilker\Documents\Atmel Studio\7.0\ArduinoSketch6\ArduinoSketch6\ArduinoCore\src\libraries\SD\utility\Sd2Card.cpp 
 // send command and return error code.  Return zero for OK
uint8_t Sd2Card::cardCommand(uint8_t cmd, uint32_t arg) {
  chipSelectLow();


  static uint8_t chip_select_asserted = 0;

void Sd2Card::chipSelectHigh(void) {
  digitalWrite(chipSelectPin_, HIGH);
  #ifdef USE_SPI_LIB
  if (chip_select_asserted) {
    chip_select_asserted = 0;
    SDCARD_SPI.endTransaction();
  }
  #endif
}
//------------------------------------------------------------------------------
void Sd2Card::chipSelectLow(void) {
  #ifdef USE_SPI_LIB
  if (!chip_select_asserted) {
    chip_select_asserted = 1;
    SDCARD_SPI.beginTransaction(settings);
  }
  #endif
  digitalWrite(chipSelectPin_, LOW);
}


 * /
 */
